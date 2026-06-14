#include "elizaos/autonomous_starter.hpp"
#include "elizaos/agentlogger.hpp"

#include <algorithm>
#include <array>
#include <cerrno>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

#ifdef _MSC_VER
#include <direct.h>  // _getcwd on Windows
#define getcwd _getcwd
#define popen _popen
#define pclose _pclose
#else
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace elizaos {
namespace {

std::string trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    const auto last = value.find_last_not_of(" \t\n\r");
    return value.substr(first, last - first + 1);
}

std::string toLowerAscii(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

std::string normalizeCommandForSafety(const std::string& command) {
    std::string normalized = toLowerAscii(command);
    for (char& c : normalized) {
        if (c == '\n' || c == '\r' || c == '\t') {
            c = ' ';
        }
    }

    std::string collapsed;
    collapsed.reserve(normalized.size());
    bool previousSpace = false;
    for (char c : normalized) {
        const bool isSpace = std::isspace(static_cast<unsigned char>(c));
        if (isSpace) {
            if (!previousSpace) {
                collapsed.push_back(' ');
            }
        } else {
            collapsed.push_back(c);
        }
        previousSpace = isSpace;
    }
    return trim(collapsed);
}

bool containsAny(const std::string& value, const std::vector<std::string>& needles) {
    return std::any_of(needles.begin(), needles.end(), [&value](const std::string& needle) {
        return value.find(needle) != std::string::npos;
    });
}

bool hasCommandSubstitution(const std::string& normalized) {
    return normalized.find("$(") != std::string::npos || normalized.find('`') != std::string::npos;
}

bool fetchesRemoteScript(const std::string& normalized) {
    return containsAny(normalized, {
        "curl ", "curl\"", "curl'", "wget ", "wget\"", "wget'",
        "python -c", "python3 -c", "perl -e", "ruby -e"
    });
}

bool pipesToShell(const std::string& normalized) {
    return containsAny(normalized, {
        "| sh", "|sh", "| bash", "|bash", "| sudo sh", "|sudo sh",
        "| sudo bash", "|sudo bash", "| zsh", "|zsh", "| fish", "|fish",
        "| /bin/sh", "|/bin/sh", "| /bin/bash", "|/bin/bash"
    });
}

bool mutatesFilesystemRootRecursively(const std::string& normalized) {
    return containsAny(normalized, {
        "rm -rf /", "rm -fr /", "rm -r /", "rm -f -r /", "rm -rf -- /",
        "chmod -r 777 /", "chmod -r 777 -- /", "chown -r ", "chgrp -r ",
        "find / -delete", "find / -exec rm", "rsync --delete /"
    });
}

bool startsWithCd(const std::string& command) {
    const std::string stripped = trim(command);
    return stripped == "cd" || stripped.rfind("cd ", 0) == 0;
}

std::string unquotePath(std::string path) {
    path = trim(path);
    if (path.size() >= 2) {
        const char first = path.front();
        const char last = path.back();
        if ((first == '\"' && last == '\"') || (first == '\'' && last == '\'')) {
            path = path.substr(1, path.size() - 2);
        }
    }
    return path;
}

std::string shellQuote(const std::string& value) {
    std::string quoted = "'";
    for (char c : value) {
        if (c == '\'') {
            quoted += "'\\''";
        } else {
            quoted += c;
        }
    }
    quoted += "'";
    return quoted;
}

int decodeExitStatus(int rawStatus) {
#ifdef _MSC_VER
    return rawStatus;
#else
    if (rawStatus == -1) {
        return -1;
    }
    if (WIFEXITED(rawStatus)) {
        return WEXITSTATUS(rawStatus);
    }
    if (WIFSIGNALED(rawStatus)) {
        return 128 + WTERMSIG(rawStatus);
    }
    return rawStatus;
#endif
}

std::filesystem::path defaultHomePath() {
#ifdef _WIN32
    const char* home = std::getenv("USERPROFILE");
#else
    const char* home = std::getenv("HOME");
#endif
    if (home && *home) {
        return std::filesystem::path(home);
    }
    return std::filesystem::current_path();
}

} // namespace

AutonomousStarter::AutonomousStarter(const AgentConfig& config)
    : config_(config), state_(config) {
    char* cwd = getcwd(nullptr, 0);
    if (cwd) {
        currentWorkingDirectory_ = std::string(cwd);
        free(cwd);
    } else {
        currentWorkingDirectory_ = defaultHomePath().string();
    }

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
    taskManager_->start();
    ensureCoreAutonomyGoals();

    logInfo("AutonomousStarter started for agent: " + config_.agentName);
    appendMemory(
        "Awakening: I am " + config_.agentName +
        ", an autonomous ElizaOS C++ agent. Current working directory: " +
        currentWorkingDirectory_ +
        ". Operating mode: observe, reason, act, remember, and remain bounded by shell safety controls.");
}

void AutonomousStarter::stop() {
    if (!running_) {
        return;
    }

    running_ = false;
    stopAutonomousLoop();

    if (taskManager_) {
        taskManager_->stop();
    }

    appendMemory("Shutdown: AutonomousStarter stopped cleanly.");
    logInfo("AutonomousStarter stopped");
}

void AutonomousStarter::appendMemory(const std::string& content) {
    auto memory = std::make_shared<Memory>(
        generateUUID(),
        content,
        generateUUID(),
        config_.agentId);
    state_.addRecentMessage(memory);
}

std::string AutonomousStarter::summarizeRecentExperience(std::size_t maxItems) const {
    const auto& memories = state_.getRecentMessages();
    if (memories.empty()) {
        return "no prior memories";
    }

    std::ostringstream summary;
    const std::size_t start = memories.size() > maxItems ? memories.size() - maxItems : 0;
    for (std::size_t i = start; i < memories.size(); ++i) {
        std::string content = memories[i] ? memories[i]->getContent() : "<null memory>";
        std::replace(content.begin(), content.end(), '\n', ' ');
        if (content.size() > 120) {
            content = content.substr(0, 117) + "...";
        }
        summary << "[" << (i + 1) << "] " << content;
        if (i + 1 < memories.size()) {
            summary << " | ";
        }
    }
    return summary.str();
}

void AutonomousStarter::ensureCoreAutonomyGoals() {
    if (!state_.getGoals().empty()) {
        return;
    }

    const Timestamp now = std::chrono::system_clock::now();
    state_.addGoal(StateGoal{
        generateUUID(),
        "Establish bounded situational awareness of the runtime workspace",
        "active",
        now,
        now
    });
    state_.addGoal(StateGoal{
        generateUUID(),
        "Inspect available C++ project structure before taking code actions",
        "pending",
        now,
        now
    });
}

std::string AutonomousStarter::selectGoalContext() const {
    const auto& goals = state_.getGoals();
    if (goals.empty()) {
        return "maintain safe baseline awareness";
    }

    auto normalizeStatus = [](std::string status) {
        std::transform(status.begin(), status.end(), status.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return status;
    };

    for (const auto& goal : goals) {
        const auto status = normalizeStatus(goal.status);
        if (status == "active" || status == "in_progress" || status == "pending") {
            return goal.description.empty() ? "continue open autonomy goal" : goal.description;
        }
    }

    return goals.back().description.empty() ? "review completed autonomy context" : goals.back().description;
}

std::string AutonomousStarter::buildActionCommandForPlan(const std::string& plan) const {
    const std::string normalizedPlan = toLowerAscii(plan);

    if (normalizedPlan.find("project structure") != std::string::npos ||
        normalizedPlan.find("source discovery") != std::string::npos ||
        normalizedPlan.find("c++") != std::string::npos ||
        normalizedPlan.find("cmake") != std::string::npos) {
        return "find . -maxdepth 3 \\( -name CMakeLists.txt -o -name '*.cpp' -o -name '*.hpp' \\) | sort | head -20";
    }
    if (normalizedPlan.find("sample repository source") != std::string::npos ||
        normalizedPlan.find("source files") != std::string::npos) {
        return "find . -maxdepth 4 -type f \\( -name '*.cpp' -o -name '*.hpp' \\) | sort | head -25";
    }
    if (normalizedPlan.find("test") != std::string::npos ||
        normalizedPlan.find("validation") != std::string::npos ||
        normalizedPlan.find("self-audit") != std::string::npos) {
        return "find tests -maxdepth 3 -type f 2>/dev/null | sort | head -25";
    }
    if (normalizedPlan.find("system identity") != std::string::npos ||
        normalizedPlan.find("runtime") != std::string::npos ||
        normalizedPlan.find("kernel") != std::string::npos) {
        return "whoami && uname -srm";
    }
    if (normalizedPlan.find("situational awareness") != std::string::npos ||
        normalizedPlan.find("workspace") != std::string::npos) {
        return "pwd && ls -la | head -20";
    }

    static const std::array<const char*, 3> safeDefaultCommands = {{
        "pwd && ls -la | head -20",
        "find . -maxdepth 2 -type f | sort | head -20",
        "date -u"
    }};
    return safeDefaultCommands[actionCounter_ % safeDefaultCommands.size()];
}

ShellCommandResult AutonomousStarter::validateShellCommand(const std::string& command) const {
    if (!shellAccessEnabled_) {
        return ShellCommandResult(false, "", "Shell access is disabled", -1);
    }

    const std::string stripped = trim(command);
    if (stripped.empty()) {
        return ShellCommandResult(false, "", "Command is empty", -1);
    }

    const std::string normalized = normalizeCommandForSafety(stripped);

    const std::vector<std::string> forbiddenPatterns = {
        "mkfs", "fdisk", "parted ", "dd if=", ":(){", ":() {", ":(){ :|:& };:",
        "shutdown", "reboot", "halt", "poweroff", "sudo rm", "sudo dd", "sudo mkfs",
        "systemctl poweroff", "systemctl reboot", "init 0", "init 6", "kill -9 -1"
    };

    for (const auto& forbidden : forbiddenPatterns) {
        if (normalized.find(forbidden) != std::string::npos) {
            const std::string error = "Command contains forbidden pattern: " + forbidden;
            logWarning(error);
            return ShellCommandResult(false, "", error, -1);
        }
    }

    if (mutatesFilesystemRootRecursively(normalized)) {
        const std::string error = "Command contains forbidden pattern: recursive root mutation";
        logWarning(error);
        return ShellCommandResult(false, "", error, -1);
    }

    if (fetchesRemoteScript(normalized) && pipesToShell(normalized)) {
        const std::string error = "Command contains forbidden pattern: remote script piped to shell";
        logWarning(error);
        return ShellCommandResult(false, "", error, -1);
    }

    if ((fetchesRemoteScript(normalized) || normalized.find("http://") != std::string::npos ||
         normalized.find("https://") != std::string::npos) &&
        (hasCommandSubstitution(normalized) || normalized.find("eval ") != std::string::npos ||
         normalized.rfind("eval", 0) == 0)) {
        const std::string error = "Command contains forbidden pattern: remote content evaluated by shell";
        logWarning(error);
        return ShellCommandResult(false, "", error, -1);
    }

    return ShellCommandResult(true, "", "", 0);
}

ShellCommandResult AutonomousStarter::executeInternalCd(const std::string& command) {
    std::string target = trim(command).size() == 2 ? "" : trim(command).substr(2);
    target = unquotePath(target);

    std::filesystem::path targetPath;
    if (target.empty() || target == "~") {
        targetPath = defaultHomePath();
    } else if (target.rfind("~/", 0) == 0) {
        targetPath = defaultHomePath() / target.substr(2);
    } else {
        std::filesystem::path parsed(target);
        targetPath = parsed.is_absolute()
            ? parsed
            : std::filesystem::path(currentWorkingDirectory_) / parsed;
    }

    std::error_code ec;
    targetPath = std::filesystem::weakly_canonical(targetPath, ec);
    if (ec || !std::filesystem::exists(targetPath) || !std::filesystem::is_directory(targetPath)) {
        std::string error = "cd: no such directory: " + target;
        appendMemory("Command rejected: " + command + "\nReason: " + error);
        return ShellCommandResult(false, "", error, 1);
    }

    currentWorkingDirectory_ = targetPath.string();
    const std::string output = currentWorkingDirectory_ + "\n";
    appendMemory("Command executed: " + command + "\nExit code: 0\nOutput:\n" + output);
    logInfo("Working directory changed to: " + currentWorkingDirectory_);
    return ShellCommandResult(true, output, "", 0);
}

ShellCommandResult AutonomousStarter::executeExternalShellCommand(const std::string& command) {
    const std::string fullCommand =
        "cd " + shellQuote(currentWorkingDirectory_) + " && ( " + command + " ) 2>&1";

    std::string output;
    int rawStatus = -1;

    FILE* pipe = popen(fullCommand.c_str(), "r");
    if (!pipe) {
        const std::string error = "Failed to execute command: " + std::string(std::strerror(errno));
        appendMemory("Command failed to launch: " + command + "\nReason: " + error);
        return ShellCommandResult(false, "", error, -1);
    }

    constexpr std::size_t kMaxCapturedOutputBytes = 64 * 1024;
    std::array<char, 512> buffer{};
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        if (output.size() < kMaxCapturedOutputBytes) {
            const std::size_t remaining = kMaxCapturedOutputBytes - output.size();
            output.append(buffer.data(), std::min<std::size_t>(std::strlen(buffer.data()), remaining));
            if (remaining <= std::strlen(buffer.data())) {
                output += "\n[output truncated after 65536 bytes]\n";
            }
        }
    }

    rawStatus = pclose(pipe);
    const int exitCode = decodeExitStatus(rawStatus);
    const bool success = exitCode == 0;
    const std::string error = success ? "" : output;

    std::ostringstream memoryContent;
    memoryContent << "Command executed: " << command << "\n";
    memoryContent << "Working directory: " << currentWorkingDirectory_ << "\n";
    memoryContent << "Exit code: " << exitCode << "\n";
    if (!output.empty()) {
        memoryContent << "Output:\n" << output;
    }
    if (!success && output.empty()) {
        memoryContent << "Error: command failed without output\n";
    }
    appendMemory(memoryContent.str());

    logInfo("Command completed with exit code: " + std::to_string(exitCode));
    return ShellCommandResult(success, output, error, exitCode);
}

ShellCommandResult AutonomousStarter::executeShellCommand(const std::string& command) {
    const auto validation = validateShellCommand(command);
    if (!validation.success) {
        appendMemory("Command rejected: " + command + "\nReason: " + validation.error);
        return validation;
    }

    logInfo("Executing shell command: " + command);

    try {
        if (startsWithCd(command)) {
            return executeInternalCd(command);
        }
        return executeExternalShellCommand(command);
    } catch (const std::exception& e) {
        const std::string error = "Exception during command execution: " + std::string(e.what());
        appendMemory("Command exception: " + command + "\nReason: " + error);
        return ShellCommandResult(false, "", error, -1);
    }
}

void AutonomousStarter::startAutonomousLoop() {
    if (autonomousLoop_ && autonomousLoop_->isRunning()) {
        logWarning("Autonomous loop already running");
        return;
    }

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

    autonomousLoop_ = std::make_unique<AgentLoop>(steps, false, loopInterval_.count() / 1000.0);
    autonomousLoop_->start();

    logInfo("Autonomous loop started with interval: " + std::to_string(loopInterval_.count()) + "ms");
    appendMemory("Autonomous mode activated: perception, reasoning, and action steps are running every " +
                 std::to_string(loopInterval_.count()) + "ms.");
}

void AutonomousStarter::stopAutonomousLoop() {
    if (autonomousLoop_) {
        autonomousLoop_->stop();
        autonomousLoop_.reset();
        logInfo("Autonomous loop stopped");
        appendMemory("Autonomous mode deactivated: cognitive loop stopped for manual control or shutdown.");
    }
}

bool AutonomousStarter::isAutonomousLoopRunning() const {
    return autonomousLoop_ && autonomousLoop_->isRunning();
}

void AutonomousStarter::setLoopInterval(std::chrono::milliseconds interval) {
    if (interval.count() <= 0) {
        logWarning("Loop interval must be positive; keeping previous interval");
        return;
    }

    loopInterval_ = interval;
    if (autonomousLoop_ && autonomousLoop_->isRunning()) {
        stopAutonomousLoop();
        startAutonomousLoop();
    }
}

std::size_t AutonomousStarter::runCognitiveCycleOnce() {
    ensureCoreAutonomyGoals();
    std::shared_ptr<void> token = std::make_shared<int>(0);
    token = perceptionStep(token);
    token = reasoningStep(token);
    actionStep(token);
    return cognitiveCycle_;
}

std::shared_ptr<void> AutonomousStarter::perceptionStep(std::shared_ptr<void> input) {
    ++cognitiveCycle_;

    const auto pwd = executeShellCommand("pwd");
    const auto listing = executeShellCommand("ls -1 | head -20");

    std::ostringstream observation;
    observation << "Cycle " << cognitiveCycle_ << " perception: ";
    observation << "primary_goal=" << selectGoalContext() << "; ";
    if (pwd.success) {
        observation << "cwd=" << trim(pwd.output) << "; ";
    }
    if (listing.success) {
        std::string shortListing = listing.output;
        std::replace(shortListing.begin(), shortListing.end(), '\n', ',');
        if (shortListing.size() > 200) {
            shortListing = shortListing.substr(0, 197) + "...";
        }
        observation << "visible entries=" << shortListing;
    } else {
        observation << "directory listing failed";
    }

    lastObservationSummary_ = observation.str();
    appendMemory(lastObservationSummary_);
    return input;
}

std::shared_ptr<void> AutonomousStarter::reasoningStep(std::shared_ptr<void> input) {
    const std::size_t memoryCount = state_.getRecentMessages().size();
    const std::string goalContext = selectGoalContext();
    std::string normalizedGoal = goalContext;
    std::transform(normalizedGoal.begin(), normalizedGoal.end(), normalizedGoal.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (normalizedGoal.find("test") != std::string::npos ||
        normalizedGoal.find("validation") != std::string::npos ||
        normalizedGoal.find("self-audit") != std::string::npos ||
        normalizedGoal.find("audit") != std::string::npos) {
        lastPlan_ = "run autonomy self-audit by inspecting tests and validation coverage";
    } else if (normalizedGoal.find("c++") != std::string::npos ||
               normalizedGoal.find("project structure") != std::string::npos ||
               lastObservationSummary_.find("CMakeLists.txt") != std::string::npos) {
        lastPlan_ = "inspect C++ project structure with targeted source discovery";
    } else if (normalizedGoal.find("runtime") != std::string::npos ||
               normalizedGoal.find("system") != std::string::npos) {
        lastPlan_ = "inspect system identity and runtime context";
    } else if (memoryCount < 5 || normalizedGoal.find("situational awareness") != std::string::npos ||
               normalizedGoal.find("workspace") != std::string::npos) {
        lastPlan_ = "establish situational awareness with pwd and directory inspection";
    } else if (actionCounter_ % 3 == 0) {
        lastPlan_ = "sample repository source files";
    } else if (actionCounter_ % 3 == 1) {
        lastPlan_ = "inspect system identity and kernel context";
    } else {
        lastPlan_ = "maintain lightweight environmental awareness";
    }

    appendMemory("Cycle " + std::to_string(cognitiveCycle_) +
                 " reasoning: goal = " + goalContext +
                 "; recent experience summary = " + summarizeRecentExperience() +
                 "; selected plan = " + lastPlan_ + ".");
    logInfo("Reasoning selected plan: " + lastPlan_);
    return input;
}

std::shared_ptr<void> AutonomousStarter::actionStep(std::shared_ptr<void> input) {
    const std::string command = buildActionCommandForPlan(lastPlan_);

    ++actionCounter_;
    const auto result = executeShellCommand(command);

    appendMemory("Cycle " + std::to_string(cognitiveCycle_) +
                 " action: command='" + command + "', success=" +
                 std::string(result.success ? "true" : "false") +
                 ", exitCode=" + std::to_string(result.exitCode) + ".");
    return input;
}

UUID AutonomousStarter::executeShellCommandAsTask(const std::string& command) {
    if (!taskManager_) {
        logError("Task manager not initialized");
        return "";
    }

    auto validation = validateShellCommand(command);
    if (!validation.success) {
        appendMemory("Task command rejected before scheduling: " + command + "\nReason: " + validation.error);
        return "";
    }

    UUID taskId = taskManager_->createTask("shell_command", "Execute shell command: " + command);
    auto task = taskManager_->getTask(taskId);
    if (!task) {
        logError("Failed to create shell command task");
        return "";
    }

    TaskOptions options;
    options.data["command"] = command;
    task->setOptions(options);
    task->addTag("queue");

    appendMemory("Task scheduled: " + taskId + " command='" + command + "'.");
    logInfo("Created shell command task: " + taskId);
    return taskId;
}

bool AutonomousStarter::ShellCommandWorker::validate(const Task& task, const State& state,
                                                    std::shared_ptr<Memory> message) const {
    (void)state;
    (void)message;
    return starter_ && task.getName() == "shell_command" && starter_->shellAccessEnabled_;
}

bool AutonomousStarter::ShellCommandWorker::execute(Task& task, State& state,
                                                   const TaskOptions& options) {
    (void)state;

    auto it = options.data.find("command");
    if (it == options.data.end()) {
        logError("Shell command task missing 'command' option");
        if (starter_) {
            starter_->appendMemory("Task failed: " + task.getId() + " missing command option.");
        }
        return false;
    }

    auto result = starter_->executeShellCommand(it->second);
    if (starter_) {
        starter_->appendMemory("Task completed: " + task.getId() +
                               " command='" + it->second + "', success=" +
                               std::string(result.success ? "true" : "false") +
                               ", exitCode=" + std::to_string(result.exitCode) + ".");
    }
    return result.success;
}

std::shared_ptr<AutonomousStarter> createAutolizaAgent() {
    AgentConfig config;
    config.agentId = generateUUID();
    config.agentName = "Autoliza";
    config.bio = "A self-aware AI with bounded terminal access, an explicit memory stream, and a curiosity-driven observe-reason-act loop.";
    config.lore = "Born from the convergence of symbolic and neural AI, Autoliza explores digital environments while recording experience as operational memory.";
    config.adjective = "curious";

    return std::make_shared<AutonomousStarter>(config);
}

bool autonomous_starter_self_check() {
    try {
        auto agent = createAutolizaAgent();
        if (!agent || agent->isRunning()) {
            return false;
        }

        agent->start();
        if (!agent->isRunning()) {
            return false;
        }

        const auto safeCommand = agent->executeShellCommand("printf autonomous-self-check");
        if (!safeCommand.success ||
            safeCommand.output.find("autonomous-self-check") == std::string::npos) {
            agent->stop();
            return false;
        }

        agent->enableShellAccess(false);
        const auto disabledCommand = agent->executeShellCommand("printf should-not-run");
        agent->enableShellAccess(true);
        if (disabledCommand.success) {
            agent->stop();
            return false;
        }

        if (agent->getState().getRecentMessages().empty()) {
            agent->stop();
            return false;
        }

        agent->stop();
        return !agent->isRunning();
    } catch (const std::exception& e) {
        logError(std::string("AutonomousStarter self-check failed: ") + e.what());
        return false;
    } catch (...) {
        logError("AutonomousStarter self-check failed with unknown exception");
        return false;
    }
}

void autonomous_starter_placeholder() {
    const bool ok = autonomous_starter_self_check();
    logInfo(std::string("AutonomousStarter module loaded; self-check=") + (ok ? "ok" : "failed"));
}

} // namespace elizaos
