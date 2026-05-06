#pragma once

/**
 * ElizaOS C++ - AgentLoop Module
 * 
 * Provides the main agent execution loop with step-based processing,
 * pause/resume capabilities, and health monitoring.
 */

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

namespace elizaos {

/**
 * Health status of the agent loop
 */
enum class HealthStatus {
    STOPPED,
    STARTING,
    HEALTHY,
    PAUSED,
    STOPPING,
    ERROR
};

/**
 * Statistics for the agent loop
 */
struct LoopStats {
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastStepTime;
    size_t totalSteps = 0;
    size_t successfulSteps = 0;
    size_t failedSteps = 0;
    double minStepDurationMs = 0.0;
    double maxStepDurationMs = 0.0;
    double avgStepDurationMs = 0.0;
};

/**
 * A single step in the agent loop
 */
struct LoopStep {
    std::string name;
    std::function<bool()> execute;
    std::optional<double> timeout;
};

/**
 * Callback type for health status changes
 */
using HealthChangeCallback = std::function<void(HealthStatus oldStatus, HealthStatus newStatus)>;

/**
 * Callback type for input handling
 */
using InputHandler = std::function<void(const std::string& input)>;

/**
 * Main agent execution loop
 */
class AgentLoop {
public:
    /**
     * Constructor
     * @param steps - The steps to execute in the loop
     * @param paused - Whether to start in paused state
     * @param stepInterval - Interval between steps in seconds
     */
    AgentLoop(const std::vector<LoopStep>& steps, bool paused = false, double stepInterval = 1.0);
    
    /**
     * Destructor - stops the loop if running
     */
    ~AgentLoop();
    
    // Non-copyable
    AgentLoop(const AgentLoop&) = delete;
    AgentLoop& operator=(const AgentLoop&) = delete;
    
    /**
     * Start the agent loop
     */
    void start();
    
    /**
     * Stop the agent loop
     */
    void stop();
    
    /**
     * Pause the agent loop
     */
    void pause();
    
    /**
     * Resume the agent loop
     */
    void resume();
    
    /**
     * Signal to execute the next step immediately
     */
    void signalStep();
    
    /**
     * Check if the loop is running
     */
    bool isRunning() const { return running_; }
    
    /**
     * Check if the loop is paused
     */
    bool isPaused() const { return pauseRequested_; }
    
    /**
     * Get the current health status
     */
    HealthStatus getHealthStatus() const { return healthStatus_.load(); }
    
    /**
     * Get loop statistics
     */
    LoopStats getStats() const;
    
    /**
     * Set the health change callback
     */
    void setHealthChangeCallback(HealthChangeCallback callback);
    
    /**
     * Enable or disable input handling
     */
    void enableInputHandling(bool enable);
    
    /**
     * Set the input handler
     */
    void setInputHandler(InputHandler handler);

private:
    void runLoop();
    void executeStep(const LoopStep& step);
    void updateStats(double durationMs, bool success);
    
    std::vector<LoopStep> steps_;
    double stepInterval_;
    
    std::atomic<bool> stopRequested_;
    std::atomic<bool> pauseRequested_;
    std::atomic<bool> running_;
    std::atomic<bool> stepSignaled_;
    std::atomic<bool> started_;
    std::atomic<bool> inputHandlingEnabled_;
    std::atomic<HealthStatus> healthStatus_{HealthStatus::STOPPED};
    
    std::unique_ptr<std::thread> loopThread_;
    std::unique_ptr<std::thread> inputThread_;
    
    mutable std::mutex statsMutex_;
    LoopStats stats_;
    
    std::mutex pauseMutex_;
    std::condition_variable pauseCondition_;
    
    std::mutex startedMutex_;
    std::condition_variable startedEvent_;
    
    std::mutex callbackMutex_;
    HealthChangeCallback healthChangeCallback_;
    InputHandler inputHandler_;
};

} // namespace elizaos
