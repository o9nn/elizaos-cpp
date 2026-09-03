#pragma once

/**
 * ElizaOS C++ - AgentLoop Module
 *
 * Event-driven agent execution loop with pause/resume, step control,
 * health monitoring, dynamic intervals, statistics, and production features.
 * 
 * Phase 1.1 Enhancements:
 * - Prometheus metrics export format
 * - Priority-based step execution
 * - Cognitive load monitoring
 * - Graceful degradation under resource pressure
 * - Step execution priority queues
 */

#include "elizaos/elizaos.hpp"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <queue>
#include <sstream>
#include <map>

namespace elizaos {

// Forward declaration
class AgentLoop;

// ============================================================================
// StepPriority - Priority levels for step execution
// ============================================================================

enum class StepPriority {
    CRITICAL = 0,   // Must execute immediately, cannot be deferred
    HIGH = 1,       // High priority, execute before normal steps
    NORMAL = 2,     // Default priority level
    LOW = 3,        // Can be deferred under resource pressure
    BACKGROUND = 4  // Execute only when system is idle
};

// ============================================================================
// CognitiveLoad - Resource monitoring for graceful degradation
// ============================================================================

struct CognitiveLoad {
    double cpuUtilization = 0.0;        // [0.0, 1.0] estimated CPU usage
    double memoryPressure = 0.0;        // [0.0, 1.0] memory pressure level
    double taskQueueDepth = 0.0;        // [0.0, 1.0] normalized queue depth
    double avgResponseTime = 0.0;       // Average step response time (ms)
    double errorRate = 0.0;             // [0.0, 1.0] recent error rate
    double throughput = 0.0;            // Steps per second
    
    // Configurable weights for composite load calculation
    static constexpr double CPU_WEIGHT = 0.25;
    static constexpr double MEMORY_WEIGHT = 0.25;
    static constexpr double QUEUE_WEIGHT = 0.2;
    static constexpr double ERROR_WEIGHT = 0.2;
    static constexpr double RESPONSE_TIME_WEIGHT = 0.1;
    static constexpr double PRESSURE_THRESHOLD = 0.75;
    static constexpr double DEGRADE_THRESHOLD = 0.85;
    
    // Composite load score [0.0, 1.0]
    double getCompositeLoad() const {
        return (cpuUtilization * CPU_WEIGHT + 
                memoryPressure * MEMORY_WEIGHT + 
                taskQueueDepth * QUEUE_WEIGHT + 
                errorRate * ERROR_WEIGHT +
                std::min(1.0, avgResponseTime / 1000.0) * RESPONSE_TIME_WEIGHT);
    }
    
    // Check if system is under pressure
    bool isUnderPressure() const {
        return getCompositeLoad() > PRESSURE_THRESHOLD;
    }
    
    // Check if system should degrade gracefully
    bool shouldDegrade() const {
        return getCompositeLoad() > DEGRADE_THRESHOLD;
    }
};

// ============================================================================
// PrometheusMetric - Metric types for Prometheus export
// ============================================================================

struct PrometheusMetric {
    std::string name;
    std::string help;
    std::string type;  // "counter", "gauge", "histogram", "summary"
    double value;
    std::map<std::string, std::string> labels;
    
    std::string toPrometheusFormat() const {
        std::ostringstream oss;
        oss << "# HELP " << name << " " << help << "\n";
        oss << "# TYPE " << name << " " << type << "\n";
        oss << name;
        if (!labels.empty()) {
            oss << "{";
            bool first = true;
            for (const auto& [k, v] : labels) {
                if (!first) oss << ",";
                oss << k << "=\"" << v << "\"";
                first = false;
            }
            oss << "}";
        }
        oss << " " << value << "\n";
        return oss.str();
    }
};

// ============================================================================
// LoopStep - Enhanced with priority support
// ============================================================================

struct LoopStep {
    using Func1 = std::function<std::shared_ptr<void>(std::shared_ptr<void>)>;
    using Func2 = std::function<std::shared_ptr<void>(std::shared_ptr<void>, AgentLoop*)>;

    enum Type { SINGLE_ARG, DUAL_ARG };

    Func1       func1;
    Func2       func2;
    Type        type = SINGLE_ARG;
    std::string name;
    StepPriority priority = StepPriority::NORMAL;
    double maxExecutionTimeMs = 0.0;    // 0 = no limit
    bool canDefer = true;               // Can be skipped under pressure
    int retryCount = 0;                 // Number of retries on failure

