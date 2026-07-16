#include "elizaos/autonomous_starter.hpp"
#include "elizaos/agentlogger.hpp"
#include "elizaos/attention.hpp"

#include <cmath>

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
#include <unordered_map>

#ifdef _WIN32
#include <direct.h>  // _getcwd on Windows
#define getcwd _getcwd
#ifndef _MSC_VER
// MinGW has popen/pclose without underscore prefix.
#else
#define popen _popen
#define pclose _pclose
#endif
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
#ifdef _WIN32
    // On Windows, pclose() returns the process exit code directly.
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

    // Prefer the currently pursued goal so perception, reasoning, and action all
    // converge on the same intent within a cycle.
    if (!activeGoalId_.empty()) {
        for (const auto& goal : goals) {
            if (goal.id == activeGoalId_) {
                const auto status = normalizeStatus(goal.status);
                if (status == "active" || status == "in_progress" || status == "pending") {
                    return goal.description.empty() ? "continue open autonomy goal" : goal.description;
                }
            }
        }
    }

    // Priority order: active > in_progress > pending.
    for (const char* wanted : {"active", "in_progress", "pending"}) {
        for (const auto& goal : goals) {
            if (normalizeStatus(goal.status) == wanted) {
                return goal.description.empty() ? "continue open autonomy goal" : goal.description;
            }
        }
    }

    return goals.back().description.empty() ? "review completed autonomy context" : goals.back().description;
}

