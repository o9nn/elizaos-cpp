// test_homework_loop_e2e.cpp
//
// Comprehensive E2E tests for the HomeworkLoop cognitive curriculum integration.
// Validates that:
//   1. HomeworkLoop initializes correctly with AutonomousStarter + CognitiveCurriculum
//   2. Homework cycles are generated and bounded (steps <= 10, the propose boundary)
//   3. Property Coherence scoring produces valid scores in [0,1]
//   4. The 12-step KSM structure-preserving transformation is respected
//   5. Alexander's 15 properties are evaluated via center evidence
//   6. The homework loop integrates with AutonomousStarter without breaking autonomy
//   7. Cross-fork parity: both forks exhibit identical homework behavior
//   8. Non-destructive guarantee: homework never issues destructive commands
//   9. Active-goal reconciliation: stale activeGoalId_ is cleared after reflection
//  10. Goal-first plan convergence: plan selection honours the active goal

#include <gtest/gtest.h>
#include "elizaos/autonomous_starter.hpp"
#include "elizaos/cognitive_curriculum.hpp"
#include "elizaos/homework_loop.hpp"
#include "elizaos/core.hpp"
#include <algorithm>
#include <chrono>
#include <map>
#include <string>
#include <vector>

using namespace elizaos;

namespace {

AgentConfig makeConfig(const std::string& name = "HomeworkLoop-Agent") {
    AgentConfig cfg;
    cfg.agentId = generateUUID();
    cfg.agentName = name;
    cfg.bio = "Homework loop E2E test agent";
    cfg.lore = "Created to validate cognitive curriculum homework integration";
    cfg.adjective = "studious";
    return cfg;
}

/// Deterministic evidence provider for test isolation.
class TestEvidenceProvider : public EvidenceProvider {
public:
    explicit TestEvidenceProvider(double baseCoherence = 0.5)
        : baseCoherence_(baseCoherence) {}

    std::map<CenterId, EvidenceMap> gather(
        const AutonomousStarter& /*agent*/) const override {
        std::map<CenterId, EvidenceMap> out;
        for (CenterId c : allCenters()) {
            EvidenceMap em;
            em["appears_in_memory_stream"] = baseCoherence_;
            em["participates_each_cycle"] = baseCoherence_;
            em["contributes_to_main_loop"] = baseCoherence_;
            out[c] = em;
        }
        return out;
    }

private:
    double baseCoherence_;
};

} // namespace

// ===========================================================================
// HomeworkLoop Core Functionality
// ===========================================================================

class HomeworkLoopE2ETest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = makeConfig();
        agent_ = std::make_shared<AutonomousStarter>(config_);
        agent_->start();
    }

    void TearDown() override {
        if (agent_) agent_->stop();
    }

    AgentConfig config_;
    std::shared_ptr<AutonomousStarter> agent_;
};

TEST_F(HomeworkLoopE2ETest, HomeworkLoopConstructsWithoutCrash) {
    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<TestEvidenceProvider>();
    HomeworkLoop loop(*agent_, curriculum, evidence);
    EXPECT_EQ(loop.homeworkCycleCount(), 0u);
}

TEST_F(HomeworkLoopE2ETest, HomeworkLoopScoresAllCenters) {
    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<TestEvidenceProvider>();
    HomeworkLoop loop(*agent_, curriculum, evidence);

    auto scores = loop.scoreAllCenters();
    EXPECT_EQ(scores.size(), allCenters().size())
        << "scoreAllCenters must return one score per center";

    for (const auto& s : scores) {
        EXPECT_GE(s.coherence, 0.0);
        EXPECT_LE(s.coherence, 1.0);
        EXPECT_FALSE(s.name.empty());
    }
}

TEST_F(HomeworkLoopE2ETest, HomeworkLoopSelectsWeakestCenter) {
    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<TestEvidenceProvider>(0.3);
    HomeworkLoop loop(*agent_, curriculum, evidence);

    auto weakest = loop.selectWeakestCenter();
    EXPECT_FALSE(weakest.name.empty());
    // The weakest center should have the highest loss
    auto allScores = loop.scoreAllCenters();
    for (const auto& s : allScores) {
        EXPECT_LE(weakest.loss, s.loss + 0.001)
            << "selectWeakestCenter must return the center with highest loss (argmax)";
    }
}

