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

#include <atomic>
#include <chrono>
#include <memory>
#include <string>

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
    std::size_t runCognitiveCycleOnce();
    std::size_t getCognitiveCycleCount() const { return cognitiveCycle_; }
    std::size_t getActionCount() const { return actionCounter_; }
    const std::string& getLastObservationSummary() const { return lastObservationSummary_; }
    const std::string& getLastPlan() const { return lastPlan_; }

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

    // Internal cognitive steps
    std::shared_ptr<void> perceptionStep(std::shared_ptr<void> input);
    std::shared_ptr<void> reasoningStep(std::shared_ptr<void> input);
    std::shared_ptr<void> actionStep(std::shared_ptr<void> input);

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
