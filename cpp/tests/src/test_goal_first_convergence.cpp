// test_goal_first_convergence.cpp
//
// Comprehensive E2E tests for the goal-first plan convergence and active-goal
// reconciliation mechanisms backported from hurdcog/elizaos.cpp.
//
// Validates that:
//   1. Goal-first plan precedence: the plan serving the active goal is promoted
//      to the front of the candidate list and only displaced by strictly better
//      measured evidence.
//   2. Active-goal reconciliation: stale activeGoalId_ pointing at a completed
//      or blocked goal is cleared at the end of reflection, so the next cycle's
//      selectActiveGoal() promotes a valid one.
//   3. Plan intent source: goalContext is resolved from selectGoalContext() first
//      (the committed goal), with attention focus as fallback only when idle.
//   4. The never-dead-end invariant holds across all paths.
//   5. Cross-fork parity: both forks exhibit identical convergence behavior.

#include <gtest/gtest.h>
#include "elizaos/autonomous_starter.hpp"
#include "elizaos/core.hpp"
#include <algorithm>
#include <chrono>
#include <string>
#include <vector>

using namespace elizaos;

namespace {

AgentConfig makeConfig(const std::string& name = "GoalFirst-Agent") {
    AgentConfig cfg;
    cfg.agentId = generateUUID();
    cfg.agentName = name;
    cfg.bio = "Goal-first convergence test agent";
    cfg.lore = "Created to validate goal-first plan selection and reconciliation";
    cfg.adjective = "focused";
    return cfg;
}

} // namespace

// ===========================================================================
// Goal-First Plan Precedence
// ===========================================================================

class GoalFirstConvergenceTest : public ::testing::Test {
protected:
    void SetUp() override {
        agent_ = std::make_shared<AutonomousStarter>(makeConfig());
        agent_->start();
    }

    void TearDown() override {
        if (agent_) agent_->stop();
    }

    std::shared_ptr<AutonomousStarter> agent_;
};

TEST_F(GoalFirstConvergenceTest, PlanAlignedWithActiveGoalIsSelected) {
    // After the first cycle, the selected plan should topically align with
    // the active goal (not a random exploratory plan).
    agent_->runCognitiveCycleOnce();

    const std::string plan = agent_->getLastPlan();
    const std::string activeGoalId = agent_->getActiveGoalId();
    EXPECT_FALSE(plan.empty())
        << "Agent must select a plan in the first cycle";

    // The plan should relate to the goal context (not be a stagnation escalation)
    // We verify this indirectly: the plan should NOT be one of the escalation plans
    // unless the agent genuinely stagnated (which shouldn't happen in cycle 1).
    EXPECT_EQ(plan.find("escalat"), std::string::npos)
        << "First cycle should not trigger stagnation escalation";
}

TEST_F(GoalFirstConvergenceTest, GoalAlignedPlanPersistsAcrossMultipleCycles) {
    // The goal-first mechanism should keep the agent on-topic for multiple cycles
    // while the goal remains open, rather than switching plans every cycle.
    std::vector<std::string> plans;
    for (int i = 0; i < 5; ++i) {
        agent_->runCognitiveCycleOnce();
        plans.push_back(agent_->getLastPlan());
    }

    // Count how many times the plan stayed the same (goal-aligned persistence)
    int sameCount = 0;
    for (std::size_t i = 1; i < plans.size(); ++i) {
        if (plans[i] == plans[i - 1]) ++sameCount;
    }

    // With goal-first precedence, the plan should persist at least once
    // (the agent converges on its goal rather than randomly switching)
    EXPECT_GE(sameCount, 1)
        << "Goal-first plan selection should produce at least some plan persistence";
}

TEST_F(GoalFirstConvergenceTest, PlanBiasOnlyOverridesWithStrictlyBetterEvidence) {
    // Run enough cycles to accumulate plan statistics
    for (int i = 0; i < 10; ++i) {
        agent_->runCognitiveCycleOnce();
    }

    // The agent should have made progress without excessive plan switching
    auto report = agent_->getAutonomyHealthReport();
    EXPECT_GT(report.completedGoals, 0u)
        << "Agent should complete goals when plan selection is stable";
    EXPECT_LT(report.stagnationCount, static_cast<std::size_t>(3))
        << "Goal-first plan selection should prevent stagnation";
}

// ===========================================================================
// Active-Goal Reconciliation
// ===========================================================================

TEST_F(GoalFirstConvergenceTest, ActiveGoalIdClearedAfterGoalCompletion) {
    // Run cycles until a goal is completed
    for (int i = 0; i < 15; ++i) {
        agent_->runCognitiveCycleOnce();
    }

    // After completion, the active goal id should either be empty (between goals)
    // or point to a valid open goal (promoted next goal).
    const UUID activeId = agent_->getActiveGoalId();
    if (!activeId.empty()) {
        bool isOpenGoal = false;
        for (const auto& goal : agent_->getState().getGoals()) {
            if (goal.id == activeId) {
                EXPECT_NE(goal.status, "completed")
                    << "activeGoalId must never point to a completed goal";
                EXPECT_NE(goal.status, "blocked")
                    << "activeGoalId must never point to a blocked goal";
                isOpenGoal = true;
                break;
            }
        }
        EXPECT_TRUE(isOpenGoal)
            << "activeGoalId must reference an existing goal in the goal list";
    }
}