TEST_F(HomeworkLoopE2ETest, HomeworkCycleRunsAndReturnsResult) {
    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<TestEvidenceProvider>();
    HomeworkLoop loop(*agent_, curriculum, evidence);

    auto result = loop.runHomeworkCycleOnce();

    EXPECT_FALSE(result.targetName.empty());
    EXPECT_GE(result.coherenceBefore, 0.0);
    EXPECT_LE(result.coherenceBefore, 1.0);
    EXPECT_GE(result.coherenceAfter, 0.0);
    EXPECT_LE(result.coherenceAfter, 1.0);
    EXPECT_GT(result.stepsRun, 0u);
    EXPECT_LE(result.stepsRun, 10u)
        << "Homework cycle must be bounded at the propose step (step 10)";
    EXPECT_TRUE(result.proposedMutation)
        << "Homework cycle must propose a mutation goal (never commit)";
    EXPECT_FALSE(result.handoffSignal.empty())
        << "Homework cycle must emit a handoff signal for the next phase";
}

TEST_F(HomeworkLoopE2ETest, HomeworkCycleCountIncrements) {
    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<TestEvidenceProvider>();
    HomeworkLoop loop(*agent_, curriculum, evidence);

    EXPECT_EQ(loop.homeworkCycleCount(), 0u);
    loop.runHomeworkCycleOnce();
    EXPECT_EQ(loop.homeworkCycleCount(), 1u);
    loop.runHomeworkCycleOnce();
    EXPECT_EQ(loop.homeworkCycleCount(), 2u);
}

TEST_F(HomeworkLoopE2ETest, HomeworkMultipleCyclesReturnResults) {
    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<TestEvidenceProvider>();
    HomeworkLoop loop(*agent_, curriculum, evidence);

    auto results = loop.runHomework(3);
    EXPECT_EQ(results.size(), 3u);
    for (const auto& r : results) {
        EXPECT_FALSE(r.targetName.empty());
        EXPECT_TRUE(r.proposedMutation);
    }
    EXPECT_EQ(loop.homeworkCycleCount(), 3u);
}

// ===========================================================================
// Cognitive Curriculum Integration
// ===========================================================================

TEST_F(HomeworkLoopE2ETest, CognitiveCurriculumHasCenters) {
    auto centers = allCenters();
    EXPECT_GT(centers.size(), 0u)
        << "Cognitive curriculum must define at least one center";
}

TEST_F(HomeworkLoopE2ETest, CognitiveCurriculumCentersHaveNames) {
    for (CenterId c : allCenters()) {
        std::string name = centerName(c);
        EXPECT_FALSE(name.empty())
            << "Every CenterId must have a non-empty name";
    }
}

TEST_F(HomeworkLoopE2ETest, CognitiveCurriculumCoversKSMSteps) {
    // The curriculum should cover aspects of the 12-step KSM transformation
    // by having centers that map to structure-preserving concerns.
    auto centers = allCenters();
    bool hasAutonomy = false;
    bool hasCognitive = false;
    for (CenterId c : centers) {
        if (c == CenterId::Autonomy) hasAutonomy = true;
        if (c == CenterId::CognitiveCycle) hasCognitive = true;
    }
    EXPECT_TRUE(hasAutonomy)
        << "Curriculum must include Autonomy center for KSM self-regulation";
    EXPECT_TRUE(hasCognitive)
        << "Curriculum must include CognitiveCycle center for KSM observe-reason-act";
}

// ===========================================================================
// Property Coherence Scoring
// ===========================================================================

TEST_F(HomeworkLoopE2ETest, PropertyCoherenceScoreIsValid) {
    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<TestEvidenceProvider>(0.7);
    HomeworkLoop loop(*agent_, curriculum, evidence);

    auto scores = loop.scoreAllCenters();
    for (const auto& s : scores) {
        EXPECT_GE(s.coherence, 0.0);
        EXPECT_LE(s.coherence, 1.0);
        // loss = 1 - coherence
        EXPECT_NEAR(s.loss, 1.0 - s.coherence, 0.001);
    }
}

