#include "elizaos/agentloop.hpp"
#include <chrono>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>

namespace elizaos {

AgentLoop::AgentLoop(const std::vector<LoopStep>& steps, bool paused, double stepInterval)
    : steps_(steps), stepInterval_(stepInterval), stopRequested_(false),
      pauseRequested_(paused), running_(false), started_(false), stepSignaled_(false),
      inputHandlingEnabled_(false) {
    // Initialize statistics
    stats_.minStepDurationMs = std::numeric_limits<double>::max();
}

AgentLoop::~AgentLoop() {
    enableInputHandling(false); // Stop input handling first
    stop();
}

void AgentLoop::start() {
    if (running_) {
        return; // Already running
    }

    // Update health status
    HealthStatus oldStatus = healthStatus_.exchange(HealthStatus::STARTING);
    if (healthChangeCallback_) {
        std::lock_guard<std::mutex> lock(callbackMutex_);
        if (healthChangeCallback_) {
            healthChangeCallback_(oldStatus, HealthStatus::STARTING);
        }
    }

    stopRequested_ = false;
    running_ = true;
    started_ = false;

    // Initialize start time for statistics
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        stats_.startTime = std::chrono::steady_clock::now();
        stats_.lastStepTime = stats_.startTime;
    }

    loopThread_ = std::make_unique<std::thread>(&AgentLoop::runLoop, this);

    // Wait for loop to start with timeout (prevents indefinite blocking)
    std::unique_lock<std::mutex> lock(startedMutex_);
    bool startSucceeded = startedEvent_.wait_for(lock, std::chrono::seconds(5), 
        [this] { return started_.load(); });
    
    if (!startSucceeded) {
        // Loop thread failed to start in time - clean up and throw
        // Note: Thread detach leaves it running but is preferable to blocking forever
        stopRequested_ = true;
        running_ = false;
        if (loopThread_ && loopThread_->joinable()) {
            loopThread_->detach();
        }
        throw std::runtime_error("AgentLoop failed to start within 5 seconds");
    }

    // Update health status to HEALTHY
    oldStatus = healthStatus_.exchange(HealthStatus::HEALTHY);
    if (healthChangeCallback_) {
        std::lock_guard<std::mutex> cbLock(callbackMutex_);
        if (healthChangeCallback_) {
            healthChangeCallback_(oldStatus, HealthStatus::HEALTHY);
        }
    }
}

void AgentLoop::stop() {
    if (!running_) {
        return;
    }

    // Update health status
    HealthStatus oldStatus = healthStatus_.exchange(HealthStatus::STOPPING);
    if (healthChangeCallback_) {
        std::lock_guard<std::mutex> lock(callbackMutex_);
        if (healthChangeCallback_) {
            healthChangeCallback_(oldStatus, HealthStatus::STOPPING);
        }
    }

    stopRequested_ = true;

    // Signal std::any waiting conditions
    {
        std::lock_guard<std::mutex> lock(stepMutex_);
        stepSignaled_ = true;
    }
    stepEvent_.notify_all();

    if (loopThread_ && loopThread_->joinable()) {
        loopThread_->join();
    }

    running_ = false;

    // Update final runtime stats
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        auto now = std::chrono::steady_clock::now();
        stats_.totalRuntimeMs = std::chrono::duration<double, std::milli>(
            now - stats_.startTime).count();
    }

    // Update health status to STOPPED
    oldStatus = healthStatus_.exchange(HealthStatus::STOPPED);
    if (healthChangeCallback_) {
        std::lock_guard<std::mutex> lock(callbackMutex_);
        if (healthChangeCallback_) {
            healthChangeCallback_(oldStatus, HealthStatus::STOPPED);
        }
    }
}

void AgentLoop::step() {
    std::lock_guard<std::mutex> lock(stepMutex_);
    stepSignaled_ = true;
    stepEvent_.notify_one();
}

void AgentLoop::pause() {
    pauseRequested_ = true;
}

void AgentLoop::unpause() {
    pauseRequested_ = false;
    {
        std::lock_guard<std::mutex> lock(stepMutex_);
        stepSignaled_ = true;
    }
    stepEvent_.notify_all();
}

bool AgentLoop::isRunning() const {
    return running_;
}

bool AgentLoop::isPaused() const {
    return pauseRequested_;
}

void AgentLoop::enableInputHandling(bool enable) {
    if (enable && !inputHandlingEnabled_) {
        inputHandlingEnabled_ = true;
        inputThread_ = std::make_unique<std::thread>(&AgentLoop::inputHandlingLoop, this);
        std::cout << "Input handling enabled. Press SPACE to step when paused, 'q' to quit." << std::endl;
    } else if (!enable && inputHandlingEnabled_) {
        inputHandlingEnabled_ = false;
        if (inputThread_ && inputThread_->joinable()) {
            inputThread_->join();
        }
        inputThread_.reset();
        std::cout << "Input handling disabled." << std::endl;
    }
}

bool AgentLoop::isInputHandlingEnabled() const {
    return inputHandlingEnabled_;
}

// =========================================================================
// Statistics and Monitoring Implementation
// =========================================================================

