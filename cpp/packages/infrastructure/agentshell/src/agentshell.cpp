#include "elizaos/agentshell.hpp"
#include "elizaos/agentlogger.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

// Conditional readline support
#ifdef HAVE_READLINE
#include <readline/history.h>
#include <readline/readline.h>
#endif

namespace elizaos {
namespace {

const std::string kDefaultPrompt = "eliza> ";

#ifdef HAVE_READLINE
std::mutex readlineHistoryMutex;
#endif

bool isValidCommandName(const std::string& name) {
    if (name.empty() || name.size() > AgentShell::MaxCommandNameLength) {
        return false;
    }

    return std::all_of(name.begin(), name.end(), [](const unsigned char character) {
        return std::isalnum(character) != 0 || character == '_' || character == '-';
    });
}

ShellCommandResult invalidArguments(const std::string& command) {
    return ShellCommandResult(false, "", command + " does not accept arguments", 2);
}

} // namespace

// Global shell instance
std::shared_ptr<AgentShell> globalShell = std::make_shared<AgentShell>();

AgentShell::AgentShell()
    : prompt_(kDefaultPrompt) {
    initializeBuiltinCommands();
}

AgentShell::~AgentShell() noexcept {
    try {
        stop();
    } catch (...) {
        // Destructors must not allow logging, allocation, or custom input
        // cancellation failures to terminate the process.
    }
}

void AgentShell::start(const std::string& prompt) {
    if (prompt.size() > MaxPromptLength) {
        throw std::invalid_argument("Prompt exceeds maximum length");
    }

    std::thread previousWorker;
    std::unique_lock<std::mutex> lifecycleLock(lifecycleMutex_);
    lifecycleCv_.wait(lifecycleLock, [this] { return !joinInProgress_; });

    if (running_.load(std::memory_order_acquire)) {
        return;
    }
    if (workerThreadId_ == std::this_thread::get_id()) {
        return;
    }

    if (shellThread_.joinable() && selfStopPending_ && !workerFinished_) {
        lifecycleCv_.wait(lifecycleLock, [this] { return workerFinished_; });
    }
    if (shellThread_.joinable()) {
        joinInProgress_ = true;
        previousWorker = std::move(shellThread_);
        lifecycleLock.unlock();
        previousWorker.join();
        lifecycleLock.lock();
        selfStopPending_ = false;
        joinInProgress_ = false;
        lifecycleCv_.notify_all();
    }

    {
        std::lock_guard<std::mutex> configLock(configMutex_);
        prompt_ = prompt;
    }

    workerFinished_ = false;
    selfStopPending_ = false;
    running_.store(true, std::memory_order_release);
    try {
        shellThread_ = std::thread(&AgentShell::shellLoop, this);
    } catch (...) {
        running_.store(false, std::memory_order_release);
        workerFinished_ = true;
        lifecycleCv_.notify_all();
        throw;
    }
    logInfo("Interactive shell started", "agentshell");
}

void AgentShell::stop() {
    std::shared_ptr<const InputCanceller> canceller;
    std::thread worker;
    bool stoppedRunningShell = false;

    std::unique_lock<std::mutex> lifecycleLock(lifecycleMutex_);
    lifecycleCv_.wait(lifecycleLock, [this] { return !joinInProgress_; });

    stoppedRunningShell = running_.exchange(false, std::memory_order_acq_rel);
    const bool shouldCancelInput = shellThread_.joinable() && !workerFinished_;
    if (shouldCancelInput) {
        std::lock_guard<std::mutex> configLock(configMutex_);
        canceller = inputCanceller_;
    }

    if (shellThread_.joinable() && workerThreadId_ == std::this_thread::get_id()) {
        // The shell thread cannot join itself. Ownership remains on this object;
        // the next external stop/start or the destructor performs the join.
        selfStopPending_ = true;
        return;
    }

    if (shellThread_.joinable()) {
        joinInProgress_ = true;
        worker = std::move(shellThread_);
    } else if (workerThreadId_ != std::thread::id{}) {
        lifecycleCv_.wait(lifecycleLock, [this] { return workerFinished_; });
    }

    lifecycleLock.unlock();
    if (canceller) {
        try {
            (*canceller)();
        } catch (const std::exception& error) {
            logError("Input cancellation error: " + std::string(error.what()), "agentshell");
        } catch (...) {
            logError("Input cancellation error: unknown exception", "agentshell");
        }
    }
    if (worker.joinable()) {
        worker.join();
    }
    lifecycleLock.lock();

    if (joinInProgress_) {
        selfStopPending_ = false;
        joinInProgress_ = false;
        lifecycleCv_.notify_all();
    }
    lifecycleCv_.wait(lifecycleLock, [this] { return workerFinished_; });
    lifecycleLock.unlock();

    if (stoppedRunningShell) {
        logInfo("Interactive shell stopped", "agentshell");
    }
}

void AgentShell::shellLoop() {
    {
        std::lock_guard<std::mutex> lifecycleLock(lifecycleMutex_);
        workerThreadId_ = std::this_thread::get_id();
    }

    logSuccess("ElizaOS Interactive Shell", "agentshell");
    logInfo("Type 'help' for available commands, 'exit' to quit", "agentshell");

    try {
        while (running_.load(std::memory_order_acquire)) {
            std::string command;
            std::string prompt;
            std::shared_ptr<const InputReader> reader;
            [[maybe_unused]] bool historyEnabled = false;
            {
                std::lock_guard<std::mutex> configLock(configMutex_);
                prompt = prompt_;
                reader = inputReader_;
                historyEnabled = historyEnabled_;
            }

            bool receivedInput = false;
            if (reader) {
                receivedInput = (*reader)(prompt, command);
            } else {
#ifdef HAVE_READLINE
                char* input = readline(prompt.c_str());
                if (input != nullptr) {
                    command.assign(input);
                    std::free(input);
                    receivedInput = true;
                    if (!command.empty() && historyEnabled) {
                        std::lock_guard<std::mutex> historyLock(readlineHistoryMutex);
                        add_history(command.c_str());
                    }
                }
#else
                std::cout << prompt << std::flush;
                receivedInput = static_cast<bool>(std::getline(std::cin, command));
#endif
            }

            if (!receivedInput || !running_.load(std::memory_order_acquire)) {
                break;
            }
            if (command.empty()) {
                continue;
            }

            const auto result = executeCommand(command);
            if (!result.output.empty()) {
                std::cout << result.output << '\n';
            }
            if (!result.success && !result.error.empty()) {
                logError(result.error, "agentshell");
            }
        }
    } catch (const std::exception& error) {
        logError("Shell input error: " + std::string(error.what()), "agentshell");
    } catch (...) {
        logError("Shell input error: unknown exception", "agentshell");
    }

    // Publish completion under the same mutex used by lifecycle waiters.
    {
        std::lock_guard<std::mutex> lifecycleLock(lifecycleMutex_);
        running_.store(false, std::memory_order_release);
        workerFinished_ = true;
        workerThreadId_ = std::thread::id{};
    }
    lifecycleCv_.notify_all();
}

ShellCommandResult AgentShell::executeCommand(const std::string& command) {
    std::string parseError;
    const auto tokens = parseCommand(command, parseError);
    if (!parseError.empty()) {
        return ShellCommandResult(false, "", parseError, 2);
    }
    if (tokens.empty()) {
        return ShellCommandResult(true, "", "", 0);
    }

    bool historyEnabled = false;
    {
        std::lock_guard<std::mutex> configLock(configMutex_);
        historyEnabled = historyEnabled_;
    }
    if (historyEnabled) {
        std::lock_guard<std::mutex> historyLock(historyMutex_);
        if (commandHistory_.size() == MaxHistoryEntries) {
            commandHistory_.erase(commandHistory_.begin());
        }
        commandHistory_.push_back(command);
    }

    std::shared_ptr<const CommandHandler> handler;
    {
        std::lock_guard<std::mutex> commandsLock(commandsMutex_);
        const auto it = commandHandlers_.find(tokens.front());
        if (it != commandHandlers_.end()) {
            handler = it->second;
        }
    }

    if (!handler) {
        return ShellCommandResult(false, "",
                                  "Unknown command: " + tokens.front() +
                                      " (type 'help' for available commands)",
                                  1);
    }

    try {
        return (*handler)(std::vector<std::string>(tokens.begin() + 1, tokens.end()));
    } catch (const std::exception& error) {
        return ShellCommandResult(false, "", "Command error: " + std::string(error.what()), 1);
    } catch (...) {
        return ShellCommandResult(false, "", "Command error: unknown exception", 1);
    }
}

std::vector<std::string> AgentShell::parseCommand(const std::string& command,
                                                   std::string& error) const {
    std::vector<std::string> tokens;
    std::string token;
    char quote = '\0';
    bool escaped = false;
    bool tokenStarted = false;

    if (command.size() > MaxCommandLength) {
        error = "Command exceeds maximum length";
        return tokens;
    }

    const auto appendToken = [&tokens, &token, &tokenStarted, &error]() -> bool {
        if (!tokenStarted) {
            return true;
        }
        if (token.size() > MaxTokenLength) {
            error = "Command token exceeds maximum length";
            return false;
        }
        if (tokens.size() == MaxCommandTokens) {
            error = "Command contains too many tokens";
            return false;
        }
        tokens.push_back(token);
        token.clear();
        tokenStarted = false;
        return true;
    };

    for (const char rawCharacter : command) {
        const auto character = static_cast<unsigned char>(rawCharacter);
        if (escaped) {
            token.push_back(static_cast<char>(character));
            tokenStarted = true;
            escaped = false;
        } else if (character == '\\') {
            escaped = true;
            tokenStarted = true;
        } else if (quote != '\0') {
            if (character == static_cast<unsigned char>(quote)) {
                quote = '\0';
            } else {
                token.push_back(static_cast<char>(character));
            }
            tokenStarted = true;
        } else if (character == '\'' || character == '"') {
            quote = static_cast<char>(character);
            tokenStarted = true;
        } else if (std::isspace(character) != 0) {
            if (!appendToken()) {
                return {};
            }
        } else {
            token.push_back(static_cast<char>(character));
            tokenStarted = true;
        }

        if (token.size() > MaxTokenLength) {
            error = "Command token exceeds maximum length";
            return {};
        }
    }

    if (escaped) {
        error = "Malformed command: trailing escape";
        return {};
    }
    if (quote != '\0') {
        error = "Malformed command: unterminated quote";
        return {};
    }
    if (!appendToken()) {
        return {};
    }
    return tokens;
}

void AgentShell::registerCommand(const std::string& commandName, CommandHandler handler) {
    if (!isValidCommandName(commandName)) {
        throw std::invalid_argument("Invalid command name");
    }
    if (!handler) {
        throw std::invalid_argument("Command handler must not be empty");
    }

    auto registeredHandler = std::make_shared<const CommandHandler>(std::move(handler));
    std::shared_ptr<const CommandHandler> retiredHandler;
    {
        std::lock_guard<std::mutex> commandsLock(commandsMutex_);
        retiredHandler = std::move(commandHandlers_[commandName]);
        commandHandlers_[commandName] = std::move(registeredHandler);
    }
}

void AgentShell::unregisterCommand(const std::string& commandName) {
    if (!isValidCommandName(commandName)) {
        return;
    }

    std::shared_ptr<const CommandHandler> retiredHandler;
    {
        std::lock_guard<std::mutex> commandsLock(commandsMutex_);
        const auto command = commandHandlers_.find(commandName);
        if (command != commandHandlers_.end()) {
            retiredHandler = std::move(command->second);
            commandHandlers_.erase(command);
        }
    }
}

std::vector<std::string> AgentShell::getAvailableCommands() const {
    std::vector<std::string> commands;
    {
        std::lock_guard<std::mutex> commandsLock(commandsMutex_);
        commands.reserve(commandHandlers_.size());
        for (const auto& entry : commandHandlers_) {
            commands.push_back(entry.first);
        }
    }
    std::sort(commands.begin(), commands.end());
    return commands;
}

void AgentShell::setPrompt(const std::string& prompt) {
    if (prompt.size() > MaxPromptLength) {
        throw std::invalid_argument("Prompt exceeds maximum length");
    }
    std::lock_guard<std::mutex> configLock(configMutex_);
    prompt_ = prompt;
}

void AgentShell::setHistoryEnabled(const bool enabled) {
    std::lock_guard<std::mutex> configLock(configMutex_);
    historyEnabled_ = enabled;
}

void AgentShell::setInputHandler(InputReader reader, InputCanceller canceller) {
    if (!reader) {
        throw std::invalid_argument("Input reader must not be empty");
    }

    auto sharedReader = std::make_shared<const InputReader>(std::move(reader));
    std::shared_ptr<const InputCanceller> sharedCanceller;
    if (canceller) {
        sharedCanceller = std::make_shared<const InputCanceller>(std::move(canceller));
    }

    std::lock_guard<std::mutex> lifecycleLock(lifecycleMutex_);
    if (running_.load(std::memory_order_acquire) || shellThread_.joinable() || joinInProgress_) {
        throw std::logic_error("Input handler can only be changed while the shell is stopped");
    }
    std::lock_guard<std::mutex> configLock(configMutex_);
    inputReader_ = std::move(sharedReader);
    inputCanceller_ = std::move(sharedCanceller);
}

void AgentShell::resetInputHandler() {
    std::lock_guard<std::mutex> lifecycleLock(lifecycleMutex_);
    if (running_.load(std::memory_order_acquire) || shellThread_.joinable() || joinInProgress_) {
        throw std::logic_error("Input handler can only be changed while the shell is stopped");
    }
    std::lock_guard<std::mutex> configLock(configMutex_);
    inputReader_ = {};
    inputCanceller_ = {};
}

std::vector<std::string> AgentShell::getHistory() const {
    std::lock_guard<std::mutex> historyLock(historyMutex_);
    return commandHistory_;
}

void AgentShell::clearHistory() {
    {
        std::lock_guard<std::mutex> historyLock(historyMutex_);
        commandHistory_.clear();
    }
#ifdef HAVE_READLINE
    std::lock_guard<std::mutex> readlineLock(readlineHistoryMutex);
    clear_history();
#endif
}

void AgentShell::initializeBuiltinCommands() {
    registerCommand("help", [this](const std::vector<std::string>& args) { return helpCommand(args); });
    registerCommand("exit", [this](const std::vector<std::string>& args) { return exitCommand(args); });
    registerCommand("quit", [this](const std::vector<std::string>& args) { return exitCommand(args); });
    registerCommand("history", [this](const std::vector<std::string>& args) { return historyCommand(args); });
    registerCommand("clear", [this](const std::vector<std::string>& args) { return clearCommand(args); });
    registerCommand("echo", [this](const std::vector<std::string>& args) { return echoCommand(args); });
    registerCommand("status", [this](const std::vector<std::string>& args) { return statusCommand(args); });
    registerCommand("version", [this](const std::vector<std::string>& args) { return versionCommand(args); });
    registerCommand("info", [this](const std::vector<std::string>& args) { return infoCommand(args); });
}

ShellCommandResult AgentShell::helpCommand(const std::vector<std::string>& args) {
    if (!args.empty()) {
        return invalidArguments("help");
    }

    std::stringstream output;
    output << "Available commands:\n";
    for (const auto& command : getAvailableCommands()) {
        output << "  " << command;
        if (command == "help") output << " - Show this help message";
        else if (command == "exit" || command == "quit") output << " - Exit the shell";
        else if (command == "history") output << " - Show command history";
        else if (command == "clear") output << " - Clear the screen";
        else if (command == "echo") output << " - Echo text to output";
        else if (command == "status") output << " - Show system status";
        else if (command == "version") output << " - Show ElizaOS version information";
        else if (command == "info") output << " - Show detailed system information";
        output << '\n';
    }
    return ShellCommandResult(true, output.str(), "", 0);
}

ShellCommandResult AgentShell::exitCommand(const std::vector<std::string>& args) {
    if (!args.empty()) {
        return invalidArguments("exit");
    }
    stop();
    return ShellCommandResult(true, "Goodbye!", "", 0);
}

ShellCommandResult AgentShell::historyCommand(const std::vector<std::string>& args) {
    if (!args.empty()) {
        return invalidArguments("history");
    }

    const auto history = getHistory();
    if (history.empty()) {
        return ShellCommandResult(true, "No command history", "", 0);
    }

    std::stringstream output;
    for (std::size_t index = 0; index < history.size(); ++index) {
        output << "  " << (index + 1) << ": " << history[index] << '\n';
    }
    return ShellCommandResult(true, output.str(), "", 0);
}

ShellCommandResult AgentShell::clearCommand(const std::vector<std::string>& args) {
    if (!args.empty()) {
        return invalidArguments("clear");
    }
    return ShellCommandResult(true, "\033[2J\033[H", "", 0);
}

ShellCommandResult AgentShell::echoCommand(const std::vector<std::string>& args) {
    std::stringstream output;
    for (std::size_t index = 0; index < args.size(); ++index) {
        if (index != 0) {
            output << ' ';
        }
        output << args[index];
    }
    return ShellCommandResult(true, output.str(), "", 0);
}

ShellCommandResult AgentShell::statusCommand(const std::vector<std::string>& args) {
    if (!args.empty()) {
        return invalidArguments("status");
    }

    std::stringstream output;
    output << "ElizaOS C++ Framework Status:\n";
    output << "  Shell: " << (isRunning() ? "Running" : "Stopped") << '\n';
    output << "  Logger: Available\n";
    output << "  Communications: Available\n";
    output << "  Memory: Available\n";
    output << "  Agent Loop: Available\n";
    output << "  Commands registered: " << getAvailableCommands().size() << '\n';
#ifdef HAVE_READLINE
    output << "  Input mode: Enhanced (readline)\n";
#else
    output << "  Input mode: Basic\n";
#endif
    return ShellCommandResult(true, output.str(), "", 0);
}

ShellCommandResult AgentShell::versionCommand(const std::vector<std::string>& args) {
    if (!args.empty()) {
        return invalidArguments("version");
    }

    std::stringstream output;
    output << "ElizaOS C++ Framework Version 1.0.0\n";
    output << "Build: " << __DATE__ << ' ' << __TIME__ << '\n';
    output << "C++ Standard: " << __cplusplus << '\n';
    output << "Compiler: ";
#ifdef __clang__
    output << "Clang " << __clang_major__ << '.' << __clang_minor__ << '.' << __clang_patchlevel__;
#elif defined(__GNUC__)
    output << "GCC " << __GNUC__ << '.' << __GNUC_MINOR__ << '.' << __GNUC_PATCHLEVEL__;
#elif defined(_MSC_VER)
    output << "MSVC " << _MSC_VER;
#else
    output << "Unknown";
#endif
    output << '\n';
    return ShellCommandResult(true, output.str(), "", 0);
}

ShellCommandResult AgentShell::infoCommand(const std::vector<std::string>& args) {
    if (!args.empty()) {
        return invalidArguments("info");
    }

    std::stringstream output;
    output << "ElizaOS C++ Framework - Advanced Agent System\n";
    output << "=============================================\n\n";
    output << "Core Components:\n";
    output << "  - Agent Shell: Interactive command interface\n";
    output << "  - Agent Logger: Comprehensive logging system\n";
    output << "  - Agent Memory: Vector-based memory management\n";
    output << "  - Agent Communications: Multi-protocol messaging\n";
    output << "  - Agent Loop: Event-driven agent execution\n";
    output << "  - Agent Actions: Task processing and execution\n\n";
    output << "Features:\n";
    output << "  - Command-line interface with history\n";
    output << "  - Thread-safe operations\n";
    output << "  - Extensible command system\n";
    output << "  - Real-time agent interaction\n";
    output << "  - Integrated logging and monitoring\n\n";
    output << "Active Commands: " << getAvailableCommands().size() << '\n';
    output << "Shell Status: " << (isRunning() ? "Running" : "Stopped") << '\n';
    return ShellCommandResult(true, output.str(), "", 0);
}

// Convenience functions
void startInteractiveShell() {
    globalShell->start();
}

void stopInteractiveShell() {
    globalShell->stop();
}

bool executeShellCommand(const std::string& command) {
    return globalShell->executeCommand(command).success;
}

void registerShellCommand(const std::string& name, CommandHandler handler) {
    globalShell->registerCommand(name, std::move(handler));
}

ShellCommandResult executeShellCommandWithResult(const std::string& command) {
    return globalShell->executeCommand(command);
}

bool isShellRunning() {
    return globalShell->isRunning();
}

std::vector<std::string> getAvailableShellCommands() {
    return globalShell->getAvailableCommands();
}

} // namespace elizaos
