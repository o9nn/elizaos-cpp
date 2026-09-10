#pragma once

/**
 * ElizaOS C++ - AgentShell Module
 *
 * Interactive command-line shell for agent operations with builtin
 * command registration and command history.
 */

#include <atomic>
#include <condition_variable>
#include <cstddef>
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
    bool success = true;
    std::string output;
    std::string error;
    int exitCode = 0;

    ShellCommandResult() = default;
    ShellCommandResult(bool success, const std::string& output,
                       const std::string& error = "", int exitCode = 0)
        : success(success), output(output), error(error), exitCode(exitCode) {}
};

// ============================================================================
// AgentShell
// ============================================================================

// Handlers receive only arguments; the registered command name is removed.
using CommandHandler = std::function<ShellCommandResult(const std::vector<std::string>& args)>;

class AgentShell {
public:
    using InputReader = std::function<bool(const std::string& prompt, std::string& command)>;
    using InputCanceller = std::function<void()>;

    static constexpr std::size_t MaxCommandLength = 4096;
    static constexpr std::size_t MaxCommandTokens = 128;
    static constexpr std::size_t MaxTokenLength = 1024;
    static constexpr std::size_t MaxCommandNameLength = 64;
    static constexpr std::size_t MaxPromptLength = 256;
    static constexpr std::size_t MaxHistoryEntries = 1000;

    AgentShell();
    ~AgentShell() noexcept;

    AgentShell(const AgentShell&) = delete;
    AgentShell& operator=(const AgentShell&) = delete;
    AgentShell(AgentShell&&) = delete;
    AgentShell& operator=(AgentShell&&) = delete;

    // Lifecycle
    void start(const std::string& prompt = "eliza> ");
    void stop();
    bool isRunning() const noexcept { return running_.load(std::memory_order_acquire); }

    // Command execution
    ShellCommandResult executeCommand(const std::string& command);

    // Command registration
    void registerCommand(const std::string& commandName, CommandHandler handler);
    void unregisterCommand(const std::string& commandName);
    std::vector<std::string> getAvailableCommands() const;

    // Configuration
    void setPrompt(const std::string& prompt);
    void setHistoryEnabled(bool enabled);
    // The optional canceller must unblock a pending reader. Input handlers may
    // only be replaced while the shell is stopped.
    void setInputHandler(InputReader reader, InputCanceller canceller = {});
    void resetInputHandler();

    // History
    std::vector<std::string> getHistory() const;
    void clearHistory();

private:
    void shellLoop();
    void initializeBuiltinCommands();
    std::vector<std::string> parseCommand(const std::string& command,
                                          std::string& error) const;

    // Builtin commands
    ShellCommandResult helpCommand(const std::vector<std::string>& args);
    ShellCommandResult exitCommand(const std::vector<std::string>& args);
    ShellCommandResult historyCommand(const std::vector<std::string>& args);
    ShellCommandResult clearCommand(const std::vector<std::string>& args);
    ShellCommandResult echoCommand(const std::vector<std::string>& args);
    ShellCommandResult statusCommand(const std::vector<std::string>& args);
    ShellCommandResult versionCommand(const std::vector<std::string>& args);
    ShellCommandResult infoCommand(const std::vector<std::string>& args);

    std::string prompt_ = "eliza> ";
    bool historyEnabled_ = true;
    std::shared_ptr<const InputReader> inputReader_;
    std::shared_ptr<const InputCanceller> inputCanceller_;
    std::atomic<bool> running_{false};

    std::vector<std::string> commandHistory_;
    std::unordered_map<std::string, std::shared_ptr<const CommandHandler>> commandHandlers_;

    std::thread shellThread_;
    std::thread::id workerThreadId_{};
    bool workerFinished_ = true;
    bool selfStopPending_ = false;
    bool joinInProgress_ = false;

    mutable std::mutex commandsMutex_;
    mutable std::mutex historyMutex_;
    mutable std::mutex configMutex_;
    mutable std::mutex lifecycleMutex_;
    std::condition_variable lifecycleCv_;
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
bool isShellRunning();
std::vector<std::string> getAvailableShellCommands();

} // namespace elizaos