LoopStats AgentLoop::getStatistics() const {
    std::lock_guard<std::mutex> lock(statsMutex_);

    LoopStats result = stats_;

    // Calculate current runtime if still running
    if (running_) {
        auto now = std::chrono::steady_clock::now();
        result.totalRuntimeMs = std::chrono::duration<double, std::milli>(
            now - stats_.startTime).count();

        // Calculate iterations per second
        if (result.totalRuntimeMs > 0) {
            result.iterationsPerSecond = (result.totalIterations * 1000.0) / result.totalRuntimeMs;
        }
    }

    return result;
}

void AgentLoop::resetStatistics() {
    std::lock_guard<std::mutex> lock(statsMutex_);

    stats_ = LoopStats{};
    stats_.minStepDurationMs = std::numeric_limits<double>::max();
    stats_.startTime = std::chrono::steady_clock::now();
    stats_.lastStepTime = stats_.startTime;
    totalStepDurationMs_ = 0.0;
}

HealthStatus AgentLoop::checkHealth() const {
    if (!running_) {
        return HealthStatus::STOPPED;
    }

    // Check for stall condition
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        auto now = std::chrono::steady_clock::now();
        auto timeSinceLastStep = std::chrono::duration<double, std::milli>(
            now - stats_.lastStepTime).count();

        if (timeSinceLastStep > static_cast<double>(stallTimeoutMs_)) {
            return HealthStatus::UNHEALTHY;
        }

        // Check error rate
        if (stats_.errorCount >= errorThreshold_) {
            double errorRate = static_cast<double>(stats_.errorCount) /
                std::max(static_cast<uint64_t>(1), stats_.totalStepsExecuted);
            if (errorRate > 0.5) {
                return HealthStatus::UNHEALTHY;
            } else if (errorRate > 0.1) {
                return HealthStatus::DEGRADED;
            }
        }
    }

    return healthStatus_.load();
}

std::string AgentLoop::healthStatusToString(HealthStatus status) {
    switch (status) {
        case HealthStatus::HEALTHY:   return "HEALTHY";
        case HealthStatus::DEGRADED:  return "DEGRADED";
        case HealthStatus::UNHEALTHY: return "UNHEALTHY";
        case HealthStatus::STOPPED:   return "STOPPED";
        case HealthStatus::STARTING:  return "STARTING";
        case HealthStatus::STOPPING:  return "STOPPING";
        default:                      return "UNKNOWN";
    }
}

void AgentLoop::updateStatistics(double stepDurationMs, bool success) {
    // First record latency without stats lock
    {
        std::lock_guard<std::mutex> latLock(latencyMutex_);
        latencyHistogram_.push_back(stepDurationMs);
        if (latencyHistogram_.size() > latencyHistogramMaxSize_) {
            latencyHistogram_.erase(latencyHistogram_.begin());
        }
    }

    std::lock_guard<std::mutex> lock(statsMutex_);

    stats_.totalStepsExecuted++;
    stats_.lastStepTime = std::chrono::steady_clock::now();

    if (success) {
        stats_.successCount++;
    } else {
        stats_.errorCount++;
    }

    // Update timing statistics
    totalStepDurationMs_ += stepDurationMs;
    stats_.avgStepDurationMs = totalStepDurationMs_ / stats_.totalStepsExecuted;
    stats_.maxStepDurationMs = std::max(stats_.maxStepDurationMs, stepDurationMs);

    if (stepDurationMs > 0) {
        stats_.minStepDurationMs = std::min(stats_.minStepDurationMs, stepDurationMs);
    }

    // Update runtime
    stats_.totalRuntimeMs = std::chrono::duration<double, std::milli>(
        stats_.lastStepTime - stats_.startTime).count();

    // Update iterations per second
    if (stats_.totalRuntimeMs > 0) {
        stats_.iterationsPerSecond = (stats_.totalIterations * 1000.0) / stats_.totalRuntimeMs;
    }
    
    // Update latency percentiles (need to release stats lock and acquire latency lock)
    {
        // Note: We already have latencyMutex_ data from above
        // Calculate percentiles inline to avoid deadlock
        std::lock_guard<std::mutex> latLock(latencyMutex_);
        if (!latencyHistogram_.empty()) {
            std::vector<double> sorted = latencyHistogram_;
            std::sort(sorted.begin(), sorted.end());
            
            auto getPercentile = [&sorted](double p) -> double {
                size_t index = static_cast<size_t>((p / 100.0) * (sorted.size() - 1));
                return sorted[index];
            };
            
            stats_.p50LatencyMs = getPercentile(50.0);
            stats_.p95LatencyMs = getPercentile(95.0);
            stats_.p99LatencyMs = getPercentile(99.0);
        }
    }
}