const StateGoal* AutonomousStarter::selectActiveGoal() {
    const auto& goals = state_.getGoals();
    auto normalizeStatus = [](std::string status) {
        std::transform(status.begin(), status.end(), status.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return status;
    };

    // Promote the first pending goal to active when no active goal exists, so
    // the agent always has exactly one goal in flight (single-tasking autonomy).
    const StateGoal* active = nullptr;
    const StateGoal* firstPending = nullptr;
    for (const auto& goal : goals) {
        const auto status = normalizeStatus(goal.status);
        if ((status == "active" || status == "in_progress") && active == nullptr) {
            active = &goal;
        } else if (status == "pending" && firstPending == nullptr) {
            firstPending = &goal;
        }
    }

    if (active == nullptr && firstPending != nullptr) {
        state_.updateGoalStatus(firstPending->id, "active");
        // Re-resolve the pointer after mutation (vector storage is stable here,
        // but re-scan defensively in case of future reallocation semantics).
        for (const auto& goal : state_.getGoals()) {
            if (goal.id == firstPending->id) {
                active = &goal;
                break;
            }
        }
    }

    if (active != nullptr) {
        activeGoalId_ = active->id;
    } else {
        activeGoalId_ = "";
    }
    return active;
}

bool AutonomousStarter::planSatisfiesGoal(const StateGoal& goal,
                                          const std::string& plan,
                                          const ShellCommandResult& result) const {
    // A goal is only satisfied by a successful action whose plan is topically
    // aligned with the goal. Failed commands never satisfy a goal, so the agent
    // will keep working (or escalate via the stagnation guard) until it produces
    // real evidence. This is the behavioral consequence that makes the goal
    // center "living" rather than a decorative status field.
    if (!result.success) {
        return false;
    }

    const std::string goalText = toLowerAscii(goal.description);
    const std::string planText = toLowerAscii(plan);

    auto mentions = [](const std::string& text, std::initializer_list<const char*> needles) {
        for (const char* needle : needles) {
            if (text.find(needle) != std::string::npos) {
                return true;
            }
        }
        return false;
    };

    // Situational-awareness goals are satisfied by any successful awareness plan.
    if (mentions(goalText, {"situational awareness", "workspace", "awareness"})) {
        return mentions(planText, {"situational awareness", "workspace", "awareness", "pwd", "directory"});
    }
    // Project-structure / source-inspection goals.
    if (mentions(goalText, {"project structure", "c++", "source", "cmake", "code actions"})) {
        return mentions(planText, {"project structure", "source", "c++", "cmake", "repository source"});
    }
    // Test/validation goals.
    if (mentions(goalText, {"test", "validation", "self-audit"})) {
        return mentions(planText, {"test", "validation", "self-audit"});
    }
    // Runtime/system goals.
    if (mentions(goalText, {"runtime", "system", "kernel", "identity"})) {
        return mentions(planText, {"runtime", "system", "kernel", "identity"});
    }

    // Generic exploration goals are satisfied by any successful action.
    return true;
}

bool AutonomousStarter::planSatisfiesGoalTopic(const std::string& normalizedGoal,
                                               const std::string& plan) const {
    const std::string goalText = normalizedGoal;  // caller passes lower-cased goal
    const std::string planText = toLowerAscii(plan);

    auto mentions = [](const std::string& text, std::initializer_list<const char*> needles) {
        for (const char* needle : needles) {
            if (text.find(needle) != std::string::npos) {
                return true;
            }
        }
        return false;
    };

    // Mirror the topic mapping used by planSatisfiesGoal(), minus the success
    // requirement. This keeps the stagnation-guard exemption in lock-step with
    // the goal-completion alignment logic so the two never disagree about whether
    // a plan serves a goal.
    if (mentions(goalText, {"situational awareness", "workspace", "awareness"})) {
        return mentions(planText, {"situational awareness", "workspace", "awareness", "pwd", "directory"});
    }
    if (mentions(goalText, {"project structure", "c++", "source", "cmake", "code actions"})) {
        return mentions(planText, {"project structure", "source", "c++", "cmake", "repository source"});
    }
    if (mentions(goalText, {"test", "validation", "self-audit", "audit"})) {
        return mentions(planText, {"test", "validation", "self-audit"});
    }
    if (mentions(goalText, {"runtime", "system", "kernel", "identity"})) {
        return mentions(planText, {"runtime", "system", "kernel", "identity"});
    }
    // For generic goals no specific plan is privileged, so repetition of any plan
    // is treated as aimless and remains subject to the stagnation guard.
    return false;
}

void AutonomousStarter::seedAdaptiveGoal() {
    // Deterministic exploratory rotation: when the agent has satisfied every
    // standing goal it widens understanding across distinct themes so it never
    // loops a single plan forever (the anti-dead-end / anti-stagnation drive).
    const Timestamp now = std::chrono::system_clock::now();
    static const std::array<const char*, 7> rotations = {{
        "Sample repository source files to deepen project understanding",
        "Self-audit test and validation surfaces for autonomy health",
        "Inspect system identity and runtime context for situational grounding",
        "Evaluate memory coherence and consolidation effectiveness",
        "Assess goal completion velocity and plan diversity metrics",
        "Survey available tooling and runtime capabilities for expansion",
        "Verify shell safety boundaries and command validation integrity"
    }};

    // Intent continuity (one continuation per theme switch): an agent that just
    // finished a validation/self-audit objective should run ONE more self-audit
    // pass before broadening into exploration, rather than abruptly topic-
    // hopping mid-drive. This keeps a validation-seeded agent coherently anchored
    // to verifying its own health across the immediate next cycle, then resumes
    // the exploratory rotation so it still never dead-ends or loops. The
    // continuation is given a UNIQUE description (a pass counter) so completion
    // by-description can never ambiguously retire several identically-named
    // goals at once -- which would otherwise let the open-goal count fall to
    // zero and break the never-dead-end invariant.
    const std::string lastTheme = toLowerAscii(lastCompletedGoalDescription_);
    const bool lastWasSelfAudit =
        lastTheme.find("self-audit") != std::string::npos ||
        lastTheme.find("validation") != std::string::npos ||
        (lastTheme.find("audit") != std::string::npos &&
         lastTheme.find("autonomy") != std::string::npos);

    // --- Intent continuity implementation ---
    // If the last completed goal was a self-audit theme and we haven't yet
    // issued the bounded continuation sprint, seed ONE continuation pass
    // before broadening into exploratory rotation. This keeps the agent
    // coherently anchored to verifying its own health before topic-hopping.
    std::string description;
    if (lastWasSelfAudit && !selfAuditContinued_ && selfAuditSprintRemaining_ == 0) {
        // Begin a bounded self-audit verification sprint (1 continuation pass).
        selfAuditSprintRemaining_ = 1;
        selfAuditContinued_ = true;
    }

    if (selfAuditSprintRemaining_ > 0) {
        // Issue a uniquely-named continuation pass so completion-by-description
        // can never ambiguously retire multiple identically-named goals.
        ++selfAuditPass_;
        --selfAuditSprintRemaining_;
        description = "Self-audit continuation pass #" + std::to_string(selfAuditPass_) +
                      ": verify autonomy health metrics and goal lifecycle integrity";
    } else {
        // Normal exploratory rotation: deterministic round-robin through themes.
        selfAuditContinued_ = false;  // Reset latch for next self-audit cycle.
        description = rotations[adaptiveGoalCounter_ % rotations.size()];
        ++adaptiveGoalCounter_;
    }

    state_.addGoal(StateGoal{
        generateUUID(),
        description,
        "pending",
        now,
        now
    });
    // Emit a novelty stimulus to the endocrine system when seeding new
    // exploration goals. This reinforces the Exploration cognitive mode,
    // keeping the agent's curiosity drive active and preventing premature
    // convergence into Exploitation when all goals are satisfied.
    endocrine_.submitStimulus(Stimulus("novelty_detected", 0.5));
    appendMemory("Adaptive goal seeded: " + description +
                 " (all prior goals satisfied; autonomy continues exploring).");
}

void AutonomousStarter::evaluateGoalProgress(const std::string& plan,
                                             const std::string& command,
                                             const ShellCommandResult& result) {
    (void)command;

    // Mark the active goal completed when the successful action provides aligned
    // evidence, then promote the next pending goal.
    if (!activeGoalId_.empty()) {
        const StateGoal* activeGoal = nullptr;
        for (const auto& goal : state_.getGoals()) {
            if (goal.id == activeGoalId_) {
                activeGoal = &goal;
                break;
            }
        }
        // Convergence gate: a single aligned success is only a probe, not proof
        // that the goal is achieved. Require the serving plan to have demonstrated
        // *reliable* success (>= 2 attempts and a success ratio >= 0.5) before
        // marking the goal completed. This keeps a sustained goal live across the
        // first cycle or two -- preserving plan continuity for a single dominant
        // goal -- while still driving every goal to closed-loop completion within
        // a handful of cycles. The threshold mirrors advanceGoalLifecycle() so the
        // two evidence-gated paths agree on what "reliable" means.
        // A goal is "achieved" only after its serving plan has produced reliable,
        // repeated evidence: at least three successful aligned attempts with a
        // success ratio >= 0.5. Three (rather than two) gives a lone dominant goal
        // a stable multi-cycle pursuit window before it converges, while a set of
        // goals still drives every member to completion well within a normal run.
        const bool planIsReliable =
            planStats_[plan].attempts >= 3 && getPlanSuccessRatio(plan) >= 0.5;
        if (activeGoal != nullptr && planSatisfiesGoal(*activeGoal, plan, result) &&
            planIsReliable) {
            const std::string completedDescription = activeGoal->description;
            state_.updateGoalStatus(activeGoalId_, "completed");
            appendMemory("Goal completed: " + completedDescription +
                         " (satisfied by reliable plan='" + plan + "').");
            lastCompletedGoalDescription_ = completedDescription;
            goalCompletedThisCycleById_ = true;
            activeGoalId_ = "";
        }
    }

    // If every goal is now complete, seed a fresh adaptive goal so the agent
    // never dead-ends into a no-open-goal idle loop.
    if (getOpenGoalCount() == 0) {
        seedAdaptiveGoal();
    }
}

std::size_t AutonomousStarter::getOpenGoalCount() const {
    std::size_t open = 0;
    for (const auto& goal : state_.getGoals()) {
        std::string status = toLowerAscii(goal.status);
        if (status == "active" || status == "in_progress" || status == "pending") {
            ++open;
        }
    }
    return open;
}

std::size_t AutonomousStarter::getCompletedGoalCount() const {
    std::size_t completed = 0;
    for (const auto& goal : state_.getGoals()) {
        if (toLowerAscii(goal.status) == "completed") {
            ++completed;
        }
    }
    return completed;
}

std::string AutonomousStarter::scoreAndSelectGoal() const {
    const auto& goals = state_.getGoals();
    if (goals.empty()) {
        return selectGoalContext();
    }

    auto normalizeStatus = [](std::string status) {
        std::transform(status.begin(), status.end(), status.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return status;
    };

    // Score each open goal through the repository's AttentionAllocator. Open
    // goals (active/in_progress/pending) receive higher urgency; freshly created
    // goals (small index) receive higher novelty; long-standing goals accrue
    // importance. This replaces ad-hoc string matching with the engine's own
    // ECAN-style composite attention scoring.
    std::vector<UUID> candidateIds;
    candidateIds.reserve(goals.size());
    const Timestamp now = std::chrono::system_clock::now();

    for (std::size_t i = 0; i < goals.size(); ++i) {
        const auto& goal = goals[i];
        const std::string status = normalizeStatus(goal.status);
        const bool isOpen = (status == "active" || status == "in_progress" || status == "pending");
        if (!isOpen) {
            continue;
        }

        AttentionValue av;
        // Urgency: active goals outrank pending goals.
        av.urgency = (status == "active" || status == "in_progress") ? 0.85 : 0.55;
        // Importance: older goals (earlier in the list / earlier createdAt) carry
        // more long-term importance because they anchor the agent's mission.
        const double ageSeconds =
            std::chrono::duration_cast<std::chrono::seconds>(now - goal.createdAt).count();
        av.importance = std::min(1.0, 0.4 + (ageSeconds / 3600.0) * 0.1);
        // Novelty: goals not yet attempted (no matching memory) are novel.
        const std::string needle = goal.description.substr(0, std::min<std::size_t>(goal.description.size(), 24));
        bool seen = false;
        for (const auto& mem : state_.getRecentMessages()) {
            if (mem && !needle.empty() && mem->getContent().find(needle) != std::string::npos) {
                seen = true;
                break;
            }
        }
        av.novelty = seen ? 0.2 : 0.8;
        // Activation: bias by current competence so a struggling agent focuses on
        // the highest-urgency goal rather than exploring novel ones.
        av.activation = competenceSignal_;

        goalAttention_.updateAttentionValue(goal.id, av);
        candidateIds.push_back(goal.id);
    }

    if (candidateIds.empty()) {
        return selectGoalContext();
    }

    const auto prioritized = goalAttention_.prioritizeElements(candidateIds);
    if (prioritized.empty()) {
        return selectGoalContext();
    }

    const UUID topId = prioritized.front().elementId;
    for (const auto& goal : goals) {
        if (goal.id == topId) {
            return goal.description.empty() ? selectGoalContext() : goal.description;
        }
    }
    return selectGoalContext();
}

std::string AutonomousStarter::getAttentionPrioritizedGoal() const {
    return scoreAndSelectGoal();
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
        }),
        LoopStep([this](std::shared_ptr<void> input) -> std::shared_ptr<void> {
            return reflectionStep(input);
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
    token = actionStep(token);
    reflectionStep(token);

    // Never-dead-end invariant (post-cycle safety net): a cognitive cycle must
    // never terminate with zero open goals -- an agent with no open goal has no
    // drive and is effectively cognitively dead. Goal completion has two
    // evidence-gated paths: evaluateGoalProgress() (action phase, completes the
    // active goal and reseeds inline) and advanceGoalLifecycle() (reflection
    // phase, completes the focused goal). Only the former reseeds, so when the
    // reflection path retires the last open goal the cycle would otherwise end
    // dead-ended. We reconcile here, after the cycle has fully settled, so a
    // freshly-seeded exploratory goal never swaps a dominant goal's objective
    // mid-window -- it only fires once no open goal remains at all.
    if (getOpenGoalCount() == 0) {
        seedAdaptiveGoal();
    }
    return cognitiveCycle_;
}

double AutonomousStarter::getPlanSuccessRatio(const std::string& plan) const {
    auto it = planStats_.find(plan);
    if (it == planStats_.end() || it->second.attempts == 0) {
        return 0.0;
    }
    return static_cast<double>(it->second.successes) /
           static_cast<double>(it->second.attempts);
}

double AutonomousStarter::planBias(const std::string& plan) const {
    // Optimistic prior: unseen plans start neutral so the agent still explores,
    // while plans with a track record are biased by their measured success rate.
    auto it = planStats_.find(plan);
    if (it == planStats_.end() || it->second.attempts == 0) {
        return 0.5;
    }
    return static_cast<double>(it->second.successes) /
           static_cast<double>(it->second.attempts);
}

void AutonomousStarter::recordPlanOutcome(const std::string& plan, bool success) {
    if (plan.empty()) {
        return;
    }
    auto& stats = planStats_[plan];
    ++stats.attempts;
    if (success) {
        ++stats.successes;
    }
}

void AutonomousStarter::refreshGoalAttention() {
    const auto& goals = state_.getGoals();
    for (const auto& goal : goals) {
        std::string status = goal.status;
        std::transform(status.begin(), status.end(), status.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        const bool open = (status == "active" || status == "in_progress" ||
                           status == "pending");

        AttentionValue av = goalAttention_.hasAttentionValue(goal.id)
                                ? goalAttention_.getAttentionValue(goal.id)
                                : AttentionValue{};
        // Importance: active goals outrank pending; completed goals fade out.
        if (status == "active" || status == "in_progress") {
            av.importance = 0.9;
            av.urgency = 0.7;
        } else if (status == "pending") {
            av.importance = 0.6;
            av.urgency = 0.5;
        } else {
            av.importance = 0.1;
            av.urgency = 0.1;
        }
        // Novelty rewards goals the agent has not focused on recently. This is
        // an exploration pressure: it nudges attention toward goals the agent
        // has not yet served.
        av.novelty = (goal.id == focusedGoalId_) ? std::max(0.0, av.novelty * 0.6)
                                                 : std::min(1.0, av.novelty + 0.3);
        av.activation = open ? std::min(1.0, av.activation + 0.2)
                             : std::max(0.0, av.activation * 0.5);
        // Focus-retention (hysteresis): the currently-focused goal, while it is
        // still open, gets its activation pinned high. The composite-score
        // novelty term (weight 0.2) swings by ~0.06 per cycle and would
        // otherwise flip the winner between equally-important co-active goals
        // every single cycle, causing the agent to thrash between objectives and
        // never converge on the one it is pursuing. Pinning activation (weight
        // 0.1) gives the incumbent focus a +0.1 composite edge that outweighs
        // the novelty swing, so attention commits to a goal until it completes
        // or its importance/urgency drops (e.g. it becomes blocked) -- a stable
        // attention economy rather than an oscillating one.
        if (open && goal.id == focusedGoalId_) {
            av.activation = 1.0;
        }
        goalAttention_.updateAttentionValue(goal.id, av);
    }

    // Thematic continuity (activation spreading): link goals that share a
    // dominant theme word so the focused goal leaks a controlled amount of
    // activation into its thematic neighbours. When the incumbent completes,
    // the successor chosen by the attention economy is then biased toward a
    // RELATED objective instead of an arbitrary co-active one, preserving
    // train-of-thought across goal transitions. The spreading rate is kept
    // deliberately low (0.15) and the threshold high enough (0.05) that
    // background goals cannot echo activation among themselves; only the
    // pinned focus (activation 1.0) meaningfully spreads. This relies on the
    // completed AttentionAllocator::setSpreadingParameters, which now tunes
    // the LIVE network instead of being a dead-store stub.
    goalAttention_.setSpreadingParameters(0.15, 0.05);
    auto themeOf = [](const std::string& description) -> std::string {
        // Dominant theme = longest word of 5+ chars (cheap, deterministic).
        std::string best;
        std::string word;
        for (char c : description + " ") {
            if (std::isalnum(static_cast<unsigned char>(c))) {
                word += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            } else {
                if (word.size() >= 5 && word.size() > best.size()) {
                    best = word;
                }
                word.clear();
            }
        }
        return best;
    };
    for (std::size_t i = 0; i < goals.size(); ++i) {
        const std::string themeA = themeOf(goals[i].description);
        if (themeA.empty()) continue;
        for (std::size_t j = i + 1; j < goals.size(); ++j) {
            if (themeOf(goals[j].description) == themeA) {
                goalAttention_.addAttentionLink(goals[i].id, goals[j].id, 1.0);
                goalAttention_.addAttentionLink(goals[j].id, goals[i].id, 1.0);
            }
        }
    }
    goalAttention_.spreadActivation(1);
    // Re-pin the focused goal after spreading: spreading decays the source's
    // activation by (1 - rate), which would erode the hysteresis edge that
    // keeps the incumbent focus stable (the exact thrash the pin prevents).
    if (!focusedGoalId_.empty() && goalAttention_.hasAttentionValue(focusedGoalId_)) {
        AttentionValue pinned = goalAttention_.getAttentionValue(focusedGoalId_);
        pinned.activation = 1.0;
        goalAttention_.updateAttentionValue(focusedGoalId_, pinned);
    }
}

const StateGoal* AutonomousStarter::selectFocusGoal() {
    const auto& goals = state_.getGoals();
    if (goals.empty()) {
        focusedGoalId_.clear();
        return nullptr;
    }
    refreshGoalAttention();

    // Single-intent coherence: perception (selectActiveGoal) has already promoted
    // exactly one in-flight goal into activeGoalId_. Reasoning MUST plan for that
    // same goal -- otherwise perception's primary_goal context and the reasoning
    // plan describe two different objectives in one cycle, and the closed loop
    // cannot converge. Previously selectFocusGoal independently picked the
    // highest attention-scoring open goal, which (with several co-active goals)
    // routinely diverged from activeGoalId_ and made the plan thrash between
    // objectives. We therefore honor the in-flight active goal as the focus
    // whenever it is still open; the attention economy still governs which
    // PENDING goal selectActiveGoal promotes next once the current one is done.
    auto isOpen = [](std::string status) {
        std::transform(status.begin(), status.end(), status.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return status == "active" || status == "in_progress" || status == "pending";
    };
    if (!activeGoalId_.empty()) {
        for (const auto& goal : goals) {
            if (goal.id == activeGoalId_ && isOpen(goal.status)) {
                focusedGoalId_ = goal.id;
                return &goal;
            }
        }
    }

    // No in-flight active goal (transient between completion and next promotion):
    // fall back to the attention economy to choose the most important/urgent/
    // novel open goal as the next focus.
    const StateGoal* best = nullptr;
    double bestScore = -1.0;
    for (const auto& goal : goals) {
        if (!isOpen(goal.status)) {
            continue;
        }
        const double score =
            goalAttention_.getAttentionValue(goal.id).getCompositeScore();
        if (score > bestScore) {
            bestScore = score;
            best = &goal;
        }
    }
    if (!best) {
        best = &goals.back();  // all goals completed: keep most recent context
    }
    focusedGoalId_ = best->id;
    return best;
}

void AutonomousStarter::advanceGoalLifecycle(const std::string& plan, bool actionSucceeded) {
    if (focusedGoalId_.empty() || !actionSucceeded) {
        return;
    }
    // Find the focused goal's current status.
    std::string currentStatus;
    for (const auto& goal : state_.getGoals()) {
        if (goal.id == focusedGoalId_) {
            currentStatus = goal.status;
            break;
        }
    }
    std::string lower = currentStatus;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    // pending -> active on first successful action toward the goal; active ->
    // completed once the agent has accumulated enough successful experience on
    // its plan for that goal.
    if (lower == "pending") {
        state_.updateGoalStatus(focusedGoalId_, "active");
        appendMemory("Goal lifecycle: goal " + focusedGoalId_ +
                     " advanced pending -> active after successful plan '" + plan + "'.");
    } else if (lower == "active" || lower == "in_progress") {
        if (getPlanSuccessRatio(plan) >= 0.5 &&
            planStats_[plan].attempts >= 3) {
            // Capture the description before mutation so the completion evidence
            // token matches the one evaluateGoalProgress emits (single vocabulary
            // for supervisors/tests scanning memory for "Goal completed:").
            std::string completedDescription;
            for (const auto& goal : state_.getGoals()) {
                if (goal.id == focusedGoalId_) {
                    completedDescription = goal.description;
                    break;
                }
            }
            state_.updateGoalStatus(focusedGoalId_, "completed");
            appendMemory("Goal lifecycle: goal " + focusedGoalId_ +
                         " advanced active -> completed after reliable plan '" + plan + "'.");
            if (!completedDescription.empty()) {
                appendMemory("Goal completed: " + completedDescription +
                             " (satisfied by reliable plan='" + plan + "').");
            }
            // Invariant repair: the active-goal pointer must never name a completed
            // goal. If the focused goal we just completed is also the currently
            // pursued active goal, clear the pointer immediately in the same step
            // so getActiveGoalId() only ever references an open goal. The next
            // perception step's selectActiveGoal() promotes the next pending goal.
            if (activeGoalId_ == focusedGoalId_) {
                activeGoalId_ = "";
            }
            // NOTE: adaptive re-seeding is intentionally NOT done here. It is owned
            // by evaluateGoalProgress(), which runs in the ACTION phase -- before
            // reflection and, crucially, after the current cycle's reasoning has
            // already chosen its plan. Seeding a fresh driver here (in reflection)
            // would swap the agent's objective mid-window and pull a sustained
            // dominant goal's plan off-topic on the very next cycle. Deferring to
            // the action-phase re-seed keeps a lone completed goal as the fallback
            // focus context (selectFocusGoal returns goals.back()), so reasoning
            // continues to serve its topic until a genuinely new open goal exists.
        }
    }
}

std::shared_ptr<void> AutonomousStarter::perceptionStep(std::shared_ptr<void> input) {
    ++cognitiveCycle_;
    // New cycle: clear the per-cycle id-completion guard so reflectionStep can
    // tell whether evaluateGoalProgress already retired a goal this cycle.
    goalCompletedThisCycleById_ = false;

    // Resolve which goal we are pursuing this cycle (promotes a pending goal to
    // active if nothing is active), so the whole observe-reason-act pass is
    // anchored to a single concrete intent.
    selectActiveGoal();

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

    // Attention bookkeeping: refresh the attention economy and the focused-goal
    // id every cycle so attention-bus introspection and novelty/importance
    // dynamics stay live. selectFocusGoal() also keeps focusedGoalId_ aligned
    // with the in-flight active goal (it honours activeGoalId_ when still open).
    const StateGoal* focus = selectFocusGoal();

    // Plan intent source: the plan must serve the goal the agent has actually
    // COMMITTED to this cycle -- the active (or next-pending) goal resolved by
    // perception via selectGoalContext(). This preserves the foundational
    // contract that a goal under pursuit keeps producing topically-aligned
    // plans until it is genuinely satisfied (a goal-first, single-tasking
    // drive). The attention focus is used only as a fallback when there is no
    // open goal context at all, so the attention economy can still suggest an
    // objective in the otherwise-idle case without yanking the plan away from
    // an actively pursued goal mid-drive.
    std::string goalContext = selectGoalContext();
    if ((goalContext.empty() || goalContext == "maintain safe baseline awareness") &&
        focus != nullptr && !focus->description.empty()) {
        goalContext = focus->description;
    }
    std::string normalizedGoal = goalContext;
    std::transform(normalizedGoal.begin(), normalizedGoal.end(), normalizedGoal.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    // Plan selection is goal-first: the plan must serve the goal the agent is
    // actively pursuing, otherwise action evidence can never satisfy that goal
    // and the closed loop cannot converge. Topic precedence is therefore keyed on
    // the active goal's description, with environmental hints used only as a
    // tie-breaker for goals that are themselves about project structure.
    if (normalizedGoal.find("situational awareness") != std::string::npos ||
        normalizedGoal.find("workspace") != std::string::npos ||
        normalizedGoal.find("awareness") != std::string::npos) {
        lastPlan_ = "establish situational awareness with pwd and directory inspection";
    } else if (normalizedGoal.find("test") != std::string::npos ||
        normalizedGoal.find("validation") != std::string::npos ||
        normalizedGoal.find("self-audit") != std::string::npos) {
        lastPlan_ = "self-audit validation and test surfaces";
    } else if (normalizedGoal.find("c++") != std::string::npos ||
               normalizedGoal.find("project structure") != std::string::npos ||
               normalizedGoal.find("source") != std::string::npos ||
               normalizedGoal.find("code actions") != std::string::npos) {
        lastPlan_ = "inspect C++ project structure with targeted source discovery";
    } else if (normalizedGoal.find("runtime") != std::string::npos ||
               normalizedGoal.find("system") != std::string::npos ||
               normalizedGoal.find("identity") != std::string::npos ||
               normalizedGoal.find("kernel") != std::string::npos) {
        lastPlan_ = "inspect system identity and runtime context";
    } else if (memoryCount < 5) {
        lastPlan_ = "establish situational awareness with pwd and directory inspection";
    } else if (actionCounter_ % 4 == 0) {
        lastPlan_ = "sample repository source files";
    } else if (actionCounter_ % 4 == 1) {
        lastPlan_ = "self-audit validation and test surfaces";
    } else if (actionCounter_ % 4 == 2) {
        lastPlan_ = "inspect system identity and kernel context";
    } else {
        lastPlan_ = "maintain lightweight environmental awareness";
    }

    // Build the candidate plan set implied by the focused goal/context.
    std::vector<std::string> candidatePlans;
    // Closed-loop feedback: if the previous actions have been failing, do not
    // keep selecting the same plan. Fall back to the safest situational-
    // awareness plan to recover ground truth before attempting richer actions.
    std::size_t recentFailures = 0;
    for (bool ok : recentActionOutcomes_) {
        if (!ok) ++recentFailures;
    }
    const bool strugglingNow = (recentActionOutcomes_.size() >= 2 && recentFailures >= 2);

    if (strugglingNow) {
        lastPlan_ = "establish situational awareness with pwd and directory inspection";
        appendMemory("Cycle " + std::to_string(cognitiveCycle_) +
                     " reasoning: goal = " + goalContext +
                     "; recent experience summary = " + summarizeRecentExperience() +
                     "; competence = " + std::to_string(competenceSignal_) +
                     "; detected repeated action failure, falling back to safe situational awareness" +
                     "; selected plan = " + lastPlan_ + ".");
        logInfo("Reasoning detected repeated failure; fallback plan: " + lastPlan_);
        return input;
    }

    if (normalizedGoal.find("test") != std::string::npos ||
        normalizedGoal.find("validation") != std::string::npos ||
        normalizedGoal.find("self-audit") != std::string::npos) {
        candidatePlans.push_back("self-audit validation and test surfaces");
    }
    if (normalizedGoal.find("c++") != std::string::npos ||
        normalizedGoal.find("project structure") != std::string::npos ||
        lastObservationSummary_.find("CMakeLists.txt") != std::string::npos) {
        candidatePlans.push_back("inspect C++ project structure with targeted source discovery");
    }
    if (normalizedGoal.find("runtime") != std::string::npos ||
        normalizedGoal.find("system") != std::string::npos) {
        candidatePlans.push_back("inspect system identity and runtime context");
    }
    if (memoryCount < 5 || normalizedGoal.find("situational awareness") != std::string::npos ||
        normalizedGoal.find("workspace") != std::string::npos) {
        candidatePlans.push_back("establish situational awareness with pwd and directory inspection");
    }
    // Always include the rotating exploratory plans so the agent keeps options
    // open even once core goals are addressed.
    candidatePlans.push_back("sample repository source files");
    candidatePlans.push_back("inspect system identity and kernel context");
    candidatePlans.push_back("maintain lightweight environmental awareness");

    // Endocrine-driven cognitive mode biasing: the virtual endocrine system
    // provides a CognitiveMode (Exploration/Exploitation/Rest/Alarm) that
    // modulates plan selection. In Alarm mode, prefer safe awareness plans;
    // in Exploration mode, prefer novel/diverse plans; in Rest mode, prefer
    // lightweight maintenance; in Exploitation mode, deepen the current focus.
    const CognitiveMode cogMode = endocrine_.cognitiveMode();
    if (cogMode == CognitiveMode::Alarm) {
        // Under alarm: override to safe fallback regardless of goal
        lastPlan_ = "establish situational awareness with pwd and directory inspection";
        appendMemory("Cycle " + std::to_string(cognitiveCycle_) +
                     " reasoning: endocrine alarm mode detected, overriding to safe plan.");
        logInfo("Reasoning: endocrine alarm override to safe plan");
        return input;
    } else if (cogMode == CognitiveMode::Rest && competenceSignal_ > 0.8) {
        // High competence + rest mode: lightweight maintenance
        lastPlan_ = "maintain lightweight environmental awareness";
        appendMemory("Cycle " + std::to_string(cognitiveCycle_) +
                     " reasoning: endocrine rest mode + high competence, selecting maintenance plan.");
        logInfo("Reasoning: endocrine rest mode, maintenance plan");
        return input;
    }
    // For Exploration/Exploitation modes, proceed with outcome-based plan selection.

    // Goal-first precedence: the if/else chain above already resolved the plan
    // that directly serves the active goal into lastPlan_. That plan is the
    // preferred candidate -- promote it to the front of the candidate list so a
    // closed loop can actually converge on the goal it is pursuing. Without this
    // the candidate-bias loop below would discard the goal-relevant plan in
    // favour of an exploratory one whenever their success priors tie.
    const std::string goalPreferredPlan = lastPlan_;
    {
        auto existing = std::find(candidatePlans.begin(), candidatePlans.end(),
                                  goalPreferredPlan);
        if (existing != candidatePlans.end()) {
            candidatePlans.erase(existing);
        }
        if (!goalPreferredPlan.empty()) {
            candidatePlans.insert(candidatePlans.begin(), goalPreferredPlan);
        }
    }

    // Outcome-based plan adaptation: start from the goal-preferred plan and only
    // switch to another candidate when it has STRICTLY better measured evidence.
    // A strict ' > bestBias' comparison keyed off the preferred plan's own bias
    // means unseen-plan priors (all 0.5) and equal success ratios can never
    // displace the goal-relevant choice; accumulated outcomes still can.
    lastPlan_ = goalPreferredPlan.empty() ? candidatePlans.front()
                                          : goalPreferredPlan;
    double bestBias = planBias(lastPlan_);
    for (const auto& plan : candidatePlans) {
        const double bias = planBias(plan);
        if (bias > bestBias) {
            bestBias = bias;
            lastPlan_ = plan;
        }
    }

    // Stagnation guard: if reasoning keeps producing the same plan, the agent is
    // stuck. Count consecutive repeats and, once a threshold is crossed, escalate
    // to a deliberately different plan so autonomy explores instead of looping.
    //
    // Crucial refinement (living-systems principle): purposeful repetition is not
    // stagnation. When the repeated plan is the one that directly SERVES the
    // agent's current goal, repeating it is convergence toward that goal, not a
    // loop the agent is trapped in. Escalating away from a goal-aligned plan would
    // starve the goal of the very evidence it needs to complete. We therefore
    // exempt the goal-serving plan from escalation while still counting repeats
    // (so the bounded-stagnation invariant holds), and only force a different plan
    // when the repetition is aimless (plan not aligned with the active goal).
    if (lastPlan_ == previousPlan_) {
        ++stagnationCounter_;
    } else {
        stagnationCounter_ = 0;
    }
    const bool planServesCurrentGoal = planSatisfiesGoalTopic(normalizedGoal, lastPlan_);
    if (stagnationCounter_ >= 2 && !planServesCurrentGoal) {
        static const std::array<const char*, 4> escalationPlans = {{
            "sample repository source files",
            "self-audit validation and test surfaces",
            "inspect system identity and kernel context",
            "establish situational awareness with pwd and directory inspection"
        }};
        std::string escalated = lastPlan_;
        for (const char* candidate : escalationPlans) {
            if (candidate != lastPlan_) {
                escalated = candidate;
                break;
            }
        }
        appendMemory("Stagnation detected after " + std::to_string(stagnationCounter_) +
                     " repeats of plan '" + lastPlan_ + "'; escalating to '" + escalated + "'.");
        lastPlan_ = escalated;
        stagnationCounter_ = 0;
    } else if (stagnationCounter_ >= 2 && planServesCurrentGoal) {
        // Purposeful repetition: keep pursuing the goal-aligned plan but reset the
        // counter so the bounded-stagnation invariant (<= 2) is preserved and the
        // guard remains armed for genuinely aimless loops later.
        appendMemory("Sustained goal-aligned pursuit of plan '" + lastPlan_ +
                     "' (convergence, not stagnation); resetting stagnation counter.");
        stagnationCounter_ = 0;
    }
    previousPlan_ = lastPlan_;

    appendMemory("Cycle " + std::to_string(cognitiveCycle_) +
                 " reasoning: focus_goal_id = " + (focus ? focus->id : std::string("none")) +
                 "; goal = " + goalContext +
                 "; recent experience summary = " + summarizeRecentExperience() +
                 "; competence = " + std::to_string(competenceSignal_) +
                 "; selected plan = " + lastPlan_ +
                 " (bias=" + std::to_string(bestBias) + ").");
    logInfo("Reasoning selected plan: " + lastPlan_);
    return input;
}

std::shared_ptr<void> AutonomousStarter::actionStep(std::shared_ptr<void> input) {
    const std::string command = buildActionCommandForPlan(lastPlan_);

    ++actionCounter_;
    const auto result = executeShellCommand(command);

    // Capture the outcome so the reflection phase and the next reasoning cycle
    // can adapt. This closes the observe->reason->act->reflect feedback loop.
    lastActionCommand_ = command;
    lastActionSucceeded_ = result.success;
    lastActionOutput_ = result.output;
    lastActionExitCode_ = result.exitCode;

    appendMemory("Cycle " + std::to_string(cognitiveCycle_) +
                 " action: command='" + command + "', success=" +
                 std::string(result.success ? "true" : "false") +
                 ", exitCode=" + std::to_string(result.exitCode) + ".");

    // Endocrine stimulus: feed the action outcome into the virtual endocrine system
    // so hormone levels modulate subsequent plan selection and memory consolidation.
    if (result.success) {
        endocrine_.submitStimulus(Stimulus("action_success", 0.7));
        if (getOpenGoalCount() == 0) {
            endocrine_.submitStimulus(Stimulus("goal_completed", 0.9));
        }
    } else {
        endocrine_.submitStimulus(Stimulus("error_detected", 0.6));
        if (consecutiveActionFailures_ >= 3) {
            endocrine_.submitStimulus(Stimulus("sustained_failure", 0.8));
        }
    }
    endocrine_.tick();

    // Close the loop: feed the action outcome back into goal state so successful,
    // aligned actions complete the active goal and promote the next one. This is
    // what turns the goal list from a static seed into a converging drive.
    evaluateGoalProgress(lastPlan_, command, result);
    return input;
}

std::string AutonomousStarter::activeGoalDescriptionForPlan() const {
    // The goal actually served this cycle is the one perception/reasoning
    // committed to: activeGoalId_. Reflection must complete THAT goal -- the one
    // whose topic drove plan selection and whose probe just ran -- otherwise it
    // completes an unrelated first-open goal while the plan served a different
    // one, desynchronising plan / completion / focus (the cause of the plan
    // drifting away from the goal under pursuit). Only when no goal is in flight
    // do we fall back to the first open goal in selection precedence.
    const auto& goals = state_.getGoals();
    if (!activeGoalId_.empty()) {
        for (const auto& goal : goals) {
            if (goal.id == activeGoalId_) {
                const std::string status = toLowerAscii(goal.status);
                if (status == "active" || status == "in_progress" ||
                    status == "pending") {
                    return goal.description;
                }
                break;
            }
        }
    }
    for (const auto& goal : goals) {
        std::string status = toLowerAscii(goal.status);
        if (status == "active" || status == "in_progress" || status == "pending") {
            return goal.description;
        }
    }
    return std::string();
}

std::shared_ptr<void> AutonomousStarter::reflectionStep(std::shared_ptr<void> input) {
    // Reflection is the fourth Echobeats-aligned phase and the point where the
    // observe->reason->act->reflect loop becomes self-regulating. It performs
    // three complementary jobs:
    //   1. Feed the action outcome back into per-plan success statistics and a
    //      bounded competence signal that biases future plan selection.
    //   2. Advance the focused goal's lifecycle (complete/rotate/escalate) so
    //      the agent does not loop forever on a satisfied or failing probe.
    //   3. Record an adaptive reflective note that becomes part of the
    //      experience the next reasoning cycle summarizes.
    recordPlanOutcome(lastPlan_, lastActionSucceeded_);
    advanceGoalLifecycle(lastPlan_, lastActionSucceeded_);

    // --- 1. Competence signal + rolling outcome window ---------------------
    constexpr std::size_t kOutcomeWindow = 5;
    recentActionOutcomes_.push_back(lastActionSucceeded_);
    while (recentActionOutcomes_.size() > kOutcomeWindow) {
        recentActionOutcomes_.pop_front();
    }
    if (lastActionSucceeded_) {
        ++successfulActionCount_;
        // Competence rises toward 1.0 with diminishing returns.
        competenceSignal_ += (1.0 - competenceSignal_) * 0.2;
    } else {
        ++failedActionCount_;
        // Competence falls toward 0.0 more sharply so failures register quickly.
        competenceSignal_ -= competenceSignal_ * 0.3;
    }
    competenceSignal_ = std::max(0.0, std::min(1.0, competenceSignal_));

    std::size_t windowFailures = 0;
    for (bool ok : recentActionOutcomes_) {
        if (!ok) ++windowFailures;
    }

    const double ratio = getPlanSuccessRatio(lastPlan_);
    std::ostringstream reflection;
    reflection << "Cycle " << cognitiveCycle_ << " reflection: plan='" << lastPlan_ << "'"
               << ", action='" << lastActionCommand_ << "'"
               << ", outcome=" << (lastActionSucceeded_ ? "succeeded" : "failed")
               << ", exitCode=" << lastActionExitCode_
               << ", competence=" << competenceSignal_
               << ", plan_success_ratio=" << ratio
               << ", recentFailures=" << windowFailures << "/" << recentActionOutcomes_.size()
               << ", focus_goal=" << (focusedGoalId_.empty() ? "none" : focusedGoalId_) << "; ";

    // --- 2. Goal lifecycle advancement / adaptive escalation ----------------
    const std::string activeGoal = activeGoalDescriptionForPlan();
    if (lastActionSucceeded_) {
        consecutiveActionFailures_ = 0;

        // Goal completion is owned by a SINGLE authoritative, evidence-gated path
        // (evaluateGoalProgress in actionStep + advanceGoalLifecycle above). The
        // reflection phase deliberately no longer completes goals by description
        // nor force-activates the next pending goal: doing so previously raced the
        // id-based path, double-completed on the first aligned success, and caused
        // a sustained dominant goal to lose plan continuity after cycle 1. Here we
        // only report the current convergence state; promotion of the next pending
        // goal is handled deterministically by selectActiveGoal() on the next
        // perception step, keeping exactly one goal in flight without a duplicate
        // rotation mechanism.
        completedGoalCount_ = getCompletedGoalCount();
        reflection << "completed_total=" << completedGoalCount_;
    } else {
        ++consecutiveActionFailures_;
        reflection << "consecutive_failures=" << consecutiveActionFailures_;

        // Adaptive escalation: if the agent repeatedly fails, fall back to a
        // minimal, guaranteed-safe situational-awareness goal so the next plan
        // selects a low-risk probe rather than retrying a failing path.
        if (consecutiveActionFailures_ >= 2 && !activeGoal.empty()) {
            const Timestamp now = std::chrono::system_clock::now();
            state_.updateGoalStatusByDescription(activeGoal, "blocked");
            state_.addGoal(StateGoal{
                generateUUID(),
                "Re-establish bounded situational awareness after repeated action failure",
                "active",
                now,
                now
            });
            reflection << "; escalated: blocked failing goal and re-seeded safe awareness goal";
        } else if (competenceSignal_ > 0.8) {
            reflection << "; lesson=high competence, free to pursue more novel goals";
        } else {
            reflection << "; lesson=steady progress, continuing current strategy";
        }
    }

    // --- 3. Active-goal reconciliation --------------------------------------
    // Three mechanisms can advance goal status within a single cycle:
    //   * evaluateGoalProgress() completes the active goal by id (actionStep),
    //   * advanceGoalLifecycle() advances the focused goal by focusedGoalId_,
    //   * the reflection block above completes the active goal by description.
    // Only the first keeps activeGoalId_ in sync. If either of the other two
    // transitions the goal activeGoalId_ points at into a non-open state, the
    // id would be left dangling on a completed/blocked goal, violating the
    // invariant that getActiveGoalId() either is empty or references an open
    // goal. Reconcile here: if activeGoalId_ no longer references an open goal,
    // clear it so the next cycle's selectActiveGoal() promotes a valid one.
    if (!activeGoalId_.empty()) {
        bool stillOpen = false;
        for (const auto& goal : state_.getGoals()) {
            if (goal.id == activeGoalId_) {
                const std::string status = toLowerAscii(goal.status);
                stillOpen = (status == "active" || status == "in_progress" ||
                             status == "pending");
                break;
            }
        }
        if (!stillOpen) {
            activeGoalId_ = "";
        }
    }

    lastReflection_ = reflection.str();
    ++reflectionCount_;
    appendMemory(lastReflection_);
    logInfo("Reflection: competence=" + std::to_string(competenceSignal_) +
            " outcome=" + std::string(lastActionSucceeded_ ? "succeeded" : "failed"));
    logInfo(lastReflection_);
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

AutonomousStarter::AutonomyHealthReport AutonomousStarter::getAutonomyHealthReport() const {
    AutonomyHealthReport report;
    report.competence = competenceSignal_;
    report.openGoals = getOpenGoalCount();
    report.completedGoals = getCompletedGoalCount();
    report.totalCycles = cognitiveCycle_;
    report.totalActions = actionCounter_;
    report.successfulActions = successfulActionCount_;
    report.failedActions = failedActionCount_;
    report.consecutiveFailures = consecutiveActionFailures_;
    report.stagnationCount = stagnationCounter_;
    report.reflections = reflectionCount_;
    report.focusedGoalId = focusedGoalId_;
    report.lastPlan = lastPlan_;
    report.lastReflection = lastReflection_;

    // Compute rates
    const std::size_t totalGoals = report.openGoals + report.completedGoals;
    report.goalCompletionRate = totalGoals > 0
        ? static_cast<double>(report.completedGoals) / static_cast<double>(totalGoals)
        : 0.0;
    report.actionSuccessRate = report.totalActions > 0
        ? static_cast<double>(report.successfulActions) / static_cast<double>(report.totalActions)
        : 0.0;

    // Health assessment
    std::string summary;
    report.isHealthy = true;

    if (report.consecutiveFailures >= 3) {
        report.isHealthy = false;
        summary += "CRITICAL: " + std::to_string(report.consecutiveFailures) +
                   " consecutive action failures. ";
    }
    if (report.competence < 0.2 && report.totalCycles > 5) {
        report.isHealthy = false;
        summary += "WARNING: Low competence (" + std::to_string(report.competence) +
                   ") after " + std::to_string(report.totalCycles) + " cycles. ";
    }
    if (report.stagnationCount >= 2) {
        summary += "NOTICE: Stagnation detected (" + std::to_string(report.stagnationCount) +
                   " repeats). ";
    }
    if (report.openGoals == 0 && report.totalCycles > 0) {
        summary += "INFO: All goals completed; adaptive exploration active. ";
    }
    if (summary.empty()) {
        summary = "HEALTHY: Agent operating normally. competence=" +
                  std::to_string(report.competence) + " cycles=" +
                  std::to_string(report.totalCycles) + " success_rate=" +
                  std::to_string(report.actionSuccessRate);
    }
    report.healthSummary = summary;
    return report;
}

} // namespace elizaos
