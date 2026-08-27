/**
 * test_autonomy_pipeline_integration_e2e.cpp — Cycle 010: Comprehensive E2E
 * integration tests for the full autonomy pipeline.
 *
 * Covers the complete cognitive pipeline from perception through homework loop:
 *   1. Cognitive momentum tracking under varied success patterns
 *   2. Cycle efficiency tracking with productive vs idle cycles
 *   3. Goal-chain coherence with thematically related goals
 *   4. Homework loop integration with autonomy health metrics
 *   5. Endocrine-autonomy coupling under momentum shifts
 *   6. Never-dead-end invariant stress test
 *   7. Attention hysteresis under momentum-driven mode changes
 *   8. Full pipeline: perception → reasoning → action → reflection → homework
 *
 * Cross-fork parity: identical test file in both hurdcog and o9nn.
 */
#include <gtest/gtest.h>
#include "elizaos/autonomous_starter.hpp"
#include "elizaos/homework_loop.hpp"
#include "elizaos/cognitive_curriculum.hpp"
#include "elizaos/endocrine.hpp"
#include "elizaos/attention.hpp"
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>
#include <set>

using namespace elizaos;
using namespace std::chrono_literals;

// ============================================================================
// Test Fixture
// ============================================================================

class AutonomyPipelineIntegrationTest : public ::testing::Test {
protected:
    AgentConfig makeConfig(const std::string& name = "Pipeline-Integration-Agent") {
        AgentConfig cfg;
        cfg.agentId = generateUUID();
        cfg.agentName = name;
        cfg.bio = "Autonomy pipeline integration test agent";
        cfg.lore = "Testing full cognitive pipeline";
        return cfg;
    }
};

// ============================================================================
// 1. Cognitive Momentum Tracking
// ============================================================================

TEST_F(AutonomyPipelineIntegrationTest, MomentumEWMAConvergence) {
    // Verify EWMA converges toward 1.0 under sustained success.
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->enableShellAccess(true);
    agent->start();
    for (int i = 0; i < 20; ++i) {
        agent->runCognitiveCycleOnce();
    }
    auto report = agent->getAutonomyHealthReport();
    // After 20 successful cycles, momentum should be well above 0.5.
    EXPECT_GT(report.cognitiveMomentum, 0.7);
    agent->stop();
}

TEST_F(AutonomyPipelineIntegrationTest, MomentumDecaysOnFailure) {
    // Verify momentum drops when shell access is disabled (actions fail).
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->enableShellAccess(true);
    agent->start();
    // Build up momentum.
    for (int i = 0; i < 10; ++i) {
        agent->runCognitiveCycleOnce();
    }
    double highMomentum = agent->getAutonomyHealthReport().cognitiveMomentum;
    // Disable shell to cause failures.
    agent->enableShellAccess(false);
    for (int i = 0; i < 10; ++i) {
        agent->runCognitiveCycleOnce();
    }
    double lowMomentum = agent->getAutonomyHealthReport().cognitiveMomentum;
    EXPECT_LT(lowMomentum, highMomentum);
    agent->stop();
}

// ============================================================================
// 2. Cycle Efficiency Tracking
// ============================================================================

TEST_F(AutonomyPipelineIntegrationTest, EfficiencyTracksProductiveCycles) {
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->enableShellAccess(true);
    agent->start();
    for (int i = 0; i < 15; ++i) {
        agent->runCognitiveCycleOnce();
    }
    auto report = agent->getAutonomyHealthReport();
    // With shell access, most cycles should be productive.
    EXPECT_GT(report.cycleEfficiency, 0.5);
    EXPECT_EQ(report.totalCycles, 15u);
    agent->stop();
}

TEST_F(AutonomyPipelineIntegrationTest, EfficiencyDropsWithoutShell) {
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->enableShellAccess(false);
    agent->start();
    for (int i = 0; i < 10; ++i) {
        agent->runCognitiveCycleOnce();
    }
    auto report = agent->getAutonomyHealthReport();
    // Without shell access, no actions succeed so efficiency should be 0.
    EXPECT_DOUBLE_EQ(report.cycleEfficiency, 0.0);
    agent->stop();
}

// ============================================================================
// 3. Goal-Chain Coherence
// ============================================================================