void AgentLoop::updateHealthStatus() {
    HealthStatus newStatus = checkHealth();
    HealthStatus oldStatus = healthStatus_.exchange(newStatus);

    if (oldStatus != newStatus && healthChangeCallback_) {
        std::lock_guard<std::mutex> lock(callbackMutex_);
        if (healthChangeCallback_) {
            healthChangeCallback_(oldStatus, newStatus);
        }
    }
    
    // Update cognitive load and degradation status
    updateCognitiveLoad();
    
    auto load = getCognitiveLoad();
    bool wasDegraded = currentlyDegraded_.load();
    bool shouldNowDegrade = load.shouldDegrade();
    
    if (wasDegraded != shouldNowDegrade) {
        currentlyDegraded_.store(shouldNowDegrade);
        if (shouldNowDegrade) {
            std::cerr << "[AgentLoop] Entering degraded mode - composite load: " 
                      << (load.getCompositeLoad() * 100) << "%" << std::endl;
        } else {
            std::cout << "[AgentLoop] Exiting degraded mode - composite load: " 
                      << (load.getCompositeLoad() * 100) << "%" << std::endl;
        }
    }
}

// =========================================================================
// Dynamic Interval Implementation
// =========================================================================

void AgentLoop::setDynamicInterval(bool enable) {
    dynamicIntervalEnabled_ = enable;
}

bool AgentLoop::isDynamicIntervalEnabled() const {
    return dynamicIntervalEnabled_;
}

void AgentLoop::setStepInterval(double interval) {
    stepInterval_ = std::max(0.0, interval);
}

double AgentLoop::getStepInterval() const {
    return stepInterval_;
}

void AgentLoop::setIntervalBounds(double minInterval, double maxInterval) {
    minInterval_ = std::max(0.0, minInterval);
    maxInterval_ = std::max(minInterval_, maxInterval);
}

double AgentLoop::calculateDynamicInterval() const {
    if (!dynamicIntervalEnabled_) {
        return stepInterval_;
    }

    std::lock_guard<std::mutex> lock(statsMutex_);

    // If no steps executed yet, use default interval
    if (stats_.totalStepsExecuted == 0) {
        return stepInterval_;
    }

    // Calculate error rate
    double errorRate = static_cast<double>(stats_.errorCount) /
        std::max(static_cast<uint64_t>(1), stats_.totalStepsExecuted);

    // Calculate load factor based on step duration vs interval
    double loadFactor = 1.0;
    if (stepInterval_ > 0 && stats_.avgStepDurationMs > 0) {
        double intervalMs = stepInterval_ * 1000.0;
        loadFactor = stats_.avgStepDurationMs / intervalMs;
    }

    // Adjust interval based on conditions:
    // - High error rate: increase interval (back off)
    // - High load: increase interval
    // - Low load and low errors: can decrease interval
    double adjustment = 1.0;

    if (errorRate > 0.3) {
        adjustment = 2.0;  // Double the interval on high error rate
    } else if (errorRate > 0.1) {
        adjustment = 1.5;  // Increase by 50% on moderate errors
    } else if (loadFactor > 0.8) {
        adjustment = 1.2;  // Slight increase when heavily loaded
    } else if (loadFactor < 0.3 && errorRate < 0.01) {
        adjustment = 0.8;  // Can speed up if underloaded and healthy
    }

    double newInterval = stepInterval_ * adjustment;
    return std::clamp(newInterval, minInterval_, maxInterval_);
}

// =========================================================================
// Callback Implementation
// =========================================================================

void AgentLoop::setBeforeStepCallback(BeforeStepCallback callback) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    beforeStepCallback_ = std::move(callback);
}

void AgentLoop::setAfterStepCallback(AfterStepCallback callback) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    afterStepCallback_ = std::move(callback);
}

void AgentLoop::setErrorCallback(ErrorCallback callback) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    errorCallback_ = std::move(callback);
}

void AgentLoop::setHealthChangeCallback(HealthChangeCallback callback) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    healthChangeCallback_ = std::move(callback);
}

// =========================================================================
// Configuration Implementation
// =========================================================================

void AgentLoop::setErrorThreshold(uint64_t threshold) {
    errorThreshold_ = threshold;
}

void AgentLoop::setStallTimeout(uint64_t timeoutMs) {
    stallTimeoutMs_ = timeoutMs;
}

// =========================================================================
// Main Loop Implementation
// =========================================================================

