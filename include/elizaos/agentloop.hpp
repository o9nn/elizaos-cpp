#pragma once

#include "elizaos/core.hpp"
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>

namespace elizaos {

/**
 * AgentLoop - Core event loop system for agent execution
 * C++ implementation of the Python agentloop module functionality
 * 
 * Provides threading-based event loop with pause/resume/step capabilities
 * similar to the Python implementation in agentloop/agentloop/loop.py
 */

// Step function type - can take 1 or 2 arguments like Python version
using StepFunction1 = std::function<std::shared_ptr<void>(std::shared_ptr<void>)>;
using StepFunction2 = std::function<std::shared_ptr<void>(std::shared_ptr<void>, class AgentLoop*)>;

struct LoopStep {
    enum Type { SINGLE_ARG, DOUBLE_ARG };
    
    Type type;
    StepFunction1 func1;
    StepFunction2 func2;
    
    LoopStep(StepFunction1 f) : type(SINGLE_ARG), func1(f) {}
    LoopStep(StepFunction2 f) : type(DOUBLE_ARG), func2(f) {}
};

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
};

} // namespace elizaos