    explicit LoopStep(Func1 fn, const std::string& n = "")
        : func1(std::move(fn)), type(SINGLE_ARG), name(n) {}

    explicit LoopStep(Func2 fn, const std::string& n = "")
        : func2(std::move(fn)), type(DUAL_ARG), name(n) {}
    
    // Builder pattern for configuration
    LoopStep& withPriority(StepPriority p) { priority = p; return *this; }
    LoopStep& withTimeout(double ms) { maxExecutionTimeMs = ms; return *this; }
    LoopStep& withRetries(int count) { retryCount = count; return *this; }
    LoopStep& nonDeferrable() { canDefer = false; return *this; }
};

// ============================================================================
// LoopStats - Enhanced with detailed metrics
// ============================================================================

struct LoopStats {
    uint64_t totalIterations     = 0;
    uint64_t totalStepsExecuted  = 0;
    uint64_t successCount        = 0;
    uint64_t errorCount          = 0;
    uint64_t deferredCount       = 0;   // Steps deferred under pressure
    uint64_t retriedCount        = 0;   // Steps that required retries
    double   avgStepDurationMs   = 0.0;
    double   minStepDurationMs   = std::numeric_limits<double>::max();
    double   maxStepDurationMs   = 0.0;
    double   iterationsPerSecond = 0.0;
    double   totalRuntimeMs      = 0.0;
    double   p50LatencyMs        = 0.0;  // Median latency
    double   p95LatencyMs        = 0.0;  // 95th percentile latency
    double   p99LatencyMs        = 0.0;  // 99th percentile latency
    std::string lastError;
    
    // Per-priority statistics
    std::map<StepPriority, uint64_t> stepsByPriority;
    std::map<StepPriority, double> avgDurationByPriority;

    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastStepTime;
    
    // Generate Prometheus metrics
    std::string toPrometheusFormat(const std::string& loopName = "agent_loop") const {
        std::ostringstream oss;
        
        oss << "# HELP " << loopName << "_iterations_total Total loop iterations\n";
        oss << "# TYPE " << loopName << "_iterations_total counter\n";
        oss << loopName << "_iterations_total " << totalIterations << "\n\n";
        
        oss << "# HELP " << loopName << "_steps_total Total steps executed\n";
        oss << "# TYPE " << loopName << "_steps_total counter\n";
        oss << loopName << "_steps_total{status=\"success\"} " << successCount << "\n";
        oss << loopName << "_steps_total{status=\"error\"} " << errorCount << "\n";
        oss << loopName << "_steps_total{status=\"deferred\"} " << deferredCount << "\n";
        oss << loopName << "_steps_total{status=\"retried\"} " << retriedCount << "\n\n";
        
        oss << "# HELP " << loopName << "_step_duration_ms Step duration statistics\n";
        oss << "# TYPE " << loopName << "_step_duration_ms summary\n";
        oss << loopName << "_step_duration_ms{quantile=\"0.5\"} " << p50LatencyMs << "\n";
        oss << loopName << "_step_duration_ms{quantile=\"0.95\"} " << p95LatencyMs << "\n";
        oss << loopName << "_step_duration_ms{quantile=\"0.99\"} " << p99LatencyMs << "\n";
        oss << loopName << "_step_duration_ms_avg " << avgStepDurationMs << "\n";
        oss << loopName << "_step_duration_ms_min " << minStepDurationMs << "\n";
        oss << loopName << "_step_duration_ms_max " << maxStepDurationMs << "\n\n";
        
        oss << "# HELP " << loopName << "_throughput_hz Steps per second\n";
        oss << "# TYPE " << loopName << "_throughput_hz gauge\n";
        oss << loopName << "_throughput_hz " << iterationsPerSecond << "\n\n";
        
        oss << "# HELP " << loopName << "_runtime_ms Total runtime in milliseconds\n";
        oss << "# TYPE " << loopName << "_runtime_ms counter\n";
        oss << loopName << "_runtime_ms " << totalRuntimeMs << "\n";
        
        return oss.str();
    }
};

// ============================================================================
// CircuitBreaker - Task 1.1.2: Fault tolerance pattern
// ============================================================================

enum class CircuitState {
    CLOSED,      // Normal operation - requests go through
    OPEN,        // Circuit is open - requests fail fast
    HALF_OPEN    // Testing if service has recovered
};

struct CircuitBreakerConfig {
    uint64_t failureThreshold = 5;        // Number of failures before opening
    uint64_t successThreshold = 3;        // Number of successes to close from half-open
    uint64_t timeoutMs = 30000;           // Time to wait before moving to half-open
    uint64_t halfOpenMaxRequests = 3;     // Max requests allowed in half-open state
    double failureRateThreshold = 0.5;    // Failure rate that triggers opening
    uint64_t evaluationWindow = 10;       // Number of calls to evaluate failure rate
};

class CircuitBreaker {
public:
    explicit CircuitBreaker(const CircuitBreakerConfig& config = {});
    
