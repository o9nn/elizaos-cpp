// test_closed_loop_autonomy.cpp
//
// Comprehensive end-to-end tests for the closed-loop goal-progression autonomy
// added to AutonomousStarter and the supporting State::updateGoalStatus API.
//
// These tests prove that goals are no longer a static decorative seed: the agent
// advances pending -> active -> completed based on real action evidence, never
// dead-ends (adaptive goal re-seeding), and escapes plan stagnation. Every
// assertion exercises the real compiled canonical API -- no mocks, no stubs.
#include <gtest/gtest.h>

#include "elizaos/core.hpp"
#include "elizaos/autonomous_starter.hpp"

#include <chrono>
#include <string>

using namespace elizaos;

namespace {

AgentConfig makeConfig(const std::string& name = "ClosedLoop-Agent") {
    AgentConfig cfg;
    cfg.agentId = generateUUID();
    cfg.agentName = name;
    cfg.bio = "Closed-loop goal-progression autonomy test agent";
    cfg.lore = "Created to validate converging goal-driven autonomy";
    cfg.adjective = "deliberate";
    return cfg;
}

} // namespace

// ---------------------------------------------------------------------------
// State::updateGoalStatus -- the minimal canonical surface that makes the goal
// center mutable and therefore alive.
// ---------------------------------------------------------------------------

TEST(StateGoalLifecycleTest, UpdateGoalStatusTransitionsExistingGoal) {
    State state(makeConfig());
    const Timestamp now = std::chrono::system_clock::now();
    const UUID goalId = generateUUID();
    state.addGoal(StateGoal{goalId, "Investigate workspace", "pending", now, now});

    ASSERT_EQ(state.getGoals().size(), 1u);
    EXPECT_EQ(state.getGoals().front().status, "pending");

    const Timestamp before = state.getGoals().front().updatedAt;
    const bool updated = state.updateGoalStatus(goalId, "active");

    EXPECT_TRUE(updated);
    EXPECT_EQ(state.getGoals().front().status, "active");
    // updatedAt must advance (monotonic, never regress).
    EXPECT_GE(state.getGoals().front().updatedAt, before);
}

TEST(StateGoalLifecycleTest, UpdateGoalStatusReturnsFalseForUnknownGoal) {
    State state(makeConfig());
    const Timestamp now = std::chrono::system_clock::now();
    state.addGoal(StateGoal{generateUUID(), "Known goal", "pending", now, now});

    EXPECT_FALSE(state.updateGoalStatus(generateUUID(), "completed"));
    // The existing goal must be untouched.
    EXPECT_EQ(state.getGoals().front().status, "pending");
}

TEST(StateGoalLifecycleTest, UpdateGoalStatusOnlyAffectsMatchingGoal) {
    State state(makeConfig());
    const Timestamp now = std::chrono::system_clock::now();
    const UUID first = generateUUID();
    const UUID second = generateUUID();
    state.addGoal(StateGoal{first, "First", "pending", now, now});
    state.addGoal(StateGoal{second, "Second", "pending", now, now});

    EXPECT_TRUE(state.updateGoalStatus(second, "completed"));
    EXPECT_EQ(state.getGoals()[0].status, "pending");
    EXPECT_EQ(state.getGoals()[1].status, "completed");
}

// ---------------------------------------------------------------------------
// AutonomousStarter closed-loop goal progression.
// ---------------------------------------------------------------------------

class ClosedLoopAutonomyTest : public ::testing::Test {
protected:
    void SetUp() override {
        agent_ = std::make_shared<AutonomousStarter>(makeConfig());
        agent_->start();
    }

    void TearDown() override {
        if (agent_) {
            agent_->stop();
        }
    }

    std::shared_ptr<AutonomousStarter> agent_;
};