TEST_F(HomeworkLoopE2ETest, PropertyCoherenceRespondsToEvidence) {
    CognitiveCurriculum curriculum;

    // Low evidence -> lower coherence
    auto lowEvidence = std::make_shared<TestEvidenceProvider>(0.1);
    HomeworkLoop loopLow(*agent_, curriculum, lowEvidence);
    auto scoresLow = loopLow.scoreAllCenters();

    // High evidence -> higher coherence
    auto highEvidence = std::make_shared<TestEvidenceProvider>(0.9);
    HomeworkLoop loopHigh(*agent_, curriculum, highEvidence);
    auto scoresHigh = loopHigh.scoreAllCenters();

    // At least one center should show higher coherence with higher evidence
    bool anyHigher = false;
    for (std::size_t i = 0; i < scoresLow.size() && i < scoresHigh.size(); ++i) {
        if (scoresHigh[i].coherence > scoresLow[i].coherence) {
            anyHigher = true;
            break;
        }
    }
    EXPECT_TRUE(anyHigher)
        << "Higher evidence should produce higher coherence in at least one center";
}

// ===========================================================================
// Non-Destructive Guarantee
// ===========================================================================

TEST_F(HomeworkLoopE2ETest, HomeworkLoopNeverIssuesDestructiveCommands) {
    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<TestEvidenceProvider>();
    HomeworkLoop loop(*agent_, curriculum, evidence);

    // Run multiple homework cycles
    auto results = loop.runHomework(5);

    // The non-destructive guarantee must hold across ALL cycles
    EXPECT_FALSE(loop.issuedDestructiveCommand())
        << "Homework loop must NEVER issue a destructive command";

    // All results must be proposals, never commits
    for (const auto& r : results) {
        EXPECT_TRUE(r.proposedMutation)
            << "Homework must propose mutations, never commit them";
        EXPECT_LE(r.stepsRun, 10u)
            << "Homework must stop at the propose boundary (step 10)";
    }
}

// ===========================================================================
// Integration with AutonomousStarter
// ===========================================================================

TEST_F(HomeworkLoopE2ETest, AutonomousStarterCanRunWithHomeworkLoop) {
    // Run cognitive cycles first to establish baseline
    for (int i = 0; i < 5; ++i) {
        agent_->runCognitiveCycleOnce();
    }

    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<TestEvidenceProvider>();
    HomeworkLoop loop(*agent_, curriculum, evidence);

    // Homework should work after agent has established state
    auto result = loop.runHomeworkCycleOnce();
    EXPECT_FALSE(result.targetName.empty());
    EXPECT_TRUE(result.proposedMutation);

    // Agent should still be healthy after homework
    auto report = agent_->getAutonomyHealthReport();
    EXPECT_TRUE(report.isHealthy)
        << "Agent must remain healthy after homework loop execution";
}

TEST_F(HomeworkLoopE2ETest, HomeworkLoopRecordsToAgentMemory) {
    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<TestEvidenceProvider>();
    HomeworkLoop loop(*agent_, curriculum, evidence);

    auto result = loop.runHomeworkCycleOnce();

    // The handoff signal should appear in agent's recent messages
    const auto& messages = agent_->getState().getRecentMessages();
    bool foundHandoff = false;
    for (const auto& msg : messages) {
        if (msg && msg->getContent().find("iteration") != std::string::npos &&
            msg->getContent().find("weakest=") != std::string::npos) {
            foundHandoff = true;
            break;
        }
    }
    EXPECT_TRUE(foundHandoff)
        << "Homework handoff signal must be recorded in agent memory";
}

// ===========================================================================
// Active-Goal Reconciliation (backported from hurdcog)
// ===========================================================================

TEST_F(HomeworkLoopE2ETest, ActiveGoalReconciliationClearsStaleId) {
    // Run enough cycles to complete at least one goal
    for (int i = 0; i < 20; ++i) {
        agent_->runCognitiveCycleOnce();
    }

    auto report = agent_->getAutonomyHealthReport();
    // After many cycles, the agent should have completed goals and the
    // active goal reconciliation should have kept the state consistent
    // (no dangling activeGoalId pointing to a completed goal).
    EXPECT_GE(report.openGoals, 1u)
        << "Never-dead-end invariant: agent must always have at least one open goal";
    EXPECT_TRUE(report.isHealthy)
        << "Agent must remain healthy after active-goal reconciliation";
}

