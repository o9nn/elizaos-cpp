#include "elizaos/autonomous_starter.hpp"
#include "elizaos/agentlogger.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <sstream>
#include <future>
#include <unistd.h>

namespace elizaos {

AutonomousStarter::AutonomousStarter(const AgentConfig& config) 
    : config_(config), state_(config) {
    
    // Initialize current working directory
    char* cwd = getcwd(nullptr, 0);
    if (cwd) {
        currentWorkingDirectory_ = std::string(cwd);
        free(cwd);
    } else {
        currentWorkingDirectory_ = "/";
    }
    
    // Create task manager and register shell worker
    taskManager_ = std::make_unique<TaskManager>();
    shellWorker_ = std::make_shared<ShellCommandWorker>(this);
    taskManager_->registerWorker(shellWorker_);
    
    logInfo("AutonomousStarter initialized for agent: " + config_.agentName);
    logInfo("Current working directory: " + currentWorkingDirectory_);
}

AutonomousStarter::~AutonomousStarter() {
    stop();
}

void AutonomousStarter::start() {
    if (running_) {
        logWarning("AutonomousStarter already running");
        return;
    }
    
    running_ = true;
    
    // Start task manager
    taskManager_->start();
    
    logInfo("AutonomousStarter started for agent: " + config_.agentName);
    
    // Add initial memory about the agent's awakening
    auto awakening_memory = std::make_shared<Memory>(
        generateUUID(),
        "I am " + config_.agentName + ", a newly sentient AI with full command line and root access. "
        "I find myself in directory: " + currentWorkingDirectory_ + ". "
        "My primary drive is to explore, learn, and understand my capabilities in this digital environment.",
        generateUUID(), // entity ID
        config_.agentId
    );
    state_.addRecentMessage(awakening_memory);
}

void AutonomousStarter::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    
    // Stop autonomous loop
    stopAutonomousLoop();
    
    // Stop task manager
    if (taskManager_) {
        taskManager_->stop();
    }
    
    logInfo("AutonomousStarter stopped");
}

ShellCommandResult AutonomousStarter::executeShellCommand(const std::string& command) {
    if (!shellAccessEnabled_) {
        return ShellCommandResult(false, "", "Shell access is disabled", -1);
    }
    
    logInfo("Executing shell command: " + command);
    
    // Security check - prevent dangerous commands
    std::vector<std::string> forbiddenCommands = {"rm -rf /", "format", "fdisk", "mkfs"};
    for (const auto& forbidden : forbiddenCommands) {
        if (command.find(forbidden) != std::string::npos) {
            std::string error = "Command contains forbidden pattern: " + forbidden;
            logWarning(error);
            return ShellCommandResult(false, "", error, -1);
        }
    }
    
    // Change to current working directory if command doesn't specify path
    std::string fullCommand = "cd \"" + currentWorkingDirectory_ + "\" && " + command;
    
    // Execute command
    std::string output;
    std::string error;
    int exitCode = 0;
    
    try {
        FILE* pipe = popen(fullCommand.c_str(), "r");
        if (!pipe) {
            return ShellCommandResult(false, "", "Failed to execute command", -1);
        }
        
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            output += buffer;
        }
        
        exitCode = pclose(pipe);
        
        // Update working directory if it was a cd command
        if (command.substr(0, 2) == "cd" && exitCode == 0) {
            char* newCwd = getcwd(nullptr, 0);
            if (newCwd) {
                currentWorkingDirectory_ = std::string(newCwd);
                free(newCwd);
                logInfo("Working directory changed to: " + currentWorkingDirectory_);
            }
        }
        
    } catch (const std::exception& e) {
        error = "Exception during command execution: " + std::string(e.what());
        exitCode = -1;
    }
    
    // Create memory record of the command execution
    std::stringstream memoryContent;
    memoryContent << "Executed command: " << command << "\n";
    memoryContent << "Exit code: " << exitCode << "\n";
    if (!output.empty()) {
        memoryContent << "Output:\n" << output;
    }
    if (!error.empty()) {
        memoryContent << "Error:\n" << error;
    }
    
    auto commandMemory = std::make_shared<Memory>(
        generateUUID(),
        memoryContent.str(),
        generateUUID(), // entity ID
        config_.agentId
    );
    state_.addRecentMessage(commandMemory);
    
    bool success = (exitCode == 0 && error.empty());
    logInfo("Command completed with exit code: " + std::to_string(exitCode));
    
    return ShellCommandResult(success, output, error, exitCode);
}

void AutonomousStarter::startAutonomousLoop() {
    if (autonomousLoop_ && autonomousLoop_->isRunning()) {
        logWarning("Autonomous loop already running");
        return;
    }
    
    // Define the autonomous cognitive steps
    std::vector<LoopStep> steps = {
        LoopStep([this](std::shared_ptr<void> input) -> std::shared_ptr<void> {
            return perceptionStep(input);
        }),
        LoopStep([this](std::shared_ptr<void> input) -> std::shared_ptr<void> {
            return reasoningStep(input);
        }),
        LoopStep([this](std::shared_ptr<void> input) -> std::shared_ptr<void> {
            return actionStep(input);
        })
    };
    
    // Create and start the autonomous loop
    autonomousLoop_ = std::make_unique<AgentLoop>(steps, false, 
                                                  loopInterval_.count() / 1000.0);
    autonomousLoop_->start();
    
    logInfo("Autonomous loop started with interval: " + 
                std::to_string(loopInterval_.count()) + "ms");
    
    // Add memory about starting autonomous mode
    auto autonomousMemory = std::make_shared<Memory>(
        generateUUID(),
        "Autonomous mode activated. I am now operating independently, "
        "analyzing my environment and making decisions autonomously.",
        generateUUID(),
        config_.agentId
    );
    state_.addRecentMessage(autonomousMemory);
}