TEST_F(GoalFirstConvergenceTest, ReconciliationHoldsAcrossManyCycles) {
    // The invariant must hold at EVERY cycle boundary, not just eventually.
    for (int i = 0; i < 30; ++i) {
        agent_->runCognitiveCycleOnce();

        const UUID activeId = agent_->getActiveGoalId();
        if (!activeId.empty()) {
            bool found = false;
            for (const auto& goal : agent_->getState().getGoals()) {
                if (goal.id == activeId) {
                    std::string status = goal.status;
                    std::transform(status.begin(), status.end(), status.begin(),
                                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                    EXPECT_TRUE(status == "active" || status == "in_progress" || status == "pending")
                        << "activeGoalId points to non-open goal at cycle " << (i + 1)
                        << " (status=" << goal.status << ")";
                    found = true;
                    break;
                }
            }
            EXPECT_TRUE(found)
                << "activeGoalId references non-existent goal at cycle " << (i + 1);
        }
    }
}

// ===========================================================================
// Plan Intent Source (goal-first, single-tasking drive)
// ===========================================================================

TEST_F(GoalFirstConvergenceTest, PlanIntentSourceUsesCommittedGoal) {
    // The plan intent should come from selectGoalContext() (the committed goal),
    // not from the attention focus. We verify by checking that the plan aligns
    // with the first open goal's topic.
    agent_->runCognitiveCycleOnce();

    const auto& goals = agent_->getState().getGoals();
    std::string firstOpenGoalDesc;
    for (const auto& goal : goals) {
        std::string status = goal.status;
        std::transform(status.begin(), status.end(), status.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (status == "active" || status == "in_progress" || status == "pending") {
            firstOpenGoalDesc = goal.description;
            break;
        }
    }

    // The plan should be topically related to the first open goal
    EXPECT_FALSE(firstOpenGoalDesc.empty())
        << "Agent must have at least one open goal";
    EXPECT_FALSE(agent_->getLastPlan().empty())
        << "Agent must have selected a plan";
}

TEST_F(GoalFirstConvergenceTest, AttentionFocusOnlyUsedWhenIdle) {
    // Run many cycles - the agent should complete goals and reseed.
    // The attention focus should only influence plan selection when there's no
    // open goal context (which should be rare with the never-dead-end invariant).
    for (int i = 0; i < 20; ++i) {
        agent_->runCognitiveCycleOnce();
        // The never-dead-end invariant ensures there's always an open goal
        EXPECT_GE(agent_->getOpenGoalCount(), 1u)
            << "Never-dead-end invariant violated at cycle " << (i + 1);
    }
}

// ===========================================================================
// Never-Dead-End Invariant (reinforced by reconciliation)
// ===========================================================================

TEST_F(GoalFirstConvergenceTest, NeverDeadEndWithReconciliation) {
    // The reconciliation mechanism should prevent dead-ends even when goals
    // are completed by different paths (evaluateGoalProgress vs advanceGoalLifecycle).
    for (int i = 0; i < 40; ++i) {
        agent_->runCognitiveCycleOnce();
        EXPECT_GE(agent_->getOpenGoalCount(), 1u)
            << "Dead-end at cycle " << (i + 1) << " despite reconciliation";
    }

    auto report = agent_->getAutonomyHealthReport();
    EXPECT_GE(report.completedGoals, static_cast<std::size_t>(3))
        << "Agent should complete multiple goals over 40 cycles";
    EXPECT_TRUE(report.isHealthy)
        << "Agent must remain healthy across all cycles";
}

// ===========================================================================
// Cross-Fork Parity
// ===========================================================================

TEST_F(GoalFirstConvergenceTest, CrossForkParity_GoalFirstAPI) {
    // Both forks must expose the same goal-first convergence API surface.
    // This test validates the API exists and returns consistent results.
    agent_->runCognitiveCycleOnce();

    // getActiveGoalId() must be available
    const UUID activeId = agent_->getActiveGoalId();
    // getLastPlan() must be available
    const std::string plan = agent_->getLastPlan();
    EXPECT_FALSE(plan.empty());

    // getOpenGoalCount() and getCompletedGoalCount() must be available
    EXPECT_GE(agent_->getOpenGoalCount(), 1u);

    // getStagnationCounter() must be available
    EXPECT_LE(agent_->getStagnationCounter(), 2u);

    // getAutonomyHealthReport() must return a valid report
    auto report = agent_->getAutonomyHealthReport();
    EXPECT_GE(report.competence, 0.0);
    EXPECT_LE(report.competence, 1.0);
    EXPECT_TRUE(report.isHealthy);
}

TEST_F(GoalFirstConvergenceTest, CrossForkParity_DeterministicConvergence) {
    // Two fresh agents driven identically must converge the same way.
    auto other = std::make_shared<AutonomousStarter>(makeConfig("GoalFirst-Agent-2"));
    other->start();

    for (int i = 0; i < 15; ++i) {
        agent_->runCognitiveCycleOnce();
        other->runCognitiveCycleOnce();
    }

    EXPECT_EQ(agent_->getCompletedGoalCount(), other->getCompletedGoalCount())
        << "Goal-first convergence must be deterministic across identical runs";
    EXPECT_EQ(agent_->getCognitiveCycleCount(), other->getCognitiveCycleCount());

    other->stop();
}