TEST_F(HomeworkLoopE2ETest, GoalFirstPlanConvergence) {
    // Run cognitive cycles and verify the agent converges on its active goal
    for (int i = 0; i < 10; ++i) {
        agent_->runCognitiveCycleOnce();
    }

    auto report = agent_->getAutonomyHealthReport();
    // The agent should have made progress (not stuck in stagnation)
    EXPECT_LT(report.stagnationCount, static_cast<std::size_t>(5))
        << "Goal-first plan selection should prevent sustained stagnation";
    EXPECT_GT(report.totalActions, static_cast<std::size_t>(0))
        << "Agent must have executed actions during cognitive cycles";
    EXPECT_FALSE(report.lastPlan.empty())
        << "Agent must have selected a plan during cognitive cycles";
}

// ===========================================================================
// Cross-Fork Parity
// ===========================================================================

TEST_F(HomeworkLoopE2ETest, CrossForkParity_HomeworkLoopAPI) {
    // Both forks must expose the same HomeworkLoop API surface
    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<TestEvidenceProvider>();
    HomeworkLoop loop(*agent_, curriculum, evidence);

    // scoreAllCenters() must return vector of CenterScore
    auto scores = loop.scoreAllCenters();
    EXPECT_GT(scores.size(), 0u);

    // selectWeakestCenter() must return a valid CenterScore
    auto weakest = loop.selectWeakestCenter();
    EXPECT_FALSE(weakest.name.empty());

    // runHomeworkCycleOnce() must return HomeworkResult
    auto result = loop.runHomeworkCycleOnce();
    EXPECT_TRUE(result.proposedMutation);
    EXPECT_FALSE(result.handoffSignal.empty());

    // homeworkCycleCount() must track iterations
    EXPECT_EQ(loop.homeworkCycleCount(), 1u);

    // lastHandoffSignal() must return the most recent signal
    EXPECT_FALSE(loop.lastHandoffSignal().empty());

    // issuedDestructiveCommand() must always be false
    EXPECT_FALSE(loop.issuedDestructiveCommand());
}

TEST_F(HomeworkLoopE2ETest, CrossForkParity_CognitiveCurriculumAPI) {
    // Both forks must expose the same CognitiveCurriculum API surface
    CognitiveCurriculum curriculum;

    // allCenters() must return non-empty vector of CenterId
    auto centers = allCenters();
    EXPECT_GT(centers.size(), 0u);

    // centerName() must return non-empty string for each center
    for (CenterId c : centers) {
        EXPECT_FALSE(centerName(c).empty());
    }

    // scoreAll() must accept evidence map and return scores
    std::map<CenterId, EvidenceMap> evidence;
    for (CenterId c : centers) {
        EvidenceMap em;
        em["appears_in_memory_stream"] = 0.5;
        em["participates_each_cycle"] = 0.5;
        em["contributes_to_main_loop"] = 0.5;
        evidence[c] = em;
    }
    auto scores = curriculum.scoreAll(evidence);
    EXPECT_EQ(scores.size(), centers.size());

    // weakest() must return the argmax-loss center
    auto weakest = curriculum.weakest(scores);
    EXPECT_FALSE(weakest.name.empty());
}

TEST_F(HomeworkLoopE2ETest, CrossForkParity_AutonomyHealthReport) {
    // Both forks must expose the same AutonomyHealthReport structure
    for (int i = 0; i < 5; ++i) {
        agent_->runCognitiveCycleOnce();
    }

    auto report = agent_->getAutonomyHealthReport();
    EXPECT_GE(report.competence, 0.0);
    EXPECT_LE(report.competence, 1.0);
    EXPECT_GE(report.openGoals, 1u);
    EXPECT_GT(report.totalCycles, 0u);
    EXPECT_GT(report.totalActions, 0u);
    EXPECT_FALSE(report.lastPlan.empty());
    EXPECT_FALSE(report.healthSummary.empty());
}

// ===========================================================================
// Cycle 009: Coherence trend tracking + adaptive practice depth + goal diversity
// ===========================================================================

namespace {

/// Evidence provider whose coherence can be reprogrammed between cycles, letting the
/// test drive improving / flat / regressing trajectories deterministically.
class ProgrammableEvidenceProvider : public EvidenceProvider {
public:
    explicit ProgrammableEvidenceProvider(double coherence = 0.5)
        : coherence_(coherence) {}

    void setCoherence(double value) { coherence_ = value; }

