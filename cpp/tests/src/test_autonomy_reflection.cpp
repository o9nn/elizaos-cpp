// test_autonomy_reflection.cpp
// Comprehensive E2E coverage for the closed-loop autonomy enhancements added to
// AutonomousStarter: the four-phase observe->reason->act->reflect cognitive
// cycle, the bounded competence signal, failure-driven plan fallback, and
// attention-weighted goal prioritization.
//
// These tests exercise the public AutonomousStarter API only, so they lock in
// the behavioral contract of the self-regulating loop without depending on
// private implementation details.

#include <gtest/gtest.h>
#include "elizaos/autonomous_starter.hpp"
#include "elizaos/core.hpp"

#include <string>
#include <vector>
//
// E2E tests for the closed-loop reflection phase of the autonomy cognitive
// cycle. Prior to the reflection repair, AutonomousStarter ran a strictly
// open-loop observe -> reason -> act cycle: action outcomes were written to
// memory but never fed back into goal state, so goals were never progressed or
// completed and planning could not adapt to repeated failures.
//
// These tests assert the self-regulating behavior added by reflectionStep():
//   * action outcomes are captured (success flag + exit code),
//   * a satisfied active goal is marked completed and counted,
//   * the agent rotates to the next pending goal instead of looping,
//   * the new State goal-status mutation API behaves as documented,
//   * repeated failures escalate to a safe re-seeded awareness goal.
//
// They also exercise the State::updateGoalStatus / updateGoalStatusByDescription
// / countGoalsWithStatus surface directly so the additive core API is covered.

#include <gtest/gtest.h>
#include "elizaos/core.hpp"
#include "elizaos/autonomous_starter.hpp"

#include <string>

using namespace elizaos;

namespace {

AgentConfig makeConfig(const std::string& name = "Reflection-Agent") {
    AgentConfig cfg;
    cfg.agentId = generateUUID();
    cfg.agentName = name;
    cfg.bio = "Reflection/feedback autonomy test agent";
    cfg.lore = "Created to validate the closed-loop cognitive cycle";
    cfg.adjective = "introspective";
    return cfg;
}

// Count how many recent-memory entries contain a given substring.
std::size_t countMemoriesContaining(const State& state, const std::string& needle) {
    std::size_t count = 0;
    for (const auto& mem : state.getRecentMessages()) {
        if (mem && mem->getContent().find(needle) != std::string::npos) {
            ++count;
        }
    }
    return count;
}

} // namespace

// The cycle must run all four phases, each leaving a distinct memory trace.
TEST(AutonomyReflectionTest, FourPhaseCycleLeavesPerceptionReasoningActionReflectionMemories) {
    AutonomousStarter agent(makeConfig("FourPhase-Agent"));
    agent.start();

    agent.runCognitiveCycleOnce();

    const State& state = agent.getState();
    EXPECT_GE(countMemoriesContaining(state, "perception:"), 1u);
    EXPECT_GE(countMemoriesContaining(state, "reasoning:"), 1u);
    EXPECT_GE(countMemoriesContaining(state, "action:"), 1u);
    EXPECT_GE(countMemoriesContaining(state, "reflection:"), 1u);

    agent.stop();
}

// Reflection state must be populated after a cycle and reference the cycle.
TEST(AutonomyReflectionTest, ReflectionSummaryIsPopulatedAndCoherent) {
    AutonomousStarter agent(makeConfig("ReflectionSummary-Agent"));
    agent.start();

    EXPECT_TRUE(agent.getLastReflection().empty());
    agent.runCognitiveCycleOnce();

    const std::string reflection = agent.getLastReflection();
    EXPECT_FALSE(reflection.empty());
    EXPECT_NE(reflection.find("reflection:"), std::string::npos);
    EXPECT_NE(reflection.find("competence="), std::string::npos);
    EXPECT_NE(reflection.find("outcome="), std::string::npos);

    agent.stop();
}

