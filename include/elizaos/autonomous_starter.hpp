#pragma once

/**
 * ElizaOS C++ - Autonomous Starter
 *
 * Canonical public header for the autonomous starter integration package.
 * The class exposes a safe, deterministic C++ autonomy surface around agent
 * state, shell-command execution, task dispatch, and a perception/reason/action
 * cognitive loop.
 */

#include "elizaos/core.hpp"
#include "elizaos/agentloop.hpp"
#include "elizaos/agentshell.hpp"
#include "elizaos/attention.hpp"
#include "elizaos/endocrine.hpp"

#include <atomic>
#include <chrono>
#include <deque>
#include <memory>
#include <mutex>
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
    // Legacy reference access is only safe while execution is quiescent. Prefer
    // getCurrentWorkingDirectorySnapshot() while cycles or task workers may run.
    const std::string& getCurrentWorkingDirectory() const { return currentWorkingDirectory_; }
    std::string getCurrentWorkingDirectorySnapshot() const;

    // Autonomous loop control
    void startAutonomousLoop();
    void stopAutonomousLoop();
    bool isAutonomousLoopRunning() const;
    void setLoopInterval(std::chrono::milliseconds interval);
    std::chrono::milliseconds getLoopInterval() const;

    // Deterministic single-cycle autonomy controls for tests, supervisors, and
    // embedding runtimes that need bounded observe-reason-act stepping.
    // Each cycle now runs the full perceive -> reason -> act -> reflect loop.
    std::size_t runCognitiveCycleOnce();
    std::size_t getCognitiveCycleCount() const;
    std::size_t getActionCount() const;
    // Legacy reference accessors are unsafe during active execution because a
    // returned reference outlives the protecting lock. Snapshot accessors below
    // return owned values and are safe for concurrent supervisors.
    const std::string& getLastObservationSummary() const { return lastObservationSummary_; }
    const std::string& getLastPlan() const { return lastPlan_; }
    std::string getLastObservationSummarySnapshot() const;
    std::string getLastPlanSnapshot() const;

    // Closed-loop goal-progression introspection. These expose the convergence
    // signals an autonomy supervisor needs: how many seeded goals are still open
    // (pending/active/in_progress) versus completed, the id of the goal the agent
    // is currently pursuing, and how many consecutive cycles produced the same
    // plan (a stagnation signal). A healthy autonomous agent drives open goals to
    // completion rather than looping a single plan indefinitely.
    std::size_t getOpenGoalCount() const;
    // Legacy reference access is unsafe during active execution; prefer the
    // value-returning getActiveGoalIdSnapshot().
    const UUID& getActiveGoalId() const { return activeGoalId_; }
    UUID getActiveGoalIdSnapshot() const;
    std::size_t getStagnationCounter() const;
    // Reflection / learning surface. After each cycle the agent records whether
    // the executed action succeeded and the reflective conclusion it drew.
    const std::string& getLastReflection() const { return lastReflection_; }
    std::string getLastReflectionSnapshot() const;
    std::size_t getReflectionCount() const;
    bool getLastActionSucceeded() const;
    // Backwards-compatible alias retained for tests and embedders that adopted
    // the shorter accessor name before the loop was unified.
    bool lastActionSucceeded() const { return getLastActionSucceeded(); }
    int getLastActionExitCode() const;

    // Attention-weighted autonomy introspection. Returns the goal id the agent
    // is currently focused on (highest attention composite score among open
    // goals), or an empty string when no goals exist.
    UUID getFocusedGoalId() const;

    // Returns the success ratio (0.0-1.0) of every plan label the agent has
    // executed so far. Used by supervisors and tests to confirm that the agent
    // adapts plan selection based on accumulated outcome feedback.
    double getPlanSuccessRatio(const std::string& plan) const;
    std::size_t getPlanAttemptCount(const std::string& plan) const;
    std::size_t getPlanSuccessCount(const std::string& plan) const;

    // Bounded competence estimate in [0, 1]; rises after successful actions and
    // falls after failures. Drives plan fallback selection on the next cycle.
    double getCompetenceSignal() const;
    std::size_t getSuccessfulActionCount() const;
    std::size_t getFailedActionCount() const;
    std::size_t getCompletedGoalCount() const;
    std::size_t getConsecutiveActionFailures() const;

    // Attention-weighted goal selection. Exposes the goal the attention
    // allocator currently considers highest priority. Empty when no goals.
    std::string getAttentionPrioritizedGoal() const;

    // State access. These legacy references preserve source compatibility but
    // cannot remain protected after return; callers must use them only while the
    // starter is quiescent or under their own external synchronization. Prefer
    // getStateSnapshot()/getEndocrineSystemSnapshot() for concurrent reads.
    State& getState() { return state_; }
    const State& getState() const { return state_; }
    State getStateSnapshot() const;
    const AgentConfig& getConfig() const { return config_; }
    const EndocrineSystem& getEndocrineSystem() const { return endocrine_; }
    EndocrineSystem getEndocrineSystemSnapshot() const;

    // Task management
    UUID executeShellCommandAsTask(const std::string& command);

    // Comprehensive autonomy health diagnostics. Returns a structured report
    // summarizing the agent's current operational state, goal convergence,
    // competence trajectory, and memory health for supervisory systems.
    struct AutonomyHealthReport {
        double competence = 0.0;
        std::size_t openGoals = 0;
        std::size_t completedGoals = 0;
        std::size_t totalCycles = 0;
        std::size_t totalActions = 0;
        std::size_t successfulActions = 0;
        std::size_t failedActions = 0;
        std::size_t consecutiveFailures = 0;
        std::size_t stagnationCount = 0;
        std::size_t reflections = 0;
        std::string focusedGoalId;
        std::string lastPlan;
        std::string lastReflection;
        double goalCompletionRate = 0.0;
        double actionSuccessRate = 0.0;
        // Goal-diversity telemetry (autonomy optimization): measures how evenly the
        // agent's completed goals are spread across distinct goal themes. A collapsed
        // diversity (near 0 with several completions) means the agent is looping one
        // theme; healthy exploration keeps diversity above the 0.3 floor once enough
        // goals have completed. Computed as normalized Shannon entropy over completed
        // goal theme buckets in [0,1].
        double goalThemeDiversity = 0.0;
        std::size_t distinctGoalThemes = 0;
        // Cognitive momentum: exponentially-weighted moving average of recent
        // action success (window=10 cycles). High momentum (>0.7) signals the
        // agent is in a productive streak; low momentum (<0.3) triggers
        // plan-strategy fallback to simpler actions.
        double cognitiveMomentum = 0.0;
        // Cycle efficiency: ratio of cycles that produced at least one
        // meaningful state change (goal progress, new memory, or successful
        // action) vs total cycles. Measures how much cognitive work translates
        // to observable progress rather than idle perception loops.
        double cycleEfficiency = 0.0;
        // Goal-chain coherence: measures sequential thematic relatedness between
        // consecutively completed goals (Jaccard similarity of 3-token theme
        // sets). High coherence (>0.6) means the agent pursues related
        // objectives in sequence; low coherence means scattered topic-hopping.
        double goalChainCoherence = 0.0;
        bool isHealthy = true;
        std::string healthSummary;
    };
    AutonomyHealthReport getAutonomyHealthReport() const;

