// test_autonomy_evolution.cpp
//
// Comprehensive end-to-end coverage for the KSM autonomy-evolution upgrade to
// the AutonomousStarter cognitive loop. These tests validate the newly added
// self-regulating behaviors that turn the previous observe->reason->act stub
// into a closed-loop perceive->reason->act->reflect autonomy engine:
//
//   1. Reflection phase actually runs and produces a non-empty conclusion every
//      cognitive cycle (reflectionCount tracks cycle count).
//   2. Attention-weighted goal selection focuses on an open goal and exposes a
//      stable focused-goal id.
//   3. Plan-success feedback accumulates real statistics that bias future plan
//      selection (getPlanSuccessRatio is well-defined and bounded).
//   4. Goal lifecycle transitions advance pending -> active -> completed when
//      the agent reliably succeeds at its plans.
//   5. The State::updateGoalStatus lifecycle-mutation API behaves correctly in
//      isolation (found/updated vs. not-found, timestamp refresh).
//   6. The full loop remains coherent and self-consistent across many cycles.
//
// Every assertion exercises real implementation behavior; there are no mocks.

#include <gtest/gtest.h>

#include "elizaos/goal_manager.hpp"  // MUST precede autonomous_starter.hpp
#include "elizaos/autonomous_starter.hpp"
#include "elizaos/core.hpp"

#include <chrono>
#include <string>
#include <thread>
#include <vector>

using namespace elizaos;
using namespace std::chrono_literals;

namespace {

AgentConfig makeConfig(const std::string& name) {
    AgentConfig cfg;
    cfg.agentId = generateUUID();
    cfg.agentName = name;
    cfg.bio = "Autonomy-evolution E2E agent";
    cfg.lore = "Created to validate the self-regulating cognitive loop";
    cfg.adjective = "reflective";
    return cfg;
}

}  // namespace

// ---------------------------------------------------------------------------
// State::updateGoalStatus - lifecycle mutation API (structure-preserving)
// ---------------------------------------------------------------------------

class StateGoalLifecycleTest : public ::testing::Test {};

TEST_F(StateGoalLifecycleTest, UpdatesStatusOfExistingGoal) {
    State state(makeConfig("State-Goal"));
    const Timestamp now = std::chrono::system_clock::now();
    StateGoal goal{generateUUID(), "investigate workspace", "pending", now, now};
    state.addGoal(goal);

    ASSERT_EQ(state.getGoals().size(), 1u);
    EXPECT_EQ(state.getGoals()[0].status, "pending");

    const bool updated = state.updateGoalStatus(goal.id, "active");
    EXPECT_TRUE(updated);
    EXPECT_EQ(state.getGoals()[0].status, "active");
}

TEST_F(StateGoalLifecycleTest, ReturnsFalseForUnknownGoal) {
    State state(makeConfig("State-Goal-Missing"));
    const Timestamp now = std::chrono::system_clock::now();
    state.addGoal(StateGoal{generateUUID(), "g", "pending", now, now});

    EXPECT_FALSE(state.updateGoalStatus("nonexistent-id", "completed"));
    // Existing goal must remain untouched.
    EXPECT_EQ(state.getGoals()[0].status, "pending");
}

TEST_F(StateGoalLifecycleTest, RefreshesUpdatedTimestamp) {
    State state(makeConfig("State-Goal-Timestamp"));
    const Timestamp past =
        std::chrono::system_clock::now() - std::chrono::hours(1);
    StateGoal goal{generateUUID(), "g", "active", past, past};
    state.addGoal(goal);

    EXPECT_TRUE(state.updateGoalStatus(goal.id, "completed"));
    EXPECT_TRUE(state.getGoals()[0].updatedAt > past);
    EXPECT_EQ(state.getGoals()[0].status, "completed");
}

// ---------------------------------------------------------------------------
// Reflection phase
// ---------------------------------------------------------------------------

class ReflectionLoopTest : public ::testing::Test {};

TEST_F(ReflectionLoopTest, ReflectionRunsEveryCycle) {
    AutonomousStarter agent(makeConfig("Reflective-Agent"));
    agent.start();

    EXPECT_EQ(agent.getReflectionCount(), 0u);

    for (std::size_t i = 1; i <= 4; ++i) {
        agent.runCognitiveCycleOnce();
        // One reflection per cognitive cycle.
        EXPECT_EQ(agent.getReflectionCount(), i);
        // Reflection conclusion must be populated and reference a plan.
        EXPECT_FALSE(agent.getLastReflection().empty());
        EXPECT_NE(agent.getLastReflection().find("reflection"), std::string::npos);
    }

    agent.stop();
}

TEST_F(ReflectionLoopTest, ReflectionRecordsActionOutcome) {
    AutonomousStarter agent(makeConfig("Outcome-Agent"));
    agent.start();
    agent.runCognitiveCycleOnce();

    // The reflection text must report either success or failure of the action,
    // and the boolean accessor must agree with the recorded reflection.
    const std::string reflection = agent.getLastReflection();
    const bool succeeded = agent.getLastActionSucceeded();
    if (succeeded) {
        EXPECT_NE(reflection.find("succeeded"), std::string::npos);
    } else {
        EXPECT_NE(reflection.find("failed"), std::string::npos);
    }

    agent.stop();
}

// ---------------------------------------------------------------------------
// Attention-weighted goal selection
// ---------------------------------------------------------------------------

class AttentionFocusTest : public ::testing::Test {};

TEST_F(AttentionFocusTest, FocusesOnAnOpenGoal) {
    AutonomousStarter agent(makeConfig("Focus-Agent"));
    agent.start();

    // Before reasoning there is no committed focus.
    EXPECT_TRUE(agent.getFocusedGoalId().empty());

    agent.runCognitiveCycleOnce();

    // After a cycle the agent must have committed to a concrete goal id, and
    // that id must correspond to one of the seeded goals.
    const UUID focused = agent.getFocusedGoalId();
    EXPECT_FALSE(focused.empty());

    bool matchesSeededGoal = false;
    for (const auto& goal : agent.getState().getGoals()) {
        if (goal.id == focused) {
            matchesSeededGoal = true;
            break;
        }
    }
    EXPECT_TRUE(matchesSeededGoal);

    agent.stop();
}

TEST_F(AttentionFocusTest, FocusRemainsValidAcrossCycles) {
    AutonomousStarter agent(makeConfig("Focus-Stability"));
    agent.start();

    for (int i = 0; i < 6; ++i) {
        agent.runCognitiveCycleOnce();
        const UUID focused = agent.getFocusedGoalId();
        EXPECT_FALSE(focused.empty());
        bool valid = false;
        for (const auto& goal : agent.getState().getGoals()) {
            if (goal.id == focused) {
                valid = true;
                break;
            }
        }
        EXPECT_TRUE(valid) << "focused goal id must always map to a real goal";
    }

    agent.stop();
}

// ---------------------------------------------------------------------------
// Plan-success feedback learning
// ---------------------------------------------------------------------------

class PlanFeedbackTest : public ::testing::Test {};

TEST_F(PlanFeedbackTest, PlanSuccessRatioIsBounded) {
    AutonomousStarter agent(makeConfig("Feedback-Agent"));
    agent.start();

    // Run enough cycles to accumulate plan statistics.
    for (int i = 0; i < 8; ++i) {
        agent.runCognitiveCycleOnce();
    }

    // The plan the agent most recently committed to must have a well-defined,
    // bounded success ratio in [0,1].
    const std::string plan = agent.getLastPlan();
    EXPECT_FALSE(plan.empty());
    const double ratio = agent.getPlanSuccessRatio(plan);
    EXPECT_GE(ratio, 0.0);
    EXPECT_LE(ratio, 1.0);

    // Unknown plans must report 0.0 rather than crash.
    EXPECT_DOUBLE_EQ(agent.getPlanSuccessRatio("a plan never executed"), 0.0);

    agent.stop();
}

TEST_F(PlanFeedbackTest, SuccessfulPlansAccumulatePositiveRatio) {
    AutonomousStarter agent(makeConfig("Feedback-Positive"));
    agent.start();

    // Safe filesystem-introspection plans should generally succeed in the
    // sandbox, so after several cycles at least one executed plan must have a
    // strictly positive success ratio (proving feedback is actually recorded).
    std::vector<std::string> seenPlans;
    for (int i = 0; i < 10; ++i) {
        agent.runCognitiveCycleOnce();
        seenPlans.push_back(agent.getLastPlan());
    }

    bool anyPositive = false;
    for (const auto& plan : seenPlans) {
        if (agent.getPlanSuccessRatio(plan) > 0.0) {
            anyPositive = true;
            break;
        }
    }
    EXPECT_TRUE(anyPositive)
        << "feedback learning must record at least one successful plan outcome";

    agent.stop();
}

// ---------------------------------------------------------------------------
// Goal lifecycle progression driven by autonomy
// ---------------------------------------------------------------------------

class GoalLifecycleProgressionTest : public ::testing::Test {};

TEST_F(GoalLifecycleProgressionTest, GoalsProgressTowardCompletion) {
    AutonomousStarter agent(makeConfig("Lifecycle-Agent"));
    agent.start();

    auto countByStatus = [&](const std::string& status) {
        std::size_t n = 0;
        for (const auto& goal : agent.getState().getGoals()) {
            std::string s = goal.status;
            for (auto& c : s) c = static_cast<char>(::tolower(c));
            if (s == status) ++n;
        }
        return n;
    };

    const std::size_t initialPending = countByStatus("pending");
    EXPECT_GE(agent.getState().getGoals().size(), 1u);

    // Drive many cycles so the agent can transition goals through their
    // lifecycle as plans prove reliable.
    for (int i = 0; i < 20; ++i) {
        agent.runCognitiveCycleOnce();
    }

    // The autonomy loop must make measurable progress: either some pending goal
    // became active/completed, or a goal reached the completed state.
    const std::size_t finalPending = countByStatus("pending");
    const std::size_t finalCompleted = countByStatus("completed");
    const std::size_t finalActive = countByStatus("active");

    EXPECT_TRUE(finalPending < initialPending || finalCompleted > 0 ||
                finalActive > 0)
        << "goal lifecycle must advance under sustained autonomy";

    agent.stop();
}

// ---------------------------------------------------------------------------
// Full-loop coherence
// ---------------------------------------------------------------------------

class FullLoopCoherenceTest : public ::testing::Test {};

TEST_F(FullLoopCoherenceTest, AllFourPhasesStayConsistent) {
    AutonomousStarter agent(makeConfig("Coherence-E2E"));
    agent.start();

    const int kCycles = 12;
    for (int i = 0; i < kCycles; ++i) {
        agent.runCognitiveCycleOnce();

        // Each cycle must leave the agent in a fully consistent state.
        EXPECT_FALSE(agent.getLastObservationSummary().empty());
        EXPECT_FALSE(agent.getLastPlan().empty());
        EXPECT_FALSE(agent.getLastReflection().empty());
    }

    // Cycle, action, and reflection counters must all advance in lock-step:
    // exactly one perception, one action, and one reflection per cycle.
    EXPECT_EQ(agent.getCognitiveCycleCount(),
              static_cast<std::size_t>(kCycles));
    EXPECT_EQ(agent.getActionCount(), static_cast<std::size_t>(kCycles));
    EXPECT_EQ(agent.getReflectionCount(), static_cast<std::size_t>(kCycles));

    // Memory must accumulate strictly. Reasoning, action, and reflection each
    // append at least one named cycle memory, so the recent-message stream must
    // grow by at least two entries per cycle (a conservative lower bound that
    // holds even if individual shell observations are coalesced).
    EXPECT_GE(agent.getState().getRecentMessages().size(),
              static_cast<std::size_t>(kCycles) * 2);

    agent.stop();
}

TEST_F(FullLoopCoherenceTest, SelfCheckStillPassesAfterEvolution) {
    // The optimized loop must not regress the package self-check.
    EXPECT_TRUE(autonomous_starter_self_check());
}