    // Check if request should be allowed
    bool allowRequest();
    
    // Record success/failure
    void recordSuccess();
    void recordFailure();
    
    // State management
    CircuitState getState() const;
    std::string getStateString() const;
    void reset();
    void forceOpen();
    void forceClosed();
    
    // Statistics
    uint64_t getFailureCount() const;
    uint64_t getSuccessCount() const;
    double getFailureRate() const;
    uint64_t getTotalCalls() const;
    
    // Metrics
    std::string toPrometheusFormat(const std::string& name = "circuit_breaker") const;

private:
    void evaluateState();
    bool shouldTransitionToHalfOpen() const;
    bool shouldTransitionToClosed() const;
    bool shouldTransitionToOpen() const;
    // Computes the rolling-window failure rate WITHOUT acquiring mutex_.
    // Callers (evaluateState/shouldTransitionToOpen) already hold mutex_;
    // the public getFailureRate() wraps this with a lock. This avoids
    // non-recursive std::mutex self-deadlock.
    double computeFailureRateLocked() const;
    
    mutable std::mutex mutex_;
    CircuitBreakerConfig config_;
    CircuitState state_ = CircuitState::CLOSED;
    
    uint64_t consecutiveFailures_ = 0;
    uint64_t consecutiveSuccesses_ = 0;
    uint64_t totalFailures_ = 0;
    uint64_t totalSuccesses_ = 0;
    uint64_t halfOpenRequests_ = 0;
    
    std::chrono::steady_clock::time_point lastFailureTime_;
    std::chrono::steady_clock::time_point lastStateChange_;
    
    // Rolling window for failure rate calculation
    std::deque<bool> recentResults_;  // true = success, false = failure
};

// ============================================================================
// HealthStatus
// ============================================================================

enum class HealthStatus {
    HEALTHY,
    DEGRADED,
    UNHEALTHY,
    STOPPED,
    STARTING,
    STOPPING
};

// ============================================================================
// AgentLoop
// ============================================================================

class AgentLoop {
public:
    using BeforeStepCallback   = std::function<void(size_t stepIndex, const std::string& stepName)>;
    using AfterStepCallback    = std::function<void(size_t stepIndex, const std::string& stepName, double durationMs)>;
    using ErrorCallback        = std::function<void(const std::string& error, size_t stepIndex)>;
    using HealthChangeCallback = std::function<void(HealthStatus oldStatus, HealthStatus newStatus)>;

    AgentLoop(const std::vector<LoopStep>& steps, bool paused = false, double stepInterval = 0.0);
    ~AgentLoop();

    // Non-copyable
    AgentLoop(const AgentLoop&)            = delete;
    AgentLoop& operator=(const AgentLoop&) = delete;

    // Lifecycle
    void start();
    void stop();
    void pause();
    void unpause();
    void step();

    // State queries
    bool isRunning() const;
    bool isPaused()  const;

    // Input handling
    void enableInputHandling(bool enable);
    bool isInputHandlingEnabled() const;

    // Statistics
    LoopStats    getStatistics()   const;
    void         resetStatistics();
    HealthStatus checkHealth()     const;

    static std::string healthStatusToString(HealthStatus status);

    // Dynamic interval
    void   setDynamicInterval(bool enable);
    bool   isDynamicIntervalEnabled() const;
    void   setStepInterval(double interval);
    double getStepInterval() const;
    void   setIntervalBounds(double minInterval, double maxInterval);

    // Callbacks
    void setBeforeStepCallback(BeforeStepCallback  callback);
    void setAfterStepCallback(AfterStepCallback    callback);
    void setErrorCallback(ErrorCallback            callback);
    void setHealthChangeCallback(HealthChangeCallback callback);

    // Thresholds
    void setErrorThreshold(uint64_t threshold);
    void setStallTimeout(uint64_t timeoutMs);
    
    // =========================================================================
    // Phase 1.1 Enhancements: Advanced Features
    // =========================================================================
    
    // Prometheus metrics export
    std::string exportPrometheusMetrics(const std::string& loopName = "agent_loop") const;
    std::vector<PrometheusMetric> getMetrics() const;
    
    // Cognitive load monitoring
    CognitiveLoad getCognitiveLoad() const;
    void enableCognitiveLoadMonitoring(bool enable = true);
    bool isCognitiveLoadMonitoringEnabled() const;
    void setCognitiveLoadThresholds(double pressureThreshold, double degradeThreshold);
    
    // Graceful degradation
    void enableGracefulDegradation(bool enable = true);
    bool isGracefulDegradationEnabled() const;
    bool isCurrentlyDegraded() const;
    
    // Priority-based step execution
    void addPriorityStep(const LoopStep& step);
    void removePriorityStep(const std::string& stepName);
    std::vector<LoopStep> getStepsByPriority(StepPriority priority) const;
    void setStepPriority(const std::string& stepName, StepPriority priority);
    void sortStepsByPriority();
    
    // Dynamic step management
    void addStep(const LoopStep& step);
    void removeStep(const std::string& stepName);
    size_t getStepCount() const;
    
    // Latency percentile tracking
    void recordLatency(double latencyMs);
    double getPercentileLatency(double percentile) const;
    void setLatencyHistogramSize(size_t size);
    
    // Task 1.1.2: Circuit breaker for fault tolerance
    void enableCircuitBreaker(bool enable = true);
    bool isCircuitBreakerEnabled() const;
    void setCircuitBreakerConfig(const CircuitBreakerConfig& config);
    CircuitBreaker& getCircuitBreaker();
    const CircuitBreaker& getCircuitBreaker() const;

private:
    void runLoop();
    void inputHandlingLoop();
    void updateStatistics(double stepDurationMs, bool success);
    void updateHealthStatus();
    void notifyHealthChange(HealthStatus oldStatus, HealthStatus newStatus);
    double calculateDynamicInterval() const;
    void updateCognitiveLoad();
    void updateLatencyPercentiles();
    bool shouldDeferStep(const LoopStep& step) const;
    std::vector<LoopStep> getExecutableSteps() const;

    std::vector<LoopStep> steps_;

    double stepInterval_         = 0.0;
    double minInterval_          = 0.0;
    double maxInterval_          = 60.0;
    bool   dynamicIntervalEnabled_ = false;

    std::atomic<bool>   stopRequested_{false};
    std::atomic<bool>   pauseRequested_{false};
    std::atomic<bool>   running_{false};
    std::atomic<bool>   started_{false};
    std::atomic<bool>   stepSignaled_{false};
    std::atomic<bool>   inputHandlingEnabled_{false};

    std::unique_ptr<std::thread> loopThread_;
    std::unique_ptr<std::thread> inputThread_;

    std::mutex              stepMutex_;
    std::condition_variable stepEvent_;

    std::mutex              startedMutex_;
    std::condition_variable startedEvent_;

    mutable std::mutex statsMutex_;
    LoopStats          stats_;
    double             totalStepDurationMs_ = 0.0;

    mutable std::atomic<HealthStatus> healthStatus_{HealthStatus::STOPPED};
    uint64_t stallTimeoutMs_  = 30000;
    uint64_t errorThreshold_  = 10;

    mutable std::mutex      callbackMutex_;
    BeforeStepCallback      beforeStepCallback_;
    AfterStepCallback       afterStepCallback_;
    ErrorCallback           errorCallback_;
    HealthChangeCallback    healthChangeCallback_;
    
    // Phase 1.1 Enhanced features
    mutable std::mutex cognitiveLoadMutex_;
    CognitiveLoad cognitiveLoad_;
    bool cognitiveLoadMonitoringEnabled_ = false;
    bool gracefulDegradationEnabled_ = false;
    std::atomic<bool> currentlyDegraded_{false};
    double pressureThreshold_ = 0.75;
    double degradeThreshold_ = 0.85;
    
    // Latency histogram for percentile calculations
    mutable std::mutex latencyMutex_;
    std::vector<double> latencyHistogram_;
    size_t latencyHistogramMaxSize_ = 1000;
    
    // Priority queue for dynamic step execution
    mutable std::mutex priorityStepsMutex_;
    std::priority_queue<std::pair<int, LoopStep>> priorityStepsQueue_;
    
    // Task 1.1.2: Circuit breaker
    bool circuitBreakerEnabled_ = false;
    std::unique_ptr<CircuitBreaker> circuitBreaker_;
};

} // namespace elizaos
