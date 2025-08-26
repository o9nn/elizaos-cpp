#include "elizaos/agentshell.hpp"
#include "elizaos/agentlogger.hpp"
#include "elizaos/agentcomms.hpp"
#include <iostream>
#include <thread>
#include <chrono>

using namespace elizaos;

int main() {
    std::cout << "ElizaOS C++ Interactive Shell Demo" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << std::endl;
    
    // Initialize logger
    globalLogger->printHeader("AgentShell Demo", LogColor::GREEN);
    
    // Register a custom command for demo
    registerShellCommand("demo", [](const std::vector<std::string>& args) -> ShellCommandResult {
        (void)args; // Suppress unused parameter warning
        
        logInfo("This is a demo command!", "shell-demo");
        logSuccess("AgentShell is working correctly!", "shell-demo");
        
        return ShellCommandResult(true, "Demo command executed successfully!", "", 0);
    });
    
    // Register an agent command
    registerShellCommand("agent", [](const std::vector<std::string>& args) -> ShellCommandResult {
        if (args.size() < 2) {
            return ShellCommandResult(false, "", "Usage: agent <command>", 1);
        }
        
        std::string subCommand = args[1];
        
        if (subCommand == "start") {
            logInfo("Starting agent systems...", "shell-demo");
            initializeComms();
            return ShellCommandResult(true, "Agent systems started", "", 0);
        } else if (subCommand == "stop") {
            logInfo("Stopping agent systems...", "shell-demo");
            shutdownComms();
            return ShellCommandResult(true, "Agent systems stopped", "", 0);
        } else {
            return ShellCommandResult(false, "", "Unknown agent command: " + subCommand, 1);
        }
    });
    
    logInfo("Starting interactive shell...", "shell-demo");
    logInfo("Try these commands: demo, agent start, agent stop, status, help, exit", "shell-demo");
    
    // Start the shell and wait for it to finish
    startInteractiveShell();
    
    // Wait for shell to complete
    while (globalShell->isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Ensure proper cleanup
    globalShell->stop();
    
    logInfo("Shell session ended", "shell-demo");
    
    return 0;
}