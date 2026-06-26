#pragma once

/**
 * ElizaOS C++ - Autonomous Starter
 *
 * Canonical public header for the autonomous starter integration package.
 * The class exposes a safe, deterministic C++ autonomy surface around agent
 * state, shell-command execution, task dispatch, and a perception/reason/action
 * cognitive loop.
 */

#include "core.hpp"
#include "agentloop.hpp"
#include "agentshell.hpp"
#include "attention.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

/**
 * AutonomousStarter - Core autonomous agent implementation.
 *
 * The starter is intentionally lightweight: it uses the existing State,
 * TaskManager, AgentLoop, and ShellCommandResult primitives instead of pulling
 * in external runtime dependencies. Shell access can be disabled, command
 * execution is guarded by explicit safety checks, and every significant
 * autonomy transition is written into the agent's recent-memory stream.
 */
class AutonomousStarter {
public:
    explicit AutonomousStarter(const AgentConfig& config);
    ~AutonomousStarter();

    AutonomousStarter(const AutonomousStarter&) = delete;
    AutonomousStarter& operator=(const AutonomousStarter&) = delete;

    // Core lifecycle
    void start();
    void stop();
    bool isRunning() const { return running_; }

    // Shell command execution
    ShellCommandResult executeShellCommand(const std::string& command);
    void enableShellAccess(bool enabled) { shellAccessEnabled_ = enabled; }
    bool isShellAccessEnabled() const { return shellAccessEnabled_; }
    const std::string& getCurrentWorkingDirectory() const { return currentWorkingDirectory_; }

    // Autonomous loop control
    void startAutonomousLoop();
    void stopAutonomousLoop();
    bool isAutonomousLoopRunning() const;
    void setLoopInterval(std::chrono::milliseconds interval);
    std::chrono::milliseconds getLoopInterval() const { return loopInterval_; }

    // Deterministic single-cycle autonomy controls for tests, supervisors, and
    // embedding runtimes that need bounded observe-reason-act stepping.
    // Each cycle now runs the full perceive -> reason -> act -> reflect loop.
    std::size_t runCognitiveCycleOnce();
    std::size_t getCognitiveCycleCount() const { return cognitiveCycle_; }
    std::size_t getActionCount() const { return actionCounter_; }
    const std::string& getLastObservationSummary() const { return lastObservationSummary_; }
    const std::string& getLastPlan() const { return lastPlan_; }

    // Reflection / learning surface. After each cycle the agent records whether
    // the executed action succeeded and the reflective conclusion it drew.
    const std::string& getLastReflection() const { return lastReflection_; }
    std::size_t getReflectionCount() const { return reflectionCount_; }
    bool getLastActionSucceeded() const { return lastActionSucceeded_; }

    // Attention-weighted autonomy introspection. Returns the goal id the agent
    // is currently focused on (highest attention composite score among open
    // goals), or an empty string when no goals exist.
    UUID getFocusedGoalId() const { return focusedGoalId_; }

    // Returns the success ratio (0.0-1.0) of every plan label the agent has
    // executed so far. Used by supervisors and tests to confirm that the agent
    // adapts plan selection based on accumulated outcome feedback.
    double getPlanSuccessRatio(const std::string& plan) const;

    // State access
    State& getState() { return state_; }
    const State& getState() const { return state_; }
    const AgentConfig& getConfig() const { return config_; }

    // Task management
    UUID executeShellCommandAsTask(const std::string& command);

private:
    // Goal-driven autonomy helpers
    void ensureCoreAutonomyGoals();
    std::string selectGoalContext() const;
    std::string buildActionCommandForPlan(const std::string& plan) const;

    // Attention-weighted goal selection. Seeds/refreshes attention values for
    // every open goal and returns the highest-scoring open goal, falling back
    // to the most recent goal when none are open.
    const StateGoal* selectFocusGoal();
    void refreshGoalAttention();

    // Outcome-based plan adaptation. Records the success/failure of a plan and
    // biases future plan selection toward historically successful plans.
    void recordPlanOutcome(const std::string& plan, bool success);
    double planBias(const std::string& plan) const;

    // Goal lifecycle transition driven by accomplished plans.
    void advanceGoalLifecycle(const std::string& plan, bool actionSucceeded);

    // Internal cognitive steps
    std::shared_ptr<void> perceptionStep(std::shared_ptr<void> input);
    std::shared_ptr<void> reasoningStep(std::shared_ptr<void> input);
    std::shared_ptr<void> actionStep(std::shared_ptr<void> input);
    std::shared_ptr<void> reflectionStep(std::shared_ptr<void> input);

    // Memory helpers
    void appendMemory(const std::string& content);
    std::string summarizeRecentExperience(std::size_t maxItems = 4) const;

    // Shell helpers
    ShellCommandResult validateShellCommand(const std::string& command) const;
    ShellCommandResult executeInternalCd(const std::string& command);
    ShellCommandResult executeExternalShellCommand(const std::string& command);

    class ShellCommandWorker : public TaskWorker {
    public:
        explicit ShellCommandWorker(AutonomousStarter* starter) : starter_(starter) {}

        std::string getName() const override { return "shell_command"; }
        bool validate(const Task& task, const State& state,
                      std::shared_ptr<Memory> message) const override;
        bool execute(Task& task, State& state, const TaskOptions& options) override;

    private:
        AutonomousStarter* starter_;
    };

    AgentConfig config_;
    State state_;
    std::atomic<bool> running_{false};
    std::atomic<bool> shellAccessEnabled_{true};

    std::unique_ptr<AgentLoop> autonomousLoop_;
    std::chrono::milliseconds loopInterval_{1000};

    std::unique_ptr<TaskManager> taskManager_;
    std::shared_ptr<ShellCommandWorker> shellWorker_;

    std::string currentWorkingDirectory_;
    std::size_t cognitiveCycle_{0};
    std::size_t actionCounter_{0};
    std::string lastObservationSummary_;
    std::string lastPlan_;

    // Reflection / learning state.
    std::string lastReflection_;
    std::size_t reflectionCount_{0};
    bool lastActionSucceeded_{false};
    std::string lastActionCommand_;
    std::string lastActionOutput_;

    // Attention-weighted goal selection state.
    AttentionAllocator goalAttention_;
    UUID focusedGoalId_;

    // Per-plan outcome feedback: plan label -> (successes, attempts).
    struct PlanStats { std::size_t successes{0}; std::size_t attempts{0}; };
    std::unordered_map<std::string, PlanStats> planStats_;
};

std::shared_ptr<AutonomousStarter> createAutolizaAgent();

/**
 * Run a lightweight AutonomousStarter health check.
 *
 * The check constructs a bounded Autoliza instance, verifies lifecycle and shell
 * guard behavior, executes a harmless command, and stops the agent. It returns
 * false instead of throwing when a check fails, which makes it suitable for
 * package-link smoke tests and startup diagnostics.
 */
bool autonomous_starter_self_check();

/**
 * Compatibility wrapper retained for older downstream code.
 * Prefer autonomous_starter_self_check() for meaningful diagnostics.
 */
void autonomous_starter_placeholder();

} // namespace elizaos