void AgentLoop::runLoop() {
    std::shared_ptr<void> nextOutput = nullptr;

    // Signal that loop has started
    {
        std::lock_guard<std::mutex> lock(startedMutex_);
        started_ = true;
    }
    startedEvent_.notify_one();

    while (!stopRequested_) {
        // Handle step interval (potentially dynamic)
        double currentInterval = dynamicIntervalEnabled_ ?
            calculateDynamicInterval() : stepInterval_;

        if (currentInterval > 0.0) {
            auto sleepDuration = std::chrono::duration<double>(currentInterval);
            std::this_thread::sleep_for(sleepDuration);

            if (stopRequested_) break;
        }

        // Execute each step in sequence
        size_t stepIndex = 0;
        for (const auto& loopStep : steps_) {
            // Handle pause state before executing step
            while (pauseRequested_ && !stopRequested_) {
                // When paused, wait for step signal, unpause, or stop
                std::unique_lock<std::mutex> lock(stepMutex_);
                bool signaled = stepEvent_.wait_for(lock, std::chrono::seconds(30), [this] {
                    return stepSignaled_ || stopRequested_ || !pauseRequested_;
                });
                
                // Timeout reached without signal - check if still paused and loop back
                if (!signaled) {
                    continue;
                }

                if (!pauseRequested_) {
                    break; // Unpaused - resume normal execution
                }

                if (stepSignaled_) {
                    stepSignaled_ = false;
                    break; // Exit pause loop and execute one step
                }

                if (stopRequested_) break;
            }

            if (stopRequested_) break;

            // Get step name for callbacks
            std::string stepName = loopStep.name.empty() ?
                ("step_" + std::to_string(stepIndex)) : loopStep.name;

            // Call before-step callback
            {
                std::lock_guard<std::mutex> lock(callbackMutex_);
                if (beforeStepCallback_) {
                    beforeStepCallback_(stepIndex, stepName);
                }
            }

            // Measure step execution time
            auto stepStart = std::chrono::steady_clock::now();
            bool stepSuccess = true;
            std::string errorMsg;

            try {
                if (loopStep.type == LoopStep::SINGLE_ARG) {
                    nextOutput = loopStep.func1(nextOutput);
                } else {
                    nextOutput = loopStep.func2(nextOutput, this);
                }
            } catch (const std::exception& e) {
                stepSuccess = false;
                errorMsg = e.what();
                std::cerr << "Error in step '" << stepName << "': " << e.what() << std::endl;

                // Update last error in stats
                {
                    std::lock_guard<std::mutex> lock(statsMutex_);
                    stats_.lastError = errorMsg;
                }

                // Call error callback
                {
                    std::lock_guard<std::mutex> lock(callbackMutex_);
                    if (errorCallback_) {
                        errorCallback_(errorMsg, stepIndex);
                    }
                }
            }

            auto stepEnd = std::chrono::steady_clock::now();
            double stepDurationMs = std::chrono::duration<double, std::milli>(
                stepEnd - stepStart).count();

            // Update statistics
            updateStatistics(stepDurationMs, stepSuccess);

            // Call after-step callback
            {
                std::lock_guard<std::mutex> lock(callbackMutex_);
                if (afterStepCallback_) {
                    afterStepCallback_(stepIndex, stepName, stepDurationMs);
                }
            }

            // Update health status periodically
            if (stats_.totalStepsExecuted % 10 == 0) {
                updateHealthStatus();
            }

            // If we're paused, stop after executing one step
            if (pauseRequested_) {
                break;
            }

            stepIndex++;
        }

        // Increment iteration count
        {
            std::lock_guard<std::mutex> lock(statsMutex_);
            stats_.totalIterations++;
        }

        if (stopRequested_) break;
    }

    running_ = false;
}

