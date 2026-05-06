#pragma once

#include "elizaos/core.hpp"
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <string>

namespace elizaos {

/**
 * AgentLoop - Core event loop system for agent execution
 * C++ implementation of the Python agentloop module functionality
 *
 * Provides threading-based event loop with pause/resume/step capabilities
 * similar to the Python implementation in agentloop/agentloop/loop.py
 *
 * Enhanced with:
 * - Loop statistics tracking (iterations, timing, errors)
 * - Lifecycle callbacks (onBeforeStep, onAfterStep, onError)
 * - Health monitoring and status reporting
 * - Dynamic interval adjustment
 */

// Step function type - can take 1 or 2 arguments like Python version
using StepFunction1 = std::function<std::shared_ptr<void>(std::shared_ptr<void>)>;
using StepFunction2 = std::function<std::shared_ptr<void>(std::shared_ptr<void>, class AgentLoop*)>;

struct LoopStep {
    enum Type { SINGLE_ARG, DOUBLE_ARG };

    Type type;
    StepFunction1 func1;
    StepFunction2 func2;
    std::string name;  // Optional name for debugging

    LoopStep(StepFunction1 f, const std::string& stepName = "")
        : type(SINGLE_ARG), func1(f), name(stepName) {}
    LoopStep(StepFunction2 f, const std::string& stepName = "")
        : type(DOUBLE_ARG), func2(f), name(stepName) {}
};

/**
 * Loop statistics for monitoring performance and health
 */
struct LoopStats {
    uint64_t totalIterations = 0;      // Total loop iterations completed
    uint64_t totalStepsExecuted = 0;   // Total individual steps executed
    uint64_t errorCount = 0;           // Number of errors encountered
    uint64_t successCount = 0;         // Number of successful step executions
    double avgStepDurationMs = 0.0;    // Average step duration in milliseconds
    double maxStepDurationMs = 0.0;    // Maximum step duration observed
    double minStepDurationMs = 0.0;    // Minimum step duration observed
    double iterationsPerSecond = 0.0;  // Current rate of iterations
    double totalRuntimeMs = 0.0;       // Total runtime in milliseconds
    std::string lastError;             // Last error message
    std::chrono::steady_clock::time_point startTime;  // Loop start time
    std::chrono::steady_clock::time_point lastStepTime;  // Time of last step
};

/**
 * Health status of the agent loop
 */
enum class HealthStatus {
    HEALTHY,       // Loop running normally
    DEGRADED,      // Loop running but with errors
    UNHEALTHY,     // High error rate or stalled
    STOPPED,       // Loop not running
    STARTING,      // Loop starting up
    STOPPING       // Loop shutting down
};

/**
 * Callback types for loop lifecycle events
 */
using BeforeStepCallback = std::function<void(size_t stepIndex, const std::string& stepName)>;
using AfterStepCallback = std::function<void(size_t stepIndex, const std::string& stepName, double durationMs)>;
using ErrorCallback = std::function<void(const std::string& errorMessage, size_t stepIndex)>;
using HealthChangeCallback = std::function<void(HealthStatus oldStatus, HealthStatus newStatus)>;

class AgentLoop {
public:
    /**
     * Constructor for AgentLoop
     * @param steps Vector of step functions to execute in the loop
     * @param paused Whether to start the loop in paused state
     * @param stepInterval Time in seconds to wait between steps
     */
    AgentLoop(const std::vector<LoopStep>& steps, bool paused = false, double stepInterval = 0.0);

    /**
     * Destructor - ensures clean shutdown
     */
    ~AgentLoop();

    /**
     * Start the agent loop in a separate thread
     * Equivalent to start() function in Python implementation
     */
    void start();

    /**
     * Stop the agent loop
     * Equivalent to stop() function in Python implementation
     */
    void stop();

    /**
     * Perform a single step in the loop
     * Equivalent to step() function in Python implementation
     */
    void step();

    /**
     * Pause the loop execution
     * Equivalent to pause() function in Python implementation
     */
    void pause();

    /**
     * Resume the loop execution
     * Equivalent to unpause() function in Python implementation
     */
    void unpause();

    /**
     * Check if the loop is currently running
     */
    bool isRunning() const;

    /**
     * Check if the loop is currently paused
     */
    bool isPaused() const;

    /**
     * Enable keyboard input handling for stepping the loop
     * Starts a background thread that listens for:
     * - Space key: execute single step when paused
     * - 'q' key: stop the loop
     * @param enable Whether to enable or disable input handling
     */
    void enableInputHandling(bool enable = true);

    /**
     * Check if input handling is enabled
     */
    bool isInputHandlingEnabled() const;

    // =========================================================================
    // Statistics and Monitoring (NEW)
    // =========================================================================