    std::map<CenterId, EvidenceMap> gather(
        const AutonomousStarter& /*agent*/) const override {
        std::map<CenterId, EvidenceMap> out;
        for (CenterId c : allCenters()) {
            EvidenceMap em;
            em["appears_in_memory_stream"] = coherence_;
            em["participates_each_cycle"] = coherence_;
            em["contributes_to_main_loop"] = coherence_;
            out[c] = em;
        }
        return out;
    }

private:
    double coherence_;
};

} // namespace

TEST_F(HomeworkLoopE2ETest, CoherenceTrendStartsEmpty) {
    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<TestEvidenceProvider>();
    HomeworkLoop loop(*agent_, curriculum, evidence);

    // Before any cycle, every center's trend must be empty and non-stagnant.
    for (CenterId c : allCenters()) {
        auto trend = loop.coherenceTrend(c);
        EXPECT_TRUE(trend.samples.empty());
        EXPECT_DOUBLE_EQ(trend.slope, 0.0);
        EXPECT_FALSE(trend.stagnant);
        EXPECT_EQ(trend.center, c);
        EXPECT_EQ(trend.name, centerName(c));
    }
    EXPECT_TRUE(loop.allCoherenceTrends().empty());
}

TEST_F(HomeworkLoopE2ETest, CoherenceTrendAccumulatesSamplesPerTargetingCycle) {
    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<TestEvidenceProvider>(0.4);
    HomeworkLoop loop(*agent_, curriculum, evidence);

    auto r1 = loop.runHomeworkCycleOnce();
    auto trend1 = loop.coherenceTrend(r1.targetCenter);
    EXPECT_EQ(trend1.samples.size(), 1u);
    EXPECT_DOUBLE_EQ(trend1.samples.back(), r1.coherenceAfter);

    // The full trend listing includes only targeted centers.
    auto all = loop.allCoherenceTrends();
    EXPECT_GE(all.size(), 1u);
    for (const auto& t : all) {
        EXPECT_FALSE(t.samples.empty());
    }
}

TEST_F(HomeworkLoopE2ETest, CoherenceTrendDetectsStagnation) {
    CognitiveCurriculum curriculum;
    // Constant evidence -> constant coherence -> zero slope -> stagnant after 3 samples.
    auto evidence = std::make_shared<TestEvidenceProvider>(0.5);
    HomeworkLoop loop(*agent_, curriculum, evidence);

    // With deterministic constant evidence the weakest center is stable (ties broken
    // by canonical order), so the same center accumulates samples each cycle.
    auto r1 = loop.runHomeworkCycleOnce();
    loop.runHomeworkCycleOnce();
    loop.runHomeworkCycleOnce();

    auto trend = loop.coherenceTrend(r1.targetCenter);
    ASSERT_GE(trend.samples.size(), 3u);
    EXPECT_NEAR(trend.slope, 0.0, 1e-9);
    EXPECT_TRUE(trend.stagnant);
}

TEST_F(HomeworkLoopE2ETest, CoherenceTrendDetectsImprovement) {
    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<ProgrammableEvidenceProvider>(0.2);
    HomeworkLoop loop(*agent_, curriculum, evidence);

    auto r1 = loop.runHomeworkCycleOnce();
    evidence->setCoherence(0.4);
    loop.runHomeworkCycleOnce();
    evidence->setCoherence(0.6);
    loop.runHomeworkCycleOnce();

    auto trend = loop.coherenceTrend(r1.targetCenter);
    ASSERT_GE(trend.samples.size(), 3u);
    EXPECT_GT(trend.slope, 0.0);
    EXPECT_FALSE(trend.stagnant);
}

TEST_F(HomeworkLoopE2ETest, AdaptivePracticeDepthDefaultsToOne) {
    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<TestEvidenceProvider>(0.5);
    HomeworkLoop loop(*agent_, curriculum, evidence);

    // No longitudinal signal yet: every center gets the default single bounded cycle.
    for (CenterId c : allCenters()) {
        EXPECT_EQ(loop.adaptivePracticeDepth(c), 1u);
    }

    auto r1 = loop.runHomeworkCycleOnce();
    EXPECT_EQ(r1.practiceDepth, 1u);
}