void AgentLoop::inputHandlingLoop() {
    std::cout << "\nKeyboard input handler started:" << std::endl;
    std::cout << "  SPACE = step when paused" << std::endl;
    std::cout << "  q + ENTER = quit loop" << std::endl;
    std::cout << "  s = print statistics" << std::endl;
    std::cout << "  h = print health status" << std::endl;
    std::cout << "  m = export Prometheus metrics" << std::endl;
    std::cout << "  c = show cognitive load" << std::endl;

    // Use int to handle EOF properly
    int input;
    while (inputHandlingEnabled_ && !stopRequested_) {
        input = std::cin.get();

        // Handle EOF (stream error/closure)
        if (input == EOF) {
            break;
        }

        // Handle commands
        if (input == ' ' && isPaused()) {
            std::cout << "Manual step triggered..." << std::endl;
            step();
        } else if (input == 'q') {
            std::cout << "Quit command received, stopping loop..." << std::endl;
            stop();
            break;
        } else if (input == 's') {
            // Print statistics
            auto stats = getStatistics();
            std::cout << "\n=== Loop Statistics ===" << std::endl;
            std::cout << "  Total iterations: " << stats.totalIterations << std::endl;
            std::cout << "  Total steps: " << stats.totalStepsExecuted << std::endl;
            std::cout << "  Success count: " << stats.successCount << std::endl;
            std::cout << "  Error count: " << stats.errorCount << std::endl;
            std::cout << "  Deferred count: " << stats.deferredCount << std::endl;
            std::cout << "  Avg step duration: " << stats.avgStepDurationMs << " ms" << std::endl;
            std::cout << "  Min step duration: " << stats.minStepDurationMs << " ms" << std::endl;
            std::cout << "  Max step duration: " << stats.maxStepDurationMs << " ms" << std::endl;
            std::cout << "  P50 latency: " << stats.p50LatencyMs << " ms" << std::endl;
            std::cout << "  P95 latency: " << stats.p95LatencyMs << " ms" << std::endl;
            std::cout << "  P99 latency: " << stats.p99LatencyMs << " ms" << std::endl;
            std::cout << "  Iterations/sec: " << stats.iterationsPerSecond << std::endl;
            std::cout << "  Total runtime: " << stats.totalRuntimeMs << " ms" << std::endl;
            if (!stats.lastError.empty()) {
                std::cout << "  Last error: " << stats.lastError << std::endl;
            }
            std::cout << "========================" << std::endl;
        } else if (input == 'h') {
            // Print health status
            std::cout << "\nHealth Status: " << healthStatusToString(checkHealth()) << std::endl;
        } else if (input == 'm') {
            // Export Prometheus metrics
            std::cout << "\n=== Prometheus Metrics ===" << std::endl;
            std::cout << exportPrometheusMetrics() << std::endl;
        } else if (input == 'c') {
            // Show cognitive load
            auto load = getCognitiveLoad();
            std::cout << "\n=== Cognitive Load ===" << std::endl;
            std::cout << "  CPU utilization: " << (load.cpuUtilization * 100) << "%" << std::endl;
            std::cout << "  Memory pressure: " << (load.memoryPressure * 100) << "%" << std::endl;
            std::cout << "  Task queue depth: " << (load.taskQueueDepth * 100) << "%" << std::endl;
            std::cout << "  Avg response time: " << load.avgResponseTime << " ms" << std::endl;
            std::cout << "  Error rate: " << (load.errorRate * 100) << "%" << std::endl;
            std::cout << "  Throughput: " << load.throughput << " steps/sec" << std::endl;
            std::cout << "  Composite load: " << (load.getCompositeLoad() * 100) << "%" << std::endl;
            std::cout << "  Under pressure: " << (load.isUnderPressure() ? "YES" : "NO") << std::endl;
            std::cout << "  Should degrade: " << (load.shouldDegrade() ? "YES" : "NO") << std::endl;
            std::cout << "========================" << std::endl;
        }

        // Small delay to prevent excessive CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// =========================================================================
// Phase 1.1: Prometheus Metrics Export Implementation
// =========================================================================

std::string AgentLoop::exportPrometheusMetrics(const std::string& loopName) const {
    auto stats = getStatistics();
    std::ostringstream oss;
    
    // Basic loop metrics
    oss << stats.toPrometheusFormat(loopName);
    
    // Cognitive load metrics
    auto load = getCognitiveLoad();
    oss << "\n# HELP " << loopName << "_cognitive_load Cognitive load metrics\n";
    oss << "# TYPE " << loopName << "_cognitive_load gauge\n";
    oss << loopName << "_cognitive_load{component=\"cpu\"} " << load.cpuUtilization << "\n";
    oss << loopName << "_cognitive_load{component=\"memory\"} " << load.memoryPressure << "\n";
    oss << loopName << "_cognitive_load{component=\"queue\"} " << load.taskQueueDepth << "\n";
    oss << loopName << "_cognitive_load{component=\"error_rate\"} " << load.errorRate << "\n";
    oss << loopName << "_cognitive_load{component=\"composite\"} " << load.getCompositeLoad() << "\n\n";
    
    // Health status
    oss << "# HELP " << loopName << "_health_status Current health status (0-5)\n";
    oss << "# TYPE " << loopName << "_health_status gauge\n";
    oss << loopName << "_health_status " << static_cast<int>(checkHealth()) << "\n\n";
    
    // Degradation status
    oss << "# HELP " << loopName << "_degraded Whether the loop is in degraded mode\n";
    oss << "# TYPE " << loopName << "_degraded gauge\n";
    oss << loopName << "_degraded " << (currentlyDegraded_.load() ? 1 : 0) << "\n";
    
    return oss.str();
}

std::vector<PrometheusMetric> AgentLoop::getMetrics() const {
    std::vector<PrometheusMetric> metrics;
    auto stats = getStatistics();
    auto load = getCognitiveLoad();
    
    metrics.push_back({"agent_loop_iterations_total", "Total loop iterations", "counter", 
                       static_cast<double>(stats.totalIterations), {}});
    metrics.push_back({"agent_loop_steps_success", "Successful steps", "counter", 
                       static_cast<double>(stats.successCount), {}});
    metrics.push_back({"agent_loop_steps_error", "Failed steps", "counter", 
                       static_cast<double>(stats.errorCount), {}});
    metrics.push_back({"agent_loop_steps_deferred", "Deferred steps", "counter", 
                       static_cast<double>(stats.deferredCount), {}});
    metrics.push_back({"agent_loop_avg_duration_ms", "Average step duration", "gauge", 
                       stats.avgStepDurationMs, {}});
    metrics.push_back({"agent_loop_cognitive_load", "Composite cognitive load", "gauge", 
                       load.getCompositeLoad(), {}});
    metrics.push_back({"agent_loop_throughput_hz", "Steps per second", "gauge", 
                       load.throughput, {}});
    
    return metrics;
}

// =========================================================================
// Phase 1.1: Cognitive Load Monitoring Implementation
// =========================================================================

CognitiveLoad AgentLoop::getCognitiveLoad() const {
    std::lock_guard<std::mutex> lock(cognitiveLoadMutex_);
    return cognitiveLoad_;
}

void AgentLoop::enableCognitiveLoadMonitoring(bool enable) {
    cognitiveLoadMonitoringEnabled_ = enable;
}

bool AgentLoop::isCognitiveLoadMonitoringEnabled() const {
    return cognitiveLoadMonitoringEnabled_;
}

void AgentLoop::setCognitiveLoadThresholds(double pressureThreshold, double degradeThreshold) {
    pressureThreshold_ = std::clamp(pressureThreshold, 0.0, 1.0);
    degradeThreshold_ = std::clamp(degradeThreshold, pressureThreshold_, 1.0);
}

void AgentLoop::updateCognitiveLoad() {
    if (!cognitiveLoadMonitoringEnabled_) return;
    
    std::lock_guard<std::mutex> loadLock(cognitiveLoadMutex_);
    std::lock_guard<std::mutex> statsLock(statsMutex_);
    
    // Estimate CPU utilization based on step duration vs interval
    if (stepInterval_ > 0 && stats_.avgStepDurationMs > 0) {
        double intervalMs = stepInterval_ * 1000.0;
        cognitiveLoad_.cpuUtilization = std::min(1.0, stats_.avgStepDurationMs / intervalMs);
    }
    
    // Calculate error rate from recent history
    if (stats_.totalStepsExecuted > 0) {
        cognitiveLoad_.errorRate = static_cast<double>(stats_.errorCount) / stats_.totalStepsExecuted;
    }
    
    // Calculate throughput
    if (stats_.totalRuntimeMs > 0) {
        cognitiveLoad_.throughput = (stats_.totalStepsExecuted * 1000.0) / stats_.totalRuntimeMs;
    }
    
    // Average response time
    cognitiveLoad_.avgResponseTime = stats_.avgStepDurationMs;
    
    // Task queue depth (normalized based on step count)
    cognitiveLoad_.taskQueueDepth = steps_.empty() ? 0.0 : 
        std::min(1.0, static_cast<double>(steps_.size()) / 100.0);
    
    // Memory pressure is harder to measure accurately without system calls
    // Use a simple heuristic based on error rate and step count growth
    cognitiveLoad_.memoryPressure = std::min(1.0, 
        cognitiveLoad_.errorRate * 0.5 + cognitiveLoad_.taskQueueDepth * 0.5);
}

// =========================================================================
// Phase 1.1: Graceful Degradation Implementation
// =========================================================================

void AgentLoop::enableGracefulDegradation(bool enable) {
    gracefulDegradationEnabled_ = enable;
}

bool AgentLoop::isGracefulDegradationEnabled() const {
    return gracefulDegradationEnabled_;
}

bool AgentLoop::isCurrentlyDegraded() const {
    return currentlyDegraded_.load();
}

bool AgentLoop::shouldDeferStep(const LoopStep& step) const {
    if (!gracefulDegradationEnabled_) return false;
    if (!step.canDefer) return false;
    
    auto load = getCognitiveLoad();
    
    // Critical and high priority steps are never deferred
    if (step.priority == StepPriority::CRITICAL || step.priority == StepPriority::HIGH) {
        return false;
    }
    
    // Background steps are deferred if under any pressure
    if (step.priority == StepPriority::BACKGROUND && load.isUnderPressure()) {
        return true;
    }
    
    // Low priority steps are deferred if should degrade
    if (step.priority == StepPriority::LOW && load.shouldDegrade()) {
        return true;
    }
    
    // Normal priority steps are only deferred under severe pressure
    if (step.priority == StepPriority::NORMAL && load.getCompositeLoad() > 0.95) {
        return true;
    }
    
    return false;
}

std::vector<LoopStep> AgentLoop::getExecutableSteps() const {
    std::vector<LoopStep> executableSteps;
    
    for (const auto& step : steps_) {
        if (!shouldDeferStep(step)) {
            executableSteps.push_back(step);
        }
    }
    
    return executableSteps;
}

// =========================================================================
// Phase 1.1: Priority-Based Step Execution Implementation
// =========================================================================

void AgentLoop::addPriorityStep(const LoopStep& step) {
    std::lock_guard<std::mutex> lock(priorityStepsMutex_);
    steps_.push_back(step);
    sortStepsByPriority();
}

void AgentLoop::removePriorityStep(const std::string& stepName) {
    std::lock_guard<std::mutex> lock(priorityStepsMutex_);
    steps_.erase(
        std::remove_if(steps_.begin(), steps_.end(),
            [&stepName](const LoopStep& s) { return s.name == stepName; }),
        steps_.end()
    );
}

std::vector<LoopStep> AgentLoop::getStepsByPriority(StepPriority priority) const {
    std::vector<LoopStep> result;
    for (const auto& step : steps_) {
        if (step.priority == priority) {
            result.push_back(step);
        }
    }
    return result;
}

void AgentLoop::setStepPriority(const std::string& stepName, StepPriority priority) {
    for (auto& step : steps_) {
        if (step.name == stepName) {
            step.priority = priority;
            break;
        }
    }
    sortStepsByPriority();
}

void AgentLoop::sortStepsByPriority() {
    std::sort(steps_.begin(), steps_.end(),
        [](const LoopStep& a, const LoopStep& b) {
            return static_cast<int>(a.priority) < static_cast<int>(b.priority);
        }
    );
}

// =========================================================================
// Phase 1.1: Dynamic Step Management Implementation
// =========================================================================

void AgentLoop::addStep(const LoopStep& step) {
    std::lock_guard<std::mutex> lock(priorityStepsMutex_);
    steps_.push_back(step);
}

void AgentLoop::removeStep(const std::string& stepName) {
    std::lock_guard<std::mutex> lock(priorityStepsMutex_);
    steps_.erase(
        std::remove_if(steps_.begin(), steps_.end(),
            [&stepName](const LoopStep& s) { return s.name == stepName; }),
        steps_.end()
    );
}

size_t AgentLoop::getStepCount() const {
    return steps_.size();
}

// =========================================================================
// Phase 1.1: Latency Percentile Tracking Implementation
// =========================================================================

void AgentLoop::recordLatency(double latencyMs) {
    std::lock_guard<std::mutex> lock(latencyMutex_);
    
    latencyHistogram_.push_back(latencyMs);
    
    // Keep histogram size bounded
    if (latencyHistogram_.size() > latencyHistogramMaxSize_) {
        latencyHistogram_.erase(latencyHistogram_.begin());
    }
    
    updateLatencyPercentiles();
}

double AgentLoop::getPercentileLatency(double percentile) const {
    std::lock_guard<std::mutex> lock(latencyMutex_);
    
    if (latencyHistogram_.empty()) return 0.0;
    
    std::vector<double> sorted = latencyHistogram_;
    std::sort(sorted.begin(), sorted.end());
    
    size_t index = static_cast<size_t>((percentile / 100.0) * (sorted.size() - 1));
    return sorted[index];
}

void AgentLoop::setLatencyHistogramSize(size_t size) {
    std::lock_guard<std::mutex> lock(latencyMutex_);
    latencyHistogramMaxSize_ = size;
    
    // Trim if necessary
    while (latencyHistogram_.size() > latencyHistogramMaxSize_) {
        latencyHistogram_.erase(latencyHistogram_.begin());
    }
}

void AgentLoop::updateLatencyPercentiles() {
    // Note: latencyMutex_ should already be held by caller
    if (latencyHistogram_.empty()) return;
    
    std::vector<double> sorted = latencyHistogram_;
    std::sort(sorted.begin(), sorted.end());
    
    auto getPercentile = [&sorted](double p) -> double {
        size_t index = static_cast<size_t>((p / 100.0) * (sorted.size() - 1));
        return sorted[index];
    };
    
    std::lock_guard<std::mutex> statsLock(statsMutex_);
    stats_.p50LatencyMs = getPercentile(50.0);
    stats_.p95LatencyMs = getPercentile(95.0);
    stats_.p99LatencyMs = getPercentile(99.0);
}

// =========================================================================
// Task 1.1.2: Circuit Breaker Implementation
// =========================================================================

CircuitBreaker::CircuitBreaker(const CircuitBreakerConfig& config)
    : config_(config) {
    lastStateChange_ = std::chrono::steady_clock::now();
}

bool CircuitBreaker::allowRequest() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    switch (state_) {
        case CircuitState::CLOSED:
            return true;
            
        case CircuitState::OPEN:
            if (shouldTransitionToHalfOpen()) {
                state_ = CircuitState::HALF_OPEN;
                halfOpenRequests_ = 0;
                lastStateChange_ = std::chrono::steady_clock::now();
            }
            return state_ == CircuitState::HALF_OPEN;
            
        case CircuitState::HALF_OPEN:
            if (halfOpenRequests_ < config_.halfOpenMaxRequests) {
                halfOpenRequests_++;
                return true;
            }
            return false;
    }
    return false;
}

void CircuitBreaker::recordSuccess() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    totalSuccesses_++;
    consecutiveSuccesses_++;
    consecutiveFailures_ = 0;
    
    // Update rolling window
    recentResults_.push_back(true);
    if (recentResults_.size() > config_.evaluationWindow) {
        recentResults_.pop_front();
    }
    
    evaluateState();
}