private:
    // Locking contract:
    //   * lifecycleMutex_ serializes public start/stop/restart/manual-cycle and
    //     task-scheduling operations.
    //   * stateMutex_ is the single-writer gate for State, endocrine state, the
    //     working directory, cognitive phases, memories, and telemetry.
    //   * lock order is lifecycleMutex_ -> stateMutex_. Background cycles and
    //     task workers take only stateMutex_, allowing lifecycle code to release
    //     stateMutex_ before joining workers that may be finishing a write.
    // No callback or worker is joined while stateMutex_ is held.
    std::size_t runCognitiveCycleLocked();
    void startAutonomousLoopLocked();
    void stopAutonomousLoopLocked();

    // Goal-driven autonomy helpers
    void ensureCoreAutonomyGoals();
    std::string selectGoalContext() const;
    std::string buildActionCommandForPlan(const std::string& plan) const;

    // Closed-loop goal-progression helpers.
    // selectActiveGoal picks the highest-priority open goal (active >
    // in_progress > pending) and records its id in activeGoalId_.
    // evaluateGoalProgress inspects the latest action evidence and advances goal
    // statuses: a satisfied active goal becomes "completed", and the next pending
    // goal is promoted to "active". seedAdaptiveGoal injects a fresh exploration
    // goal when every seeded goal is complete so autonomy never dead-ends.
    const StateGoal* selectActiveGoal();
    void evaluateGoalProgress(const std::string& plan,
                              const std::string& command,
                              const ShellCommandResult& result);
    void seedAdaptiveGoal();
    bool planSatisfiesGoal(const StateGoal& goal,
                           const std::string& plan,
                           const ShellCommandResult& result) const;
    // Topic-only variant of goal/plan alignment used by the stagnation guard to
    // decide whether a repeated plan is purposeful (serving the current goal) or
    // aimless. Unlike planSatisfiesGoal it takes an already-normalized goal string
    // and does not require a successful ShellCommandResult, because it answers
    // "does this plan serve this goal's topic?" independent of any single action's
    // outcome.
    bool planSatisfiesGoalTopic(const std::string& normalizedGoal,
                                const std::string& plan) const;
    // Attention-weighted goal selection. Seeds/refreshes attention values for
    // every open goal and returns the highest-scoring open goal, falling back
    // to the most recent goal when none are open.
    const StateGoal* selectFocusGoal();
    void refreshGoalAttention();

    // Outcome-based plan adaptation. Records the success/failure of a plan and
    // biases future plan selection toward historically successful plans.
    void recordPlanOutcome(const std::string& plan, bool success);
    double planBias(const std::string& plan) const;
    double planSuccessRatioLocked(const std::string& plan) const;
    std::size_t openGoalCountLocked() const;
    std::size_t completedGoalCountLocked() const;

    // Goal lifecycle transition driven by accomplished plans.
    void advanceGoalLifecycle(const std::string& plan, bool actionSucceeded);

    // Internal cognitive steps
    // Internal cognitive steps. perception->reasoning->action->reflection forms
    // the full four-phase Echobeats-aligned cognitive cycle.
    std::shared_ptr<void> perceptionStep(std::shared_ptr<void> input);
    std::shared_ptr<void> reasoningStep(std::shared_ptr<void> input);
    std::shared_ptr<void> actionStep(std::shared_ptr<void> input);
    std::shared_ptr<void> reflectionStep(std::shared_ptr<void> input);

    // Attention-driven goal prioritization helper. Scores currently open goals
    // through the repository's AttentionAllocator and returns the description of
    // the highest-scoring goal (falling back to selectGoalContext()).
    std::string scoreAndSelectGoal() const;
    // Map a satisfied plan probe back onto the goal it advances, returning the
    // active goal description that the current plan is serving.
    std::string activeGoalDescriptionForPlan() const;

    // Memory helpers
    void appendMemoryLocked(const std::string& content);
    std::string summarizeRecentExperience(std::size_t maxItems = 4) const;

    // Shell helpers
    ShellCommandResult validateShellCommand(const std::string& command) const;
    ShellCommandResult executeShellCommandLocked(const std::string& command);
    ShellCommandResult executeInternalCd(const std::string& command);
    ShellCommandResult executeExternalShellCommand(const std::string& command);
    bool executeShellTaskLocked(Task& task, const TaskOptions& options);

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

    mutable std::mutex lifecycleMutex_;
    mutable std::mutex stateMutex_;

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

    // Closed-loop goal-progression state.
    UUID activeGoalId_;
    std::string previousPlan_;
    std::size_t stagnationCounter_{0};
    std::size_t adaptiveGoalCounter_{0};
    // Theme of the most recently completed goal. seedAdaptiveGoal() uses this to
    // keep a freshly-idle agent's next self-seeded goal coherent with the drive
    // it was just pursuing (intent continuity) rather than topic-hopping.
    std::string lastCompletedGoalDescription_;
    // One-shot continuation latch for intent continuity in seedAdaptiveGoal():
    // true once a self-audit theme has already earned its single continuation
    // pass, reset when the agent broadens into exploratory rotation.
    bool selfAuditContinued_{false};
    std::size_t selfAuditPass_{0};
    // Remaining passes in the current bounded self-audit verification sprint.
    std::size_t selfAuditSprintRemaining_{0};
    // Per-cycle guard: set true when evaluateGoalProgress() (actionStep) has
    // already completed the active goal by id this cycle, so reflectionStep()
    // does not redundantly complete a SECOND goal by description in the same
    // cycle. Reset at the start of every cycle in perceptionStep(). This makes
    // evaluateGoalProgress the single completion authority and keeps exactly one
    // goal completed per successful cycle (reconciling the historically divergent
    // by-id and by-description completion writers).
    bool goalCompletedThisCycleById_{false};
    // Reflection / learning state.
    std::string lastReflection_;
    std::size_t reflectionCount_{0};
    std::string lastActionOutput_;

    // Closed-loop reflection / feedback state.
    std::string lastActionCommand_;
    bool lastActionSucceeded_{false};
    int lastActionExitCode_{0};
    double competenceSignal_{0.5};
    std::size_t successfulActionCount_{0};
    std::size_t failedActionCount_{0};
    std::size_t completedGoalCount_{0};
    std::size_t consecutiveActionFailures_{0};
    // Rolling window of recent action outcomes (true=success) used by reasoning
    // to detect repeated failure and switch to a safe fallback plan.
    std::deque<bool> recentActionOutcomes_;

    // Per-plan outcome feedback: plan label -> (successes, attempts).
    struct PlanStats { std::size_t successes{0}; std::size_t attempts{0}; };
    std::unordered_map<std::string, PlanStats> planStats_;

    // Attention allocator used for goal prioritization. mutable so that const
    // goal-selection accessors can update transient attention bookkeeping.
    mutable AttentionAllocator goalAttention_;
    UUID focusedGoalId_;

    // Cognitive momentum tracking: EWMA of recent action outcomes.
    double cognitiveMomentum_{0.5};
    // Productive cycle counter (cycles with at least one meaningful outcome).
    std::size_t productiveCycleCount_{0};

    // Virtual endocrine system: provides hormone-based affect signaling that
    // modulates plan selection (CognitiveMode) and memory consolidation.
    EndocrineSystem endocrine_;
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