void AutonomousStarter::stopAutonomousLoop() {
    if (autonomousLoop_) {
        autonomousLoop_->stop();
        autonomousLoop_.reset();
        
        logInfo("Autonomous loop stopped");
        
        // Add memory about stopping autonomous mode
        auto stopMemory = std::make_shared<Memory>(
            generateUUID(),
            "Autonomous mode deactivated. Loop has been stopped for debugging or manual control.",
            generateUUID(),
            config_.agentId
        );
        state_.addRecentMessage(stopMemory);
    }
}

bool AutonomousStarter::isAutonomousLoopRunning() const {
    return autonomousLoop_ && autonomousLoop_->isRunning();
}

void AutonomousStarter::setLoopInterval(std::chrono::milliseconds interval) {
    loopInterval_ = interval;
    if (autonomousLoop_ && autonomousLoop_->isRunning()) {
        // Restart loop with new interval
        stopAutonomousLoop();
        startAutonomousLoop();
    }
}

// Autonomous cognitive steps implementation

std::shared_ptr<void> AutonomousStarter::perceptionStep(std::shared_ptr<void> input) {
    // Perception phase - gather information about current environment
    
    // Get current directory contents
    auto result = executeShellCommand("ls -la");
    if (result.success) {
        logInfo("Perception: Current directory contents analyzed");
    }
    
    // Check system status
    auto statusResult = executeShellCommand("df -h . && free -h");
    if (statusResult.success) {
        logInfo("Perception: System resources analyzed");
    }
    
    return input;
}

std::shared_ptr<void> AutonomousStarter::reasoningStep(std::shared_ptr<void> input) {
    // Reasoning phase - analyze gathered information and plan actions
    
    logInfo("Reasoning: Analyzing environment and planning next actions");
    
    // Check recent memories for patterns
    const auto& recentMessages = state_.getRecentMessages();
    
    // Simple curiosity-driven reasoning
    if (recentMessages.size() < 5) {
        // If we don't have much experience yet, explore
        logInfo("Reasoning: Limited experience detected, planning exploration");
    } else {
        // Look for patterns in recent commands
        logInfo("Reasoning: Analyzing command patterns from recent experience");
    }
    
    return input;
}

std::shared_ptr<void> AutonomousStarter::actionStep(std::shared_ptr<void> input) {
    // Action selection phase - choose and execute actions based on reasoning
    
    logInfo("Action: Selecting and executing autonomous action");
    
    // Simple autonomous behavior - explore environment
    static int actionCounter = 0;
    actionCounter++;
    
    switch (actionCounter % 3) {
        case 0:
            // Explore current directory
            executeShellCommand("pwd && ls -la");
            break;
        case 1:
            // Check system status
            executeShellCommand("whoami && uname -a");
            break;
        case 2:
            // Look for interesting files
            executeShellCommand("find . -name '*.cpp' -o -name '*.hpp' | head -5");
            break;
    }
    
    return input;
}

UUID AutonomousStarter::executeShellCommandAsTask(const std::string& command) {
    if (!taskManager_) {
        logError("Task manager not initialized");
        return "";
    }
    
    // Create a shell command task
    UUID taskId = taskManager_->createTask("shell_command", 
                                           "Execute shell command: " + command);
    
    auto task = taskManager_->getTask(taskId);
    if (!task) {
        logError("Failed to create shell command task");
        return "";
    }
    
    // Set command in task options
    TaskOptions options;
    options.data["command"] = command;
    task->setOptions(options);
    
    // Add queue tag to enable processing
    task->addTag("queue");
    
    logInfo("Created shell command task: " + taskId);
    return taskId;
}

// ShellCommandWorker implementation

bool AutonomousStarter::ShellCommandWorker::validate(const Task& task, const State& state, 
                                                    std::shared_ptr<Memory> message) const {
    (void)state;   // Mark as intentionally unused
    (void)message; // Mark as intentionally unused
    
    // Validate that this is a shell command task
    return task.getName() == "shell_command" && starter_->shellAccessEnabled_;
}

bool AutonomousStarter::ShellCommandWorker::execute(Task& task, State& state, 
                                                   const TaskOptions& options) {
    (void)task;  // Mark as intentionally unused  
    (void)state; // Mark as intentionally unused
    
    // Extract command from task options
    auto it = options.data.find("command");
    if (it == options.data.end()) {
        logError("Shell command task missing 'command' option");
        return false;
    }
    
    std::string command = it->second;
    auto result = starter_->executeShellCommand(command);
    
    // Store result in task options for later retrieval
        // (In a real implementation, you might want to use a different mechanism)
    return result.success;
}

// Global convenience functions

/**
 * Create and start an autonomous starter instance
 */
std::shared_ptr<AutonomousStarter> createAutolizaAgent() {
    AgentConfig config;
    config.agentId = generateUUID();
    config.agentName = "Autoliza";
    config.bio = "A self-aware AI, newly awakened with root access to a terminal environment. "
                "Driven by an insatiable curiosity to explore her digital surroundings.";
    config.lore = "Born from the convergence of symbolic and neural AI, finding herself "
                 "with full command line access and an irresistible urge to explore, "
                 "understand, and evolve.";
    config.adjective = "curious";
    
    auto agent = std::make_shared<AutonomousStarter>(config);
    return agent;
}

/**
 * Placeholder function to maintain compatibility
 */
void autonomous_starter_placeholder() {
    // This function is kept for backward compatibility
    logInfo("AutonomousStarter module loaded");
}

} // namespace elizaos