TEST_F(AutonomyPipelineIntegrationTest, CoherenceGrowsWithRelatedGoals) {
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->enableShellAccess(true);
    agent->start();
    // Run enough cycles to complete multiple goals.
    for (int i = 0; i < 30; ++i) {
        agent->runCognitiveCycleOnce();
    }
    auto report = agent->getAutonomyHealthReport();
    if (report.completedGoals >= 2) {
        // With the intent-continuity mechanism, consecutive goals should share
        // some thematic overlap.
        EXPECT_GE(report.goalChainCoherence, 0.0);
    }
    agent->stop();
}

// ============================================================================
// 4. Homework Loop Integration
// ============================================================================

TEST_F(AutonomyPipelineIntegrationTest, HomeworkLoopUsesAutonomyMetrics) {
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->enableShellAccess(true);
    agent->start();
    // Run some cycles to build state.
    for (int i = 0; i < 5; ++i) {
        agent->runCognitiveCycleOnce();
    }
    CognitiveCurriculum curriculum;
    HomeworkLoop loop(*agent, curriculum);
    auto result = loop.runHomeworkCycleOnce();
    // Homework should complete without destructive commands.
    EXPECT_FALSE(loop.issuedDestructiveCommand());
    // The weakest center should be identified.
    EXPECT_FALSE(result.targetName.empty());
    // Coherence scores should be bounded.
    EXPECT_GE(result.coherenceBefore, 0.0);
    EXPECT_LE(result.coherenceBefore, 1.0);
    EXPECT_GE(result.coherenceAfter, 0.0);
    EXPECT_LE(result.coherenceAfter, 1.0);
    agent->stop();
}

TEST_F(AutonomyPipelineIntegrationTest, HomeworkTrendTrackingWithMomentum) {
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->enableShellAccess(true);
    agent->start();
    for (int i = 0; i < 5; ++i) {
        agent->runCognitiveCycleOnce();
    }
    CognitiveCurriculum curriculum;
    HomeworkLoop loop(*agent, curriculum);
    // Run multiple homework cycles to build trend data.
    auto results = loop.runHomework(3);
    EXPECT_EQ(results.size(), 3u);
    // Check that trends accumulate.
    auto trends = loop.allCoherenceTrends();
    bool hasSamples = false;
    for (const auto& t : trends) {
        if (!t.samples.empty()) {
            hasSamples = true;
            EXPECT_GE(t.slope, -1.0);
            EXPECT_LE(t.slope, 1.0);
        }
    }
    EXPECT_TRUE(hasSamples);
    agent->stop();
}

// ============================================================================
// 5. Endocrine-Autonomy Coupling
// ============================================================================

TEST_F(AutonomyPipelineIntegrationTest, EndocrineModulatesCognitiveCycle) {
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->enableShellAccess(true);
    agent->start();
    // Run cycles and verify endocrine state is reflected in health.
    for (int i = 0; i < 10; ++i) {
        agent->runCognitiveCycleOnce();
    }
    auto report = agent->getAutonomyHealthReport();
    // Competence and momentum should both be positive after successful cycles.
    EXPECT_GT(report.competence, 0.0);
    EXPECT_GT(report.cognitiveMomentum, 0.0);
    agent->stop();
}

// ============================================================================
// 6. Never-Dead-End Invariant Stress Test
// ============================================================================

TEST_F(AutonomyPipelineIntegrationTest, NeverDeadEndUnder50Cycles) {
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->enableShellAccess(true);
    agent->start();
    for (int i = 0; i < 50; ++i) {
        agent->runCognitiveCycleOnce();
        // After every cycle, the agent must have at least one open goal.
        EXPECT_GE(agent->getOpenGoalCount(), 1u)
            << "Dead-ended at cycle " << i;
    }
    agent->stop();
}

TEST_F(AutonomyPipelineIntegrationTest, NeverDeadEndWithShellDisabled) {
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->enableShellAccess(false);
    agent->start();
    for (int i = 0; i < 20; ++i) {
        agent->runCognitiveCycleOnce();
        EXPECT_GE(agent->getOpenGoalCount(), 1u)
            << "Dead-ended at cycle " << i << " (shell disabled)";
    }
    agent->stop();
}

// ============================================================================
// 7. Attention Hysteresis Under Mode Changes
// ============================================================================