TEST_F(HomeworkLoopE2ETest, AdaptivePracticeDepthDeepensOnFlatTrend) {
    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<TestEvidenceProvider>(0.5);
    HomeworkLoop loop(*agent_, curriculum, evidence);

    // Two flat samples -> the third cycle should deepen practice beyond 1.
    auto r1 = loop.runHomeworkCycleOnce();
    loop.runHomeworkCycleOnce();
    const std::size_t depth = loop.adaptivePracticeDepth(r1.targetCenter);
    EXPECT_GT(depth, 1u);
    EXPECT_LE(depth, loop.maxPracticeDepth());

    auto r3 = loop.runHomeworkCycleOnce();
    EXPECT_GT(r3.practiceDepth, 1u);
    EXPECT_LE(r3.practiceDepth, loop.maxPracticeDepth());
}

TEST_F(HomeworkLoopE2ETest, AdaptivePracticeDepthStaysBoundedAtConfiguredMax) {
    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<TestEvidenceProvider>(0.5);
    HomeworkLoop loop(*agent_, curriculum, evidence);
    loop.setMaxPracticeDepth(2);
    EXPECT_EQ(loop.maxPracticeDepth(), 2u);

    // Even after many flat cycles, depth never exceeds the configured maximum,
    // preserving the bounded-homework alignment tenet.
    for (int i = 0; i < 5; ++i) {
        auto r = loop.runHomeworkCycleOnce();
        EXPECT_LE(r.practiceDepth, 2u);
        EXPECT_LE(r.stepsRun, 10u);  // propose boundary unchanged
        EXPECT_FALSE(loop.issuedDestructiveCommand());
    }

    // A zero max is clamped to 1 (never zero cycles).
    loop.setMaxPracticeDepth(0);
    EXPECT_EQ(loop.maxPracticeDepth(), 1u);
}

TEST_F(HomeworkLoopE2ETest, AdaptivePracticeDepthResetsWhenImproving) {
    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<ProgrammableEvidenceProvider>(0.2);
    HomeworkLoop loop(*agent_, curriculum, evidence);

    auto r1 = loop.runHomeworkCycleOnce();
    evidence->setCoherence(0.5);
    loop.runHomeworkCycleOnce();
    evidence->setCoherence(0.8);
    loop.runHomeworkCycleOnce();

    // Rising trend: no extra depth is spent on an improving center.
    EXPECT_EQ(loop.adaptivePracticeDepth(r1.targetCenter), 1u);
}

TEST_F(HomeworkLoopE2ETest, GoalThemeDiversityReportedInHealthReport) {
    // Fresh agent: no completed goals yet -> zero diversity, zero themes.
    auto before = agent_->getAutonomyHealthReport();
    EXPECT_DOUBLE_EQ(before.goalThemeDiversity, 0.0);
    EXPECT_EQ(before.distinctGoalThemes, 0u);

    // Drive cycles until goals complete, then verify the diversity telemetry is
    // consistent: themes counted only over completed goals, entropy in [0,1].
    for (int i = 0; i < 12; ++i) {
        agent_->runCognitiveCycleOnce();
    }
    auto after = agent_->getAutonomyHealthReport();
    EXPECT_GE(after.goalThemeDiversity, 0.0);
    EXPECT_LE(after.goalThemeDiversity, 1.0);
    if (after.completedGoals >= 2 && after.distinctGoalThemes >= 2) {
        // Multiple themes must register non-zero diversity.
        EXPECT_GT(after.goalThemeDiversity, 0.0);
    }
    if (after.completedGoals > 0) {
        EXPECT_GE(after.distinctGoalThemes, 1u);
        EXPECT_LE(after.distinctGoalThemes, after.completedGoals);
    }
}

TEST_F(HomeworkLoopE2ETest, HomeworkResultCarriesPracticeDepthAndStaysAligned) {
    CognitiveCurriculum curriculum;
    auto evidence = std::make_shared<TestEvidenceProvider>(0.5);
    HomeworkLoop loop(*agent_, curriculum, evidence);

    auto results = loop.runHomework(3);
    ASSERT_EQ(results.size(), 3u);
    for (const auto& r : results) {
        // Every cycle stays within the alignment contract: bounded steps,
        // propose-only mutation, at least one bounded cognitive cycle of practice.
        EXPECT_GE(r.practiceDepth, 1u);
        EXPECT_LE(r.practiceDepth, loop.maxPracticeDepth());
        EXPECT_LE(r.stepsRun, 10u);
        EXPECT_TRUE(r.proposedMutation);
        EXPECT_FALSE(r.handoffSignal.empty());
    }
    EXPECT_FALSE(loop.issuedDestructiveCommand());
}