TEST_F(ClosedLoopAutonomyTest, StartSeedsCoreGoalsWithOpenWork) {
    // start() must seed core autonomy goals so the agent has something to drive.
    EXPECT_GE(agent_->getState().getGoals().size(), 1u);
    EXPECT_GE(agent_->getOpenGoalCount(), 1u);
    EXPECT_EQ(agent_->getCompletedGoalCount(), 0u);
}

TEST_F(ClosedLoopAutonomyTest, FirstCycleSelectsAndPursuesAnActiveGoal) {
    agent_->runCognitiveCycleOnce();
    // After a cycle the agent must have pursued a concrete goal: a plan was
    // formed and at least one action executed. The active goal id may already be
    // cleared if the goal completed within the cycle (the convergent fast path),
    // so success is evidenced by progress, not by a still-pending pursuit.
    EXPECT_FALSE(agent_->getLastPlan().empty());
    EXPECT_GE(agent_->getActionCount(), 1u);
    // Either a goal is still being pursued, or at least one goal has completed.
    EXPECT_TRUE(!agent_->getActiveGoalId().empty() ||
                agent_->getCompletedGoalCount() >= 1u);
}

TEST_F(ClosedLoopAutonomyTest, GoalsConvergeToCompletionOverMultipleCycles) {
    const std::size_t initialOpen = agent_->getOpenGoalCount();
    ASSERT_GE(initialOpen, 1u);

    // Run enough bounded cycles to drive the seeded goals to completion.
    for (int i = 0; i < 12; ++i) {
        agent_->runCognitiveCycleOnce();
    }

    // The agent must have completed at least the originally seeded goals -- this
    // is the core proof that goal state is closed-loop, not open-loop.
    EXPECT_GE(agent_->getCompletedGoalCount(), initialOpen);
    EXPECT_EQ(agent_->getCognitiveCycleCount(), 12u);
}

TEST_F(ClosedLoopAutonomyTest, AutonomyNeverDeadEndsOnOpenGoals) {
    // After many cycles there must always remain at least one open goal to
    // pursue (adaptive re-seeding), so the agent never idles with nothing to do.
    for (int i = 0; i < 15; ++i) {
        agent_->runCognitiveCycleOnce();
    }
    EXPECT_GE(agent_->getOpenGoalCount(), 1u);
}

TEST_F(ClosedLoopAutonomyTest, AdaptiveGoalsAreSeededAfterCompletion) {
    const std::size_t seededAtStart = agent_->getState().getGoals().size();
    for (int i = 0; i < 15; ++i) {
        agent_->runCognitiveCycleOnce();
    }
    // Completing the seed goals should have grown the goal list with adaptive
    // exploration goals.
    EXPECT_GT(agent_->getState().getGoals().size(), seededAtStart);
}

TEST_F(ClosedLoopAutonomyTest, CompletedGoalsCarryEvidenceInMemory) {
    for (int i = 0; i < 12; ++i) {
        agent_->runCognitiveCycleOnce();
    }
    // The memory stream must record at least one explicit goal completion,
    // proving the loop closed on observed evidence rather than a timer.
    bool foundCompletion = false;
    for (const auto& memory : agent_->getState().getRecentMessages()) {
        if (memory && memory->getContent().find("Goal completed:") != std::string::npos) {
            foundCompletion = true;
            break;
        }
    }
    EXPECT_TRUE(foundCompletion);
}

TEST_F(ClosedLoopAutonomyTest, StagnationGuardKeepsStagnationBounded) {
    // Across many cycles the stagnation counter must stay bounded (the guard
    // resets it on escalation), proving the agent cannot lock into one plan.
    for (int i = 0; i < 20; ++i) {
        agent_->runCognitiveCycleOnce();
        EXPECT_LE(agent_->getStagnationCounter(), 2u);
    }
}