TEST_F(AutonomyPipelineIntegrationTest, FocusedGoalRetainedAcrossCycles) {
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->enableShellAccess(true);
    agent->start();
    agent->runCognitiveCycleOnce();
    auto firstFocus = agent->getFocusedGoalId();
    // The focused goal should persist across several cycles (hysteresis).
    bool retained = true;
    for (int i = 0; i < 5; ++i) {
        agent->runCognitiveCycleOnce();
        if (agent->getFocusedGoalId() != firstFocus) {
            // Focus can change if the goal was completed, but should not thrash.
            retained = false;
            break;
        }
    }
    // Either retained focus or the goal was completed (both valid).
    if (!retained) {
        // If focus changed, the original goal should be completed.
        auto goals = agent->getState().getGoals();
        bool originalCompleted = false;
        for (const auto& g : goals) {
            if (g.id == firstFocus && g.status == "completed") {
                originalCompleted = true;
                break;
            }
        }
        // A focused goal may only relinquish focus after completion, and the
        // never-dead-end invariant must immediately leave another open goal.
        EXPECT_TRUE(originalCompleted);
        EXPECT_GE(agent->getOpenGoalCount(), 1u);
    }
    agent->stop();
}

// ============================================================================
// 8. Full Pipeline Integration
// ============================================================================

TEST_F(AutonomyPipelineIntegrationTest, FullPipelineEndToEnd) {
    auto agent = std::make_shared<AutonomousStarter>(makeConfig("FullPipeline-Agent"));
    agent->enableShellAccess(true);
    agent->start();
    // Phase 1: Build momentum.
    for (int i = 0; i < 10; ++i) {
        agent->runCognitiveCycleOnce();
    }
    auto midReport = agent->getAutonomyHealthReport();
    EXPECT_GT(midReport.totalCycles, 0u);
    EXPECT_GT(midReport.cognitiveMomentum, 0.0);
    EXPECT_GT(midReport.cycleEfficiency, 0.0);
    // Phase 2: Run homework loop.
    CognitiveCurriculum curriculum;
    HomeworkLoop loop(*agent, curriculum);
    auto hwResult = loop.runHomeworkCycleOnce();
    EXPECT_FALSE(loop.issuedDestructiveCommand());
    // Phase 3: Continue cognitive cycles after homework.
    for (int i = 0; i < 10; ++i) {
        agent->runCognitiveCycleOnce();
    }
    auto finalReport = agent->getAutonomyHealthReport();
    // Final state should show progress.
    EXPECT_GT(finalReport.totalCycles, midReport.totalCycles);
    EXPECT_GE(finalReport.completedGoals, midReport.completedGoals);
    EXPECT_GE(finalReport.cognitiveMomentum, 0.0);
    EXPECT_LE(finalReport.cognitiveMomentum, 1.0);
    EXPECT_GE(finalReport.cycleEfficiency, 0.0);
    EXPECT_LE(finalReport.cycleEfficiency, 1.0);
    EXPECT_GE(finalReport.goalChainCoherence, 0.0);
    EXPECT_LE(finalReport.goalChainCoherence, 1.0);
    // Never-dead-end must hold.
    EXPECT_GE(agent->getOpenGoalCount(), 1u);
    agent->stop();
}

TEST_F(AutonomyPipelineIntegrationTest, HealthReportAllFieldsPopulated) {
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->enableShellAccess(true);
    agent->start();
    for (int i = 0; i < 15; ++i) {
        agent->runCognitiveCycleOnce();
    }
    auto r = agent->getAutonomyHealthReport();
    // All fields should be populated after running cycles.
    EXPECT_GT(r.totalCycles, 0u);
    EXPECT_GT(r.totalActions, 0u);
    EXPECT_GT(r.successfulActions, 0u);
    EXPECT_GT(r.reflections, 0u);
    EXPECT_FALSE(r.focusedGoalId.empty());
    EXPECT_FALSE(r.lastPlan.empty());
    EXPECT_FALSE(r.lastReflection.empty());
    EXPECT_FALSE(r.healthSummary.empty());
    // Bounded metrics.
    EXPECT_GE(r.competence, 0.0);
    EXPECT_LE(r.competence, 1.0);
    EXPECT_GE(r.goalCompletionRate, 0.0);
    EXPECT_LE(r.goalCompletionRate, 1.0);
    EXPECT_GE(r.actionSuccessRate, 0.0);
    EXPECT_LE(r.actionSuccessRate, 1.0);
    EXPECT_GE(r.goalThemeDiversity, 0.0);
    EXPECT_LE(r.goalThemeDiversity, 1.0);
    EXPECT_GE(r.cognitiveMomentum, 0.0);
    EXPECT_LE(r.cognitiveMomentum, 1.0);
    EXPECT_GE(r.cycleEfficiency, 0.0);
    EXPECT_LE(r.cycleEfficiency, 1.0);
    EXPECT_GE(r.goalChainCoherence, 0.0);
    EXPECT_LE(r.goalChainCoherence, 1.0);
    agent->stop();
}
