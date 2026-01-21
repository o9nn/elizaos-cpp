#include "elizaos/agentloop.hpp"
#include <chrono>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>

namespace elizaos {

AgentLoop::AgentLoop(const std::vector<LoopStep>& steps, bool paused, double stepInterval)
    : steps_(steps), stepInterval_(stepInterval), stopRequested_(false),
      pauseRequested_(paused), running_(false), stepSignaled_(false), started_(false),
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

    // Wait for loop to start (equivalent to Python's started_event.wait())
    std::unique_lock<std::mutex> lock(startedMutex_);
    startedEvent_.wait(lock, [this] { return started_; });

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

    // Signal any waiting conditions
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
                // When paused, wait for step signal
                std::unique_lock<std::mutex> lock(stepMutex_);
                stepEvent_.wait(lock, [this] {
                    return stepSignaled_ || stopRequested_;
                });

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
            std::cout << "  Avg step duration: " << stats.avgStepDurationMs << " ms" << std::endl;
            std::cout << "  Min step duration: " << stats.minStepDurationMs << " ms" << std::endl;
            std::cout << "  Max step duration: " << stats.maxStepDurationMs << " ms" << std::endl;
            std::cout << "  Iterations/sec: " << stats.iterationsPerSecond << std::endl;
            std::cout << "  Total runtime: " << stats.totalRuntimeMs << " ms" << std::endl;
            if (!stats.lastError.empty()) {
                std::cout << "  Last error: " << stats.lastError << std::endl;
            }
            std::cout << "========================" << std::endl;
        } else if (input == 'h') {
            // Print health status
            std::cout << "\nHealth Status: " << healthStatusToString(checkHealth()) << std::endl;
        }

        // Small delay to prevent excessive CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

} // namespace elizaos
