#pragma once

/**
 * ElizaOS C++ - AgentShell Module
 *
 * Interactive command-line shell for agent operations with builtin
 * command registration and command history.
 */

#include "elizaos.hpp"
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace elizaos {

// ============================================================================
// ShellCommandResult
// ============================================================================

struct ShellCommandResult {
    bool        success  = true;
    std::string output;
    std::string error;
    int         exitCode = 0;

    ShellCommandResult() = default;
    ShellCommandResult(bool success, const std::string& output,
                       const std::string& error = "", int exitCode = 0)
        : success(success), output(output), error(error), exitCode(exitCode) {}
};

// ============================================================================
// AgentShell
// ============================================================================

using CommandHandler = std::function<ShellCommandResult(const std::vector<std::string>& args)>;

class AgentShell {
public:
    AgentShell();
    ~AgentShell();

    // Lifecycle
    void start(const std::string& prompt = "eliza> ");
    void stop();
    bool isRunning() const { return running_; }

    // Command execution
    ShellCommandResult executeCommand(const std::string& command);

    // Command registration
    void registerCommand(const std::string& commandName, CommandHandler handler);
    void unregisterCommand(const std::string& commandName);
    std::vector<std::string> getAvailableCommands() const;

    // Configuration
    void setPrompt(const std::string& prompt);
    void setHistoryEnabled(bool enabled);

    // History
    const std::vector<std::string>& getHistory() const;
    void clearHistory();

private:
    void shellLoop();
    void initializeBuiltinCommands();
    std::vector<std::string> parseCommand(const std::string& command);

    // Builtin commands
    ShellCommandResult helpCommand(const std::vector<std::string>& args);
    ShellCommandResult exitCommand(const std::vector<std::string>& args);
    ShellCommandResult historyCommand(const std::vector<std::string>& args);
    ShellCommandResult clearCommand(const std::vector<std::string>& args);
    ShellCommandResult echoCommand(const std::vector<std::string>& args);
    ShellCommandResult statusCommand(const std::vector<std::string>& args);
    ShellCommandResult versionCommand(const std::vector<std::string>& args);
    ShellCommandResult infoCommand(const std::vector<std::string>& args);

    std::string prompt_          = "eliza> ";
    bool        historyEnabled_  = true;
    std::atomic<bool> running_{false};
    std::atomic<bool> stopRequested_{false};

    std::vector<std::string>                            history_;
    std::unordered_map<std::string, CommandHandler>     commands_;

    std::unique_ptr<std::thread> shellThread_;
    mutable std::mutex           shellMutex_;
};

// ============================================================================
// Global shell instance and convenience functions
// ============================================================================

extern std::shared_ptr<AgentShell> globalShell;

void startInteractiveShell();
void stopInteractiveShell();
bool executeShellCommand(const std::string& command);
void registerShellCommand(const std::string& name, CommandHandler handler);
ShellCommandResult executeShellCommandWithResult(const std::string& command);

} // namespace elizaos