void CircuitBreaker::recordFailure() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    totalFailures_++;
    consecutiveFailures_++;
    consecutiveSuccesses_ = 0;
    lastFailureTime_ = std::chrono::steady_clock::now();
    
    // Update rolling window
    recentResults_.push_back(false);
    if (recentResults_.size() > config_.evaluationWindow) {
        recentResults_.pop_front();
    }
    
    evaluateState();
}

void CircuitBreaker::evaluateState() {
    // Note: mutex_ should already be held
    
    switch (state_) {
        case CircuitState::CLOSED:
            if (shouldTransitionToOpen()) {
                state_ = CircuitState::OPEN;
                lastStateChange_ = std::chrono::steady_clock::now();
            }
            break;
            
        case CircuitState::HALF_OPEN:
            if (shouldTransitionToClosed()) {
                state_ = CircuitState::CLOSED;
                consecutiveFailures_ = 0;
                consecutiveSuccesses_ = 0;
                lastStateChange_ = std::chrono::steady_clock::now();
            } else if (shouldTransitionToOpen()) {
                state_ = CircuitState::OPEN;
                lastStateChange_ = std::chrono::steady_clock::now();
            }
            break;
            
        case CircuitState::OPEN:
            // State transition handled in allowRequest()
            break;
    }
}

