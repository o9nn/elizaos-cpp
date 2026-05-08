#pragma once

/**
 * ElizaOS C++ - AgentLoop Module
 *
 * Event-driven agent execution loop with pause/resume, step control,
 * health monitoring, dynamic intervals, and statistics.
 */

#include "elizaos.hpp"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace elizaos {

// Forward declaration
class AgentLoop;

// ============================================================================
// LoopStep
// ============================================================================

struct LoopStep {
    using Func1 = std::function<std::shared_ptr<void>(std::shared_ptr<void>)>;
    using Func2 = std::function<std::shared_ptr<void>(std::shared_ptr<void>, AgentLoop*)>;

    enum Type { SINGLE_ARG, DUAL_ARG };

    Func1       func1;
    Func2       func2;
    Type        type = SINGLE_ARG;
    std::string name;

    explicit LoopStep(Func1 fn, const std::string& n = "")
        : func1(std::move(fn)), type(SINGLE_ARG), name(n) {}

    explicit LoopStep(Func2 fn, const std::string& n = "")
        : func2(std::move(fn)), type(DUAL_ARG), name(n) {}
};

// ============================================================================
// LoopStats
// ============================================================================

struct LoopStats {
    uint64_t totalIterations     = 0;
    uint64_t totalStepsExecuted  = 0;
    uint64_t successCount        = 0;
    uint64_t errorCount          = 0;
    double   avgStepDurationMs   = 0.0;
    double   minStepDurationMs   = std::numeric_limits<double>::max();
    double   maxStepDurationMs   = 0.0;
    double   iterationsPerSecond = 0.0;
    double   totalRuntimeMs      = 0.0;
    std::string lastError;

    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastStepTime;
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

private:
    void runLoop();
    void inputHandlingLoop();
    void updateStatistics(double stepDurationMs, bool success);
    void updateHealthStatus();
    double calculateDynamicInterval() const;

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
};

} // namespace elizaos