// Competence must stay bounded in [0, 1] and rise with successful actions.
TEST(AutonomyReflectionTest, CompetenceSignalStaysBoundedAndRisesOnSuccess) {
    AutonomousStarter agent(makeConfig("Competence-Agent"));
    agent.start();

    const double initial = agent.getCompetenceSignal();
    EXPECT_GE(initial, 0.0);
    EXPECT_LE(initial, 1.0);

    // Safe default plans produce successful shell commands, so competence should
    // be non-decreasing across several cycles and remain bounded.
    double previous = initial;
    for (int i = 0; i < 6; ++i) {
        agent.runCognitiveCycleOnce();
        const double current = agent.getCompetenceSignal();
        EXPECT_GE(current, 0.0);
        EXPECT_LE(current, 1.0);
        EXPECT_GE(current + 1e-9, previous); // monotone non-decreasing on success
        previous = current;
    }

    // After repeated successful actions competence should have grown.
    EXPECT_GT(agent.getCompetenceSignal(), initial);
    EXPECT_GE(agent.getSuccessfulActionCount(), 1u);

    agent.stop();
}

// Successful-action accounting must match the number of cycles when all succeed.
TEST(AutonomyReflectionTest, SuccessfulActionCountTracksCycles) {
    AutonomousStarter agent(makeConfig("ActionCount-Agent"));
    agent.start();

    constexpr int kCycles = 4;
    for (int i = 0; i < kCycles; ++i) {
        agent.runCognitiveCycleOnce();
    }

    EXPECT_TRUE(agent.lastActionSucceeded());
    EXPECT_EQ(agent.getSuccessfulActionCount() + agent.getFailedActionCount(),
              static_cast<std::size_t>(kCycles));
    // Default safe commands succeed, so there should be no failures.
    EXPECT_EQ(agent.getFailedActionCount(), 0u);

    agent.stop();
}

// Attention-prioritized goal selection must return one of the seeded goals.
TEST(AutonomyReflectionTest, AttentionPrioritizedGoalMatchesASeededGoal) {
    AutonomousStarter agent(makeConfig("AttentionGoal-Agent"));
    agent.start();

    const auto& goals = agent.getState().getGoals();
    ASSERT_GE(goals.size(), 1u);

    const std::string prioritized = agent.getAttentionPrioritizedGoal();
    EXPECT_FALSE(prioritized.empty());

    bool matchesAGoal = false;
    for (const auto& goal : goals) {
        if (goal.description == prioritized) {
            matchesAGoal = true;
            break;
        }
    }
    EXPECT_TRUE(matchesAGoal);

    agent.stop();
}

// Attention prioritization must be deterministic for an unchanged goal set.
TEST(AutonomyReflectionTest, AttentionPrioritizationIsStableAcrossRepeatedCalls) {
    AutonomousStarter agent(makeConfig("AttentionStable-Agent"));
    agent.start();

    const std::string first = agent.getAttentionPrioritizedGoal();
    const std::string second = agent.getAttentionPrioritizedGoal();
    EXPECT_EQ(first, second);

    agent.stop();
}

// The reflection phase must keep producing fresh reflections each cycle.
TEST(AutonomyReflectionTest, EachCycleProducesAFreshReflectionTrace) {
    AutonomousStarter agent(makeConfig("FreshReflection-Agent"));
    agent.start();

    std::vector<std::string> reflections;
    for (int i = 0; i < 3; ++i) {
        agent.runCognitiveCycleOnce();
        reflections.push_back(agent.getLastReflection());
    }

    // Each reflection should reference its own cycle number, so they differ.
    for (std::size_t i = 0; i < reflections.size(); ++i) {
        EXPECT_FALSE(reflections[i].empty());
        const std::string marker = "Cycle " + std::to_string(i + 1) + " reflection:";
        EXPECT_NE(reflections[i].find(marker), std::string::npos)
            << "reflection " << i << " = " << reflections[i];
    }

    agent.stop();
}

// Reflection memories must accumulate one per cycle alongside the other phases.
TEST(AutonomyReflectionTest, ReflectionMemoriesAccumulatePerCycle) {
    AutonomousStarter agent(makeConfig("ReflectionAccrual-Agent"));
    agent.start();

    constexpr int kCycles = 3;
    for (int i = 0; i < kCycles; ++i) {
        agent.runCognitiveCycleOnce();
    }

    EXPECT_GE(countMemoriesContaining(agent.getState(), "reflection:"),
              static_cast<std::size_t>(kCycles));

    agent.stop();
}

// The cognitive cycle count must increase by exactly one per cycle even though
// the cycle now performs four phases.
TEST(AutonomyReflectionTest, CycleCountIncrementsOncePerFourPhaseCycle) {
    AutonomousStarter agent(makeConfig("CycleCount-Agent"));
    agent.start();

    EXPECT_EQ(agent.getCognitiveCycleCount(), 0u);
    agent.runCognitiveCycleOnce();
    EXPECT_EQ(agent.getCognitiveCycleCount(), 1u);
    agent.runCognitiveCycleOnce();
    EXPECT_EQ(agent.getCognitiveCycleCount(), 2u);

        agent.stop();
}

// ---------------------------------------------------------------------------
// State goal-status mutation API (additive core surface)
// ---------------------------------------------------------------------------

TEST(StateGoalMutationTest, UpdateGoalStatusById) {
    AgentConfig cfg = makeConfig("State-ById");
    State state(cfg);

    const Timestamp now = std::chrono::system_clock::now();
    const UUID id = generateUUID();
    state.addGoal(StateGoal{id, "Probe workspace", "active", now, now});

    EXPECT_TRUE(state.updateGoalStatus(id, "completed"));
    ASSERT_EQ(state.getGoals().size(), 1u);
    EXPECT_EQ(state.getGoals()[0].status, "completed");

    // Unknown id must not match.
    EXPECT_FALSE(state.updateGoalStatus(generateUUID(), "active"));
}

TEST(StateGoalMutationTest, UpdateGoalStatusByDescriptionIsCaseInsensitive) {
    AgentConfig cfg = makeConfig("State-ByDesc");
    State state(cfg);

    const Timestamp now = std::chrono::system_clock::now();
    state.addGoal(StateGoal{generateUUID(), "Inspect Source", "pending", now, now});

    // Match is case-insensitive on the description.
    EXPECT_TRUE(state.updateGoalStatusByDescription("inspect source", "active"));
    EXPECT_EQ(state.getGoals()[0].status, "active");

    EXPECT_FALSE(state.updateGoalStatusByDescription("nonexistent goal", "active"));
}

TEST(StateGoalMutationTest, CountGoalsWithStatus) {
    AgentConfig cfg = makeConfig("State-Count");
    State state(cfg);

    const Timestamp now = std::chrono::system_clock::now();
    state.addGoal(StateGoal{generateUUID(), "g1", "active", now, now});
    state.addGoal(StateGoal{generateUUID(), "g2", "pending", now, now});
    state.addGoal(StateGoal{generateUUID(), "g3", "Completed", now, now});
    state.addGoal(StateGoal{generateUUID(), "g4", "completed", now, now});

    EXPECT_EQ(state.countGoalsWithStatus("active"), 1u);
    EXPECT_EQ(state.countGoalsWithStatus("pending"), 1u);
    // Case-insensitive: "Completed" and "completed" both count.
    EXPECT_EQ(state.countGoalsWithStatus("completed"), 2u);
    EXPECT_EQ(state.countGoalsWithStatus("blocked"), 0u);
}

// ---------------------------------------------------------------------------
// Closed-loop reflection behavior on the autonomy cycle
// ---------------------------------------------------------------------------

class AutonomyReflectionFixtureTest : public ::testing::Test {
protected:
    AutonomyReflectionFixtureTest() : agent(makeConfig()) {}
    AutonomousStarter agent;
};

TEST_F(AutonomyReflectionFixtureTest, ActionOutcomeIsCaptured) {
    // A successful, safe probe must register as a captured action outcome.
    agent.enableShellAccess(true);
    const std::size_t cycles = agent.runCognitiveCycleOnce();

    EXPECT_GE(cycles, 1u);
    EXPECT_GE(agent.getActionCount(), 1u);
    // The default autonomy probes (pwd/ls/find) succeed in the test sandbox.
    EXPECT_TRUE(agent.getLastActionSucceeded());
    EXPECT_EQ(agent.getLastActionExitCode(), 0);
    EXPECT_FALSE(agent.getLastReflection().empty());
}

TEST_F(AutonomyReflectionFixtureTest, SuccessfulCycleProgressesGoals) {
    agent.enableShellAccess(true);

    // Before any cycle, no goals have been completed.
    EXPECT_EQ(agent.getCompletedGoalCount(), 0u);

    // Closed-loop convergence is evidence-driven, not one-shot: a goal is only
    // marked completed once its serving plan has produced reliable, repeated
    // evidence (a single successful probe is not proof of achievement -- treating
    // it as such was the focus-drift defect this suite now guards against). Run a
    // bounded window sufficient for the seeded goals to accrue that evidence.
    for (int i = 0; i < 6; ++i) {
        agent.runCognitiveCycleOnce();
    }

    // Having gathered reliable evidence, the loop closes: at least one goal has
    // advanced to completed and the completion counter reflects it.
    EXPECT_GE(agent.getCompletedGoalCount(), 1u);
    const auto& goals = agent.getState().getGoals();
    EXPECT_GE(agent.getState().countGoalsWithStatus("completed"), 1u);
    EXPECT_FALSE(goals.empty());
}

TEST_F(AutonomyReflectionFixtureTest, GoalsRotateAcrossCyclesInsteadOfLooping) {
    agent.enableShellAccess(true);

    // Run several cycles; the agent should progressively complete its seeded
    // autonomy goals rather than re-running one satisfied probe forever. Progress
    // is monotonically non-decreasing (a completed goal never reverts) and the
    // window is long enough for the evidence-gated loop to converge on at least
    // one goal without prematurely completing on a single probe.
    std::size_t lastCompleted = 0;
    for (int i = 0; i < 8; ++i) {
        agent.runCognitiveCycleOnce();
        EXPECT_GE(agent.getCompletedGoalCount(), lastCompleted);
        lastCompleted = agent.getCompletedGoalCount();
    }

    // Multiple cycles must have produced monotonically non-decreasing progress
    // and at least one completed goal overall.
    EXPECT_GE(agent.getCompletedGoalCount(), 1u);
    // The consecutive-failure counter stays at zero while probes succeed.
    EXPECT_EQ(agent.getConsecutiveActionFailures(), 0u);
}

TEST_F(AutonomyReflectionFixtureTest, RepeatedFailureEscalatesToSafeGoal) {
    // Disabling shell access makes every action fail, which must drive the
    // adaptive escalation path: consecutive failures accumulate and a safe
    // re-seeded awareness goal is added after the second failure.
    agent.enableShellAccess(false);

    agent.runCognitiveCycleOnce();
    EXPECT_FALSE(agent.getLastActionSucceeded());
    EXPECT_GE(agent.getConsecutiveActionFailures(), 1u);

    const std::size_t goalsAfterFirst = agent.getState().getGoals().size();

    agent.runCognitiveCycleOnce();
    EXPECT_GE(agent.getConsecutiveActionFailures(), 2u);

    // After two consecutive failures the reflection step re-seeds a safe
    // awareness goal, so the goal set grows and a "blocked" goal exists.
    const auto& goals = agent.getState().getGoals();
    EXPECT_GT(goals.size(), goalsAfterFirst);
    EXPECT_GE(agent.getState().countGoalsWithStatus("blocked"), 1u);

    // No goal should have been marked completed while every action failed.
    EXPECT_EQ(agent.getCompletedGoalCount(), 0u);
}

TEST_F(AutonomyReflectionFixtureTest, RecoveryResetsFailureCounter) {
    // Fail twice, then re-enable shell access; the next successful cycle must
    // reset the consecutive-failure counter, demonstrating self-recovery.
    agent.enableShellAccess(false);
    agent.runCognitiveCycleOnce();
    agent.runCognitiveCycleOnce();
    EXPECT_GE(agent.getConsecutiveActionFailures(), 2u);

    agent.enableShellAccess(true);
    agent.runCognitiveCycleOnce();
    EXPECT_TRUE(agent.getLastActionSucceeded());
    EXPECT_EQ(agent.getConsecutiveActionFailures(), 0u);
}