bool CircuitBreaker::shouldTransitionToOpen() const {
    // Open if consecutive failures exceed threshold
    if (consecutiveFailures_ >= config_.failureThreshold) {
        return true;
    }
    
    // Or if failure rate exceeds threshold within evaluation window.
    // NOTE: mutex_ is already held by the caller chain
    // (recordSuccess/recordFailure -> evaluateState -> shouldTransitionToOpen).
    // Use the no-lock helper to avoid re-locking the non-recursive mutex_.
    if (recentResults_.size() >= config_.evaluationWindow) {
        double failureRate = computeFailureRateLocked();
        if (failureRate >= config_.failureRateThreshold) {
            return true;
        }
    }
    
    return false;
}

bool CircuitBreaker::shouldTransitionToHalfOpen() const {
    auto elapsed = std::chrono::steady_clock::now() - lastStateChange_;
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    return static_cast<uint64_t>(elapsedMs) >= config_.timeoutMs;
}

bool CircuitBreaker::shouldTransitionToClosed() const {
    return consecutiveSuccesses_ >= config_.successThreshold;
}

CircuitState CircuitBreaker::getState() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_;
}

std::string CircuitBreaker::getStateString() const {
    switch (getState()) {
        case CircuitState::CLOSED: return "CLOSED";
        case CircuitState::OPEN: return "OPEN";
        case CircuitState::HALF_OPEN: return "HALF_OPEN";
    }
    return "UNKNOWN";
}

void CircuitBreaker::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    state_ = CircuitState::CLOSED;
    consecutiveFailures_ = 0;
    consecutiveSuccesses_ = 0;
    totalFailures_ = 0;
    totalSuccesses_ = 0;
    halfOpenRequests_ = 0;
    recentResults_.clear();
    lastStateChange_ = std::chrono::steady_clock::now();
}

void CircuitBreaker::forceOpen() {
    std::lock_guard<std::mutex> lock(mutex_);
    state_ = CircuitState::OPEN;
    lastStateChange_ = std::chrono::steady_clock::now();
}

void CircuitBreaker::forceClosed() {
    std::lock_guard<std::mutex> lock(mutex_);
    state_ = CircuitState::CLOSED;
    consecutiveFailures_ = 0;
    lastStateChange_ = std::chrono::steady_clock::now();
}

uint64_t CircuitBreaker::getFailureCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return totalFailures_;
}

uint64_t CircuitBreaker::getSuccessCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return totalSuccesses_;
}

double CircuitBreaker::computeFailureRateLocked() const {
    // Precondition: mutex_ is already held by the caller.
    if (recentResults_.empty()) return 0.0;

    uint64_t failures = 0;
    for (bool success : recentResults_) {
        if (!success) failures++;
    }
    return static_cast<double>(failures) / static_cast<double>(recentResults_.size());
}

double CircuitBreaker::getFailureRate() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return computeFailureRateLocked();
}

uint64_t CircuitBreaker::getTotalCalls() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return totalFailures_ + totalSuccesses_;
}

std::string CircuitBreaker::toPrometheusFormat(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream oss;
    oss << "# HELP " << name << "_state Circuit breaker state (0=closed, 1=open, 2=half-open)\n";
    oss << "# TYPE " << name << "_state gauge\n";
    oss << name << "_state " << static_cast<int>(state_) << "\n\n";
    
    oss << "# HELP " << name << "_requests_total Total requests by outcome\n";
    oss << "# TYPE " << name << "_requests_total counter\n";
    oss << name << "_requests_total{outcome=\"success\"} " << totalSuccesses_ << "\n";
    oss << name << "_requests_total{outcome=\"failure\"} " << totalFailures_ << "\n\n";
    
    oss << "# HELP " << name << "_failure_rate Current failure rate\n";
    oss << "# TYPE " << name << "_failure_rate gauge\n";
    
    double failureRate = 0.0;
    if (!recentResults_.empty()) {
        uint64_t failures = 0;
        for (bool success : recentResults_) {
            if (!success) failures++;
        }
        failureRate = static_cast<double>(failures) / static_cast<double>(recentResults_.size());
    }
    oss << name << "_failure_rate " << failureRate << "\n";
    
    return oss.str();
}

// =========================================================================
// AgentLoop Circuit Breaker Integration
// =========================================================================

void AgentLoop::enableCircuitBreaker(bool enable) {
    if (enable && !circuitBreaker_) {
        circuitBreaker_ = std::make_unique<CircuitBreaker>();
    }
    circuitBreakerEnabled_ = enable;
}

bool AgentLoop::isCircuitBreakerEnabled() const {
    return circuitBreakerEnabled_;
}

void AgentLoop::setCircuitBreakerConfig(const CircuitBreakerConfig& config) {
    if (!circuitBreaker_) {
        circuitBreaker_ = std::make_unique<CircuitBreaker>(config);
    } else {
        // Reset with new config
        circuitBreaker_ = std::make_unique<CircuitBreaker>(config);
    }
}

CircuitBreaker& AgentLoop::getCircuitBreaker() {
    if (!circuitBreaker_) {
        circuitBreaker_ = std::make_unique<CircuitBreaker>();
    }
    return *circuitBreaker_;
}

const CircuitBreaker& AgentLoop::getCircuitBreaker() const {
    if (!circuitBreaker_) {
        // Create default circuit breaker (const_cast is safe here as we're initializing)
        const_cast<AgentLoop*>(this)->circuitBreaker_ = std::make_unique<CircuitBreaker>();
    }
    return *circuitBreaker_;
}

} // namespace elizaos