    /**
     * Get current loop statistics
     * @return LoopStats structure with performance metrics
     */
    LoopStats getStatistics() const;

    /**
     * Reset all statistics counters
     */
    void resetStatistics();

    /**
     * Get current health status of the loop
     * @return HealthStatus enum indicating loop health
     */
    HealthStatus checkHealth() const;

    /**
     * Get health status as human-readable string
     */
    static std::string healthStatusToString(HealthStatus status);

    // =========================================================================
    // Dynamic Interval Adjustment (NEW)
    // =========================================================================

    /**
     * Enable or disable dynamic interval adjustment
     * When enabled, the loop automatically adjusts step intervals based on load
     * @param enable Whether to enable dynamic adjustment
     */
    void setDynamicInterval(bool enable);

    /**
     * Check if dynamic interval adjustment is enabled
     */
    bool isDynamicIntervalEnabled() const;

    /**
     * Set the step interval
     * @param interval Time in seconds between steps
     */
    void setStepInterval(double interval);

    /**
     * Get the current step interval
     */
    double getStepInterval() const;

    /**
     * Set minimum and maximum bounds for dynamic interval
     * @param minInterval Minimum interval in seconds
     * @param maxInterval Maximum interval in seconds
     */
    void setIntervalBounds(double minInterval, double maxInterval);

    // =========================================================================
    // Lifecycle Callbacks (NEW)
    // =========================================================================

    /**
     * Set callback to be called before each step execution
     * @param callback Function to call with step index and name
     */
    void setBeforeStepCallback(BeforeStepCallback callback);

    /**
     * Set callback to be called after each step execution
     * @param callback Function to call with step index, name, and duration
     */
    void setAfterStepCallback(AfterStepCallback callback);

    /**
     * Set callback to be called when an error occurs
     * @param callback Function to call with error message and step index
     */
    void setErrorCallback(ErrorCallback callback);

    /**
     * Set callback to be called when health status changes
     * @param callback Function to call with old and new health status
     */
    void setHealthChangeCallback(HealthChangeCallback callback);

    // =========================================================================
    // Configuration (NEW)
    // =========================================================================

    /**
     * Set error threshold for health degradation
     * @param threshold Number of errors before status becomes DEGRADED
     */
    void setErrorThreshold(uint64_t threshold);

    /**
     * Set stall timeout for health monitoring
     * @param timeoutMs Time in milliseconds without steps before UNHEALTHY
     */
    void setStallTimeout(uint64_t timeoutMs);

private:
    /**
     * Main loop execution function - runs in separate thread
     * Equivalent to loop() function in Python implementation
     */
    void runLoop();

    /**
     * Input handling thread function
     */
    void inputHandlingLoop();

    /**
     * Update statistics after a step execution
     */
    void updateStatistics(double stepDurationMs, bool success);

    /**
     * Update health status based on current statistics
     */
    void updateHealthStatus();

    /**
     * Calculate dynamic interval based on load
     */
    double calculateDynamicInterval() const;

    std::vector<LoopStep> steps_;
    double stepInterval_;

    // Threading and synchronization
    std::unique_ptr<std::thread> loopThread_;
    std::atomic<bool> stopRequested_;
    std::atomic<bool> pauseRequested_;
    std::atomic<bool> running_;

    // Events for synchronization (similar to Python threading.Event)
    std::condition_variable stepEvent_;
    std::condition_variable startedEvent_;
    std::mutex stepMutex_;
    std::mutex startedMutex_;

    bool stepSignaled_;
    bool started_;

    // Input handling
    std::atomic<bool> inputHandlingEnabled_;
    std::unique_ptr<std::thread> inputThread_;

    // Statistics (NEW)
    mutable std::mutex statsMutex_;
    LoopStats stats_;
    double totalStepDurationMs_ = 0.0;  // For calculating average

    // Health monitoring (NEW)
    std::atomic<HealthStatus> healthStatus_{HealthStatus::STOPPED};
    uint64_t errorThreshold_ = 10;      // Errors before DEGRADED
    uint64_t stallTimeoutMs_ = 30000;   // 30 seconds without steps = UNHEALTHY

    // Dynamic interval (NEW)
    std::atomic<bool> dynamicIntervalEnabled_{false};
    double minInterval_ = 0.001;   // 1ms minimum
    double maxInterval_ = 10.0;    // 10s maximum

    // Callbacks (NEW)
    BeforeStepCallback beforeStepCallback_;
    AfterStepCallback afterStepCallback_;
    ErrorCallback errorCallback_;
    HealthChangeCallback healthChangeCallback_;
    std::mutex callbackMutex_;
};

} // namespace elizaos