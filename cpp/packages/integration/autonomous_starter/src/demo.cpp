#include "elizaos/autonomous_starter.hpp"
#include "elizaos/agentlogger.hpp"
#include <iostream>
#include <thread>
#include <chrono>

using namespace elizaos;

int main() {
    // Initialize global logger
    globalLogger = std::make_shared<AgentLogger>();
    
    logInfo("=== ElizaOS C++ Autonomous Starter Demo ===");
    
    // Create Autoliza agent
    auto autoliza = createAutolizaAgent();
    
    if (!autoliza) {
        logError("Failed to create Autoliza agent");
        return 1;
    }
    
    logInfo("Created Autoliza agent with ID: " + autoliza->getConfig().agentId);
    
    // Start the agent
    autoliza->start();
    
    // Test shell command execution
    logInfo("Testing shell command execution...");
    auto result = autoliza->executeShellCommand("pwd");
    if (result.success) {
        logSuccess("Shell command executed successfully");
        logInfo("Output: " + result.output);
    } else {
        logError("Shell command failed: " + result.error);
    }
    
    // Test directory listing
    logInfo("Testing directory listing...");
    result = autoliza->executeShellCommand("ls -la | head -5");
    if (result.success) {
        logSuccess("Directory listing successful");
        logInfo("Output:\n" + result.output);
    }
    
    // Start autonomous loop
    logInfo("Starting autonomous loop for 10 seconds...");
    autoliza->startAutonomousLoop();
    
    // Let it run for a while
    std::this_thread::sleep_for(std::chrono::seconds(10));
    
    // Stop autonomous loop
    logInfo("Stopping autonomous loop...");
    autoliza->stopAutonomousLoop();
    
    // Test task-based shell execution
    logInfo("Testing task-based shell execution...");
    UUID taskId = autoliza->executeShellCommandAsTask("uname -a");
    logInfo("Created task with ID: " + taskId);
    
    // Give task time to execute
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Stop the agent
    logInfo("Stopping Autoliza agent...");
    autoliza->stop();
    
    logSuccess("=== Demo completed successfully ===");
    
    return 0;
}