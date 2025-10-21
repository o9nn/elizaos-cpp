#include "elizaos/agentloop.hpp"
#include <chrono>
#include <iostream>

namespace elizaos {

AgentLoop::AgentLoop(const std::vector<LoopStep>& steps, bool paused, double stepInterval)
    : steps_(steps), stepInterval_(stepInterval), stopRequested_(false), 
      pauseRequested_(paused), running_(false), stepSignaled_(false), started_(false),
      inputHandlingEnabled_(false) {
}

AgentLoop::~AgentLoop() {
    enableInputHandling(false); // Stop input handling first
    stop();
}

void AgentLoop::start() {
    if (running_) {
        return; // Already running
    }
    
    stopRequested_ = false;
    running_ = true;
    started_ = false;
    
    loopThread_ = std::make_unique<std::thread>(&AgentLoop::runLoop, this);
    
    // Wait for loop to start (equivalent to Python's started_event.wait())
    std::unique_lock<std::mutex> lock(startedMutex_);
    startedEvent_.wait(lock, [this] { return started_; });
}

void AgentLoop::stop() {
    if (!running_) {
        return;
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

void AgentLoop::runLoop() {
    std::shared_ptr<void> nextOutput = nullptr;
    
    // Signal that loop has started
    {
        std::lock_guard<std::mutex> lock(startedMutex_);
        started_ = true;
    }
    startedEvent_.notify_one();
    
    while (!stopRequested_) {
        // Handle step interval
        if (stepInterval_ > 0.0) {
            auto sleepDuration = std::chrono::duration<double>(stepInterval_);
            std::this_thread::sleep_for(sleepDuration);
            
            if (stopRequested_) break;
        }
        
        // Execute each step in sequence
        for (const auto& step : steps_) {
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
            
            try {
                if (step.type == LoopStep::SINGLE_ARG) {
                    nextOutput = step.func1(nextOutput);
                } else {
                    nextOutput = step.func2(nextOutput, this);
                }
            } catch (const std::exception& e) {
                std::cerr << "Error in step execution: " << e.what() << std::endl;
                // Continue with next step rather than crashing
            }
            
            // If we're paused, stop after executing one step
            if (pauseRequested_) {
                break;
            }
        }
        
        if (stopRequested_) break;
    }
    
    running_ = false;
}

void AgentLoop::inputHandlingLoop() {
    std::cout << "\nKeyboard input handler started:" << std::endl;
    std::cout << "  SPACE = step when paused" << std::endl;
    std::cout << "  q + ENTER = quit loop" << std::endl;
    
    char input;
    while (inputHandlingEnabled_ && !stopRequested_) {
        input = std::cin.get();
        
        if (input == ' ' && isPaused()) {
            std::cout << "Manual step triggered..." << std::endl;
            step();
        } else if (input == 'q') {
            std::cout << "Quit command received, stopping loop..." << std::endl;
            stop();
            break;
        }
        
        // Small delay to prevent excessive CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

} // namespace elizaos