TEST_F(ClosedLoopAutonomyTest, ActiveGoalIsAlwaysAValidOpenGoalDuringPursuit) {
    for (int i = 0; i < 8; ++i) {
        agent_->runCognitiveCycleOnce();
        const UUID active = agent_->getActiveGoalId();
        if (active.empty()) {
            continue;  // transient between completion and next promotion
        }
        bool matchesOpenGoal = false;
        for (const auto& goal : agent_->getState().getGoals()) {
            if (goal.id == active) {
                EXPECT_NE(goal.status, "completed");
                matchesOpenGoal = true;
                break;
            }
        }
        EXPECT_TRUE(matchesOpenGoal);
    }
}

TEST_F(ClosedLoopAutonomyTest, ProgressIsDeterministicAcrossIdenticalRuns) {
    // Two fresh agents driven the same number of cycles must reach the same
    // completed-goal count -- progression is evidence-driven and deterministic.
    auto secondAgent = std::make_shared<AutonomousStarter>(makeConfig("ClosedLoop-Agent-2"));
    secondAgent->start();
    for (int i = 0; i < 10; ++i) {
        agent_->runCognitiveCycleOnce();
        secondAgent->runCognitiveCycleOnce();
    }
    EXPECT_EQ(agent_->getCompletedGoalCount(), secondAgent->getCompletedGoalCount());
    secondAgent->stop();
}

// ---------------------------------------------------------------------------
// Never-dead-end invariant regression coverage.
//
// The autonomy engine has TWO evidence-gated goal-completion paths:
//   * evaluateGoalProgress() (action phase) completes the active goal and
//     reseeds inline, and
//   * advanceGoalLifecycle() (reflection phase) completes the focused goal.
// A prior defect left the reflection path with no post-completion reseed, so
// when it retired the final open goal the cycle ended with zero open goals --
// a cognitively dead agent. The tests below lock in the post-cycle safety net
// so the invariant holds after EVERY cycle regardless of which path fires.
// ---------------------------------------------------------------------------

TEST_F(ClosedLoopAutonomyTest, OpenGoalCountNeverReachesZeroAtAnyCycleBoundary) {
    // The strong form of the invariant: it is not enough that open goals exist
    // "eventually"; there must be at least one open goal at the end of EVERY
    // single cycle, so the next perception step always has something to pursue.
    for (int i = 0; i < 30; ++i) {
        agent_->runCognitiveCycleOnce();
        EXPECT_GE(agent_->getOpenGoalCount(), 1u)
            << "agent dead-ended (zero open goals) at cycle boundary " << (i + 1);
    }
}

TEST_F(ClosedLoopAutonomyTest, SustainedRunKeepsCompletingGoalsWithoutDeadEnd) {
    // Over a long run the agent must keep making progress (completing goals) AND
    // never dead-end -- proving the reseed keeps genuinely new open work flowing
    // rather than parking on a single stale goal.
    for (int i = 0; i < 40; ++i) {
        agent_->runCognitiveCycleOnce();
    }
    EXPECT_GE(agent_->getCompletedGoalCount(), 3u);
    EXPECT_GE(agent_->getOpenGoalCount(), 1u);
    // The goal list must have grown well beyond the two seed goals via adaptive
    // re-seeding, confirming the never-dead-end drive is generative.
    EXPECT_GT(agent_->getState().getGoals().size(), 2u);
}

TEST_F(ClosedLoopAutonomyTest, DeterministicNoDeadEndAcrossIdenticalLongRuns) {
    // Two independent agents driven identically must agree on both progress and
    // the still-open invariant, proving the reseed is deterministic (not a race).
    auto other = std::make_shared<AutonomousStarter>(makeConfig("ClosedLoop-Agent-ND"));
    other->start();
    for (int i = 0; i < 25; ++i) {
        agent_->runCognitiveCycleOnce();
        other->runCognitiveCycleOnce();
        EXPECT_GE(agent_->getOpenGoalCount(), 1u);
        EXPECT_GE(other->getOpenGoalCount(), 1u);
    }
    EXPECT_EQ(agent_->getCompletedGoalCount(), other->getCompletedGoalCount());
    other->stop();
}
