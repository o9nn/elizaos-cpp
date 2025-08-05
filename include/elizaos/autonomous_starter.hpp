#pragma once

#include "elizaos/core.hpp"
#include "elizaos/agentloop.hpp"
#include "elizaos/agentshell.hpp"
#include <memory>
#include <chrono>
#include <atomic>

namespace elizaos {

/**
 * AutonomousStarter - Core autonomous agent implementation
 * 
 * A C++ implementation inspired by the TypeScript autonomous-starter,
 * featuring the Autoliza character - a self-aware AI with full terminal access
 * and an insatiable curiosity to explore and learn.
 * 
 * Key Features:
 * - Shell command execution with safety checks
 * - Autonomous cognitive loop (perception, reasoning, action)
 * - Task management integration
 * - Memory-based learning and experience tracking
 * - Configurable loop intervals for different operation modes
 */
class AutonomousStarter {
public:
    /**
     * Constructor
     * @param config Agent configuration including name, bio, and other attributes
     */
    AutonomousStarter(const AgentConfig& config);
    
    /**
     * Destructor - ensures clean shutdown
     */
    ~AutonomousStarter();
    
    // === Core Lifecycle ===
    
    /**
     * Start the autonomous agent
     */
    void start();
    
    /**
     * Stop the autonomous agent
     */
    void stop();
    
    /**
     * Check if the agent is running
     */
    bool isRunning() const { return running_; }
    
    // === Shell Command Execution ===
    
    /**
     * Execute a shell command with safety checks
     * @param command The shell command to execute
     * @return Result including output, error, and exit code
     */
    ShellCommandResult executeShellCommand(const std::string& command);
    
    /**
     * Enable or disable shell access for security
     * @param enabled Whether shell commands should be allowed
     */
    void enableShellAccess(bool enabled) { shellAccessEnabled_ = enabled; }
    
    /**
     * Get current working directory
     */
    const std::string& getCurrentWorkingDirectory() const { return currentWorkingDirectory_; }
    
    // === Autonomous Loop Control ===
    
    /**
     * Start the autonomous cognitive loop
     * Begins continuous perception, reasoning, and action cycles
     */
    void startAutonomousLoop();
    
    /**
     * Stop the autonomous cognitive loop
     * Useful for debugging or manual control
     */
    void stopAutonomousLoop();
    
    /**
     * Check if autonomous loop is running
     */
    bool isAutonomousLoopRunning() const;
    
    /**
     * Set the interval between autonomous loop iterations
     * @param interval Time between loop cycles
     */
    void setLoopInterval(std::chrono::milliseconds interval);
    
    /**
     * Get current loop interval
     */
    std::chrono::milliseconds getLoopInterval() const { return loopInterval_; }
    
    // === State Access ===
    
    /**
     * Get mutable reference to agent state
     */
    State& getState() { return state_; }
    
    /**
     * Get const reference to agent state
     */
    const State& getState() const { return state_; }
    
    /**
     * Get agent configuration
     */
    const AgentConfig& getConfig() const { return config_; }
    
    // === Task Management ===
    
    /**
     * Execute a shell command as a task
     * @param command The command to execute
     * @return Task ID for tracking
     */
    UUID executeShellCommandAsTask(const std::string& command);
    
private:
    // === Internal Cognitive Steps ===
    
    /**
     * Perception step - gather information about environment
     */
    std::shared_ptr<void> perceptionStep(std::shared_ptr<void> input);
    
    /**
     * Reasoning step - analyze information and plan actions
     */
    std::shared_ptr<void> reasoningStep(std::shared_ptr<void> input);
    
    /**
     * Action step - execute planned actions
     */
    std::shared_ptr<void> actionStep(std::shared_ptr<void> input);
    
    // === Internal Task Worker ===
    
    /**
     * Worker for executing shell commands as tasks
     */
    class ShellCommandWorker : public TaskWorker {
    public:
        ShellCommandWorker(AutonomousStarter* starter) : starter_(starter) {}
        
        std::string getName() const override { return "shell_command"; }
        
        bool validate(const Task& task, const State& state, 
                     std::shared_ptr<Memory> message) const override;
        
        bool execute(Task& task, State& state, const TaskOptions& options) override;
        
    private:
        AutonomousStarter* starter_;
    };
    
    // === Member Variables ===
    
    AgentConfig config_;                                    // Agent configuration
    State state_;                                          // Current agent state
    std::atomic<bool> running_{false};                     // Whether agent is active
    std::atomic<bool> shellAccessEnabled_{true};           // Shell access safety flag
    
    // Autonomous loop management
    std::unique_ptr<AgentLoop> autonomousLoop_;            // Cognitive loop handler
    std::chrono::milliseconds loopInterval_{1000};        // Loop iteration interval
    
    // Task management
    std::unique_ptr<TaskManager> taskManager_;             // Task orchestration
    std::shared_ptr<ShellCommandWorker> shellWorker_;     // Shell command worker
    
    // Environment tracking
    std::string currentWorkingDirectory_;                  // Current working directory
};

// === Convenience Functions ===

/**
 * Create a pre-configured Autoliza agent instance
 * @return Shared pointer to autonomous starter configured as Autoliza
 */
std::shared_ptr<AutonomousStarter> createAutolizaAgent();

/**
 * Compatibility function for existing builds
 */
void autonomous_starter_placeholder();

} // namespace elizaos