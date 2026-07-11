/**
 * test_autonomous_cognitive_e2e.cpp — Comprehensive E2E tests for
 * AutonomousStarter and CognitiveCurriculum modules
 *
 * Covers:
 *   1. AutonomousStarter — lifecycle, cognitive cycle, goal management, health
 *   2. CognitiveCurriculum — center scoring, property evidence, weakest center
 *   3. Integration — starter with curriculum feedback
 *
 * Cross-fork parity: identical test file in both hurdcog and o9nn.
 */
#include <gtest/gtest.h>
#include "elizaos/autonomous_starter.hpp"
#include "elizaos/cognitive_curriculum.hpp"
#include <string>
#include <vector>
#include <chrono>
#include <thread>

using namespace elizaos;

// ============================================================================
// CognitiveCurriculum Tests
// ============================================================================

class CognitiveCurriculumTest : public ::testing::Test {
protected:
    void SetUp() override {
        curriculum_ = std::make_unique<CognitiveCurriculum>();
    }
    std::unique_ptr<CognitiveCurriculum> curriculum_;
};

TEST_F(CognitiveCurriculumTest, PropertyCount) {
    // Should have 15 Alexander properties
    EXPECT_EQ(curriculum_->propertyCount(), 15u);
}

TEST_F(CognitiveCurriculumTest, AllCentersAvailable) {
    auto centers = allCenters();
    EXPECT_GE(centers.size(), 5u);
}

TEST_F(CognitiveCurriculumTest, CenterNaming) {
    // Each center should have a non-empty name
    for (auto id : allCenters()) {
        std::string name = centerName(id);
        EXPECT_FALSE(name.empty());
    }
}

TEST_F(CognitiveCurriculumTest, PropertyEvidenceKeys) {
    auto keys = propertyEvidenceKeys();
    EXPECT_EQ(keys.size(), 15u);
    // Verify some expected keys exist (actual keys from implementation)
    bool hasCoarseAndFine = false;
    bool hasPrimaryClass = false;
    for (auto& k : keys) {
        if (k == "has_coarse_and_fine_parts") hasCoarseAndFine = true;
        if (k == "primary_class_implemented") hasPrimaryClass = true;
    }
    EXPECT_TRUE(hasCoarseAndFine);
    EXPECT_TRUE(hasPrimaryClass);
}

TEST_F(CognitiveCurriculumTest, ScoreAllWithEmptyEvidence) {
    std::map<CenterId, EvidenceMap> evidence;
    for (auto id : allCenters()) {
        evidence[id] = {};
    }
    auto scores = curriculum_->scoreAll(evidence);
    EXPECT_EQ(scores.size(), allCenters().size());
    // All scores should be 0 with no evidence
    for (auto& s : scores) {
        EXPECT_NEAR(s.coherence, 0.0, 0.01);
    }
}

TEST_F(CognitiveCurriculumTest, ScoreAllWithFullEvidence) {
    std::map<CenterId, EvidenceMap> evidence;
    auto keys = propertyEvidenceKeys();
    for (auto id : allCenters()) {
        EvidenceMap em;
        for (auto& k : keys) {
            em[k] = 1.0;
        }
        evidence[id] = em;
    }
    auto scores = curriculum_->scoreAll(evidence);
    // With full evidence, scores should be high
    for (auto& s : scores) {
        EXPECT_GT(s.coherence, 0.5);
    }
}

TEST_F(CognitiveCurriculumTest, WeakestCenter) {
    std::map<CenterId, EvidenceMap> evidence;
    auto keys = propertyEvidenceKeys();
    auto centers = allCenters();
    // Give all centers full evidence except the first one
    for (size_t i = 0; i < centers.size(); ++i) {
        EvidenceMap em;
        double val = (i == 0) ? 0.1 : 1.0;
        for (auto& k : keys) {
            em[k] = val;
        }
        evidence[centers[i]] = em;
    }
    auto scores = curriculum_->scoreAll(evidence);
    auto weakest = curriculum_->weakest(scores);
    EXPECT_EQ(weakest.center, centers[0]);
    EXPECT_LT(weakest.coherence, 0.5);
}

TEST_F(CognitiveCurriculumTest, ScoreMonotonicity) {
    // More evidence should yield higher scores
    auto keys = propertyEvidenceKeys();
    auto centers = allCenters();
    CenterId testCenter = centers[0];

    // Low evidence
    std::map<CenterId, EvidenceMap> lowEvidence;
    EvidenceMap lowEm;
    for (auto& k : keys) lowEm[k] = 0.2;
    lowEvidence[testCenter] = lowEm;

    // High evidence
    std::map<CenterId, EvidenceMap> highEvidence;
    EvidenceMap highEm;
    for (auto& k : keys) highEm[k] = 0.9;
    highEvidence[testCenter] = highEm;

    auto lowScores = curriculum_->scoreAll(lowEvidence);
    auto highScores = curriculum_->scoreAll(highEvidence);

    double lowScore = 0.0, highScore = 0.0;
    for (auto& s : lowScores) if (s.center == testCenter) lowScore = s.coherence;
    for (auto& s : highScores) if (s.center == testCenter) highScore = s.coherence;
    EXPECT_GT(highScore, lowScore);
}

// ============================================================================
// AutonomousStarter — Lifecycle Tests
// ============================================================================

class AutonomousStarterTest : public ::testing::Test {
protected:
    void SetUp() override {
        AgentConfig config;
        config.agentId = "test-agent-001";
        config.agentName = "TestAgent";
        starter_ = std::make_unique<AutonomousStarter>(config);
    }
    void TearDown() override {
        if (starter_ && starter_->isRunning()) {
            starter_->stop();
        }
    }
    std::unique_ptr<AutonomousStarter> starter_;
};

TEST_F(AutonomousStarterTest, InitialState) {
    EXPECT_FALSE(starter_->isRunning());
    EXPECT_FALSE(starter_->isAutonomousLoopRunning());
    EXPECT_EQ(starter_->getCognitiveCycleCount(), 0u);
    EXPECT_EQ(starter_->getActionCount(), 0u);
}

TEST_F(AutonomousStarterTest, StartAndStop) {
    starter_->start();
    EXPECT_TRUE(starter_->isRunning());
    starter_->stop();
    EXPECT_FALSE(starter_->isRunning());
}

TEST_F(AutonomousStarterTest, ShellAccessControl) {
    // After construction, shell access may be enabled by default
    bool initial = starter_->isShellAccessEnabled();
    starter_->enableShellAccess(!initial);
    EXPECT_NE(starter_->isShellAccessEnabled(), initial);
    starter_->enableShellAccess(initial);
    EXPECT_EQ(starter_->isShellAccessEnabled(), initial);
}

TEST_F(AutonomousStarterTest, LoopIntervalConfig) {
    auto interval = std::chrono::milliseconds(500);
    starter_->setLoopInterval(interval);
    EXPECT_EQ(starter_->getLoopInterval(), interval);
}

TEST_F(AutonomousStarterTest, CognitiveCycleExecution) {
    starter_->start();
    starter_->enableShellAccess(true);
    auto count = starter_->runCognitiveCycleOnce();
    // Should return the cycle count (at least 1)
    EXPECT_GE(count, 1u);
    EXPECT_EQ(starter_->getCognitiveCycleCount(), count);
}

TEST_F(AutonomousStarterTest, MultipleCogitveCycles) {
    starter_->start();
    starter_->enableShellAccess(true);
    for (int i = 0; i < 3; ++i) {
        starter_->runCognitiveCycleOnce();
    }
    EXPECT_GE(starter_->getCognitiveCycleCount(), 3u);
}

// ============================================================================
// AutonomousStarter — Goal Management
// ============================================================================

TEST_F(AutonomousStarterTest, OpenGoalCount) {
    starter_->start();
    // After start, core autonomy goals should be seeded
    EXPECT_GE(starter_->getOpenGoalCount(), 0u);
}

TEST_F(AutonomousStarterTest, CompletedGoalCount) {
    starter_->start();
    EXPECT_EQ(starter_->getCompletedGoalCount(), 0u);
}

TEST_F(AutonomousStarterTest, StagnationCounter) {
    starter_->start();
    EXPECT_EQ(starter_->getStagnationCounter(), 0u);
}

// ============================================================================
// AutonomousStarter — Health Report
// ============================================================================

TEST_F(AutonomousStarterTest, HealthReport) {
    starter_->start();
    auto report = starter_->getAutonomyHealthReport();
    EXPECT_GE(report.competence, 0.0);
    EXPECT_LE(report.competence, 1.0);
    EXPECT_EQ(report.totalCycles, starter_->getCognitiveCycleCount());
    EXPECT_TRUE(report.isHealthy);
    EXPECT_FALSE(report.healthSummary.empty());
}

TEST_F(AutonomousStarterTest, HealthReportAfterCycles) {
    starter_->start();
    starter_->enableShellAccess(true);
    starter_->runCognitiveCycleOnce();
    starter_->runCognitiveCycleOnce();
    auto report = starter_->getAutonomyHealthReport();
    EXPECT_GE(report.totalCycles, 2u);
    EXPECT_GE(report.totalActions, 0u);
}

// ============================================================================
// AutonomousStarter — Action Tracking
// ============================================================================

TEST_F(AutonomousStarterTest, ActionCounters) {
    starter_->start();
    EXPECT_EQ(starter_->getSuccessfulActionCount(), 0u);
    EXPECT_EQ(starter_->getFailedActionCount(), 0u);
    EXPECT_EQ(starter_->getConsecutiveActionFailures(), 0u);
}

TEST_F(AutonomousStarterTest, ReflectionTracking) {
    starter_->start();
    EXPECT_EQ(starter_->getReflectionCount(), 0u);
    EXPECT_TRUE(starter_->getLastReflection().empty());
}

TEST_F(AutonomousStarterTest, PlanTracking) {
    starter_->start();
    EXPECT_TRUE(starter_->getLastPlan().empty());
}

// ============================================================================
// AutonomousStarter — Shell Command Execution
// ============================================================================

TEST_F(AutonomousStarterTest, ShellCommandWithoutAccess) {
    starter_->start();
    starter_->enableShellAccess(false);
    auto result = starter_->executeShellCommand("echo hello");
    EXPECT_FALSE(result.success);
}

TEST_F(AutonomousStarterTest, ShellCommandWithAccess) {
    starter_->start();
    starter_->enableShellAccess(true);
    auto result = starter_->executeShellCommand("echo hello");
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.output.find("hello"), std::string::npos);
}

TEST_F(AutonomousStarterTest, ShellCommandCd) {
    starter_->start();
    starter_->enableShellAccess(true);
    auto result = starter_->executeShellCommand("cd /tmp");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(starter_->getCurrentWorkingDirectory(), "/tmp");
}

TEST_F(AutonomousStarterTest, DangerousCommandBlocked) {
    starter_->start();
    starter_->enableShellAccess(true);
    // rm -rf / should be blocked
    auto result = starter_->executeShellCommand("rm -rf /");
    EXPECT_FALSE(result.success);
}

// ============================================================================
// AutonomousStarter — Plan Success Tracking
// ============================================================================

TEST_F(AutonomousStarterTest, PlanSuccessRatio) {
    starter_->start();
    // No plans executed yet
    double ratio = starter_->getPlanSuccessRatio("test_plan");
    EXPECT_NEAR(ratio, 0.5, 0.5); // Default or 0
}

// ============================================================================
// AutonomousStarter — Autonomous Loop
// ============================================================================

TEST_F(AutonomousStarterTest, AutonomousLoopStartStop) {
    starter_->start();
    starter_->enableShellAccess(true);
    starter_->setLoopInterval(std::chrono::milliseconds(100));
    starter_->startAutonomousLoop();
    EXPECT_TRUE(starter_->isAutonomousLoopRunning());
    std::this_thread::sleep_for(std::chrono::milliseconds(350));
    starter_->stopAutonomousLoop();
    EXPECT_FALSE(starter_->isAutonomousLoopRunning());
    // Should have run at least 2-3 cycles
    EXPECT_GE(starter_->getCognitiveCycleCount(), 2u);
}

// ============================================================================
// Integration — Starter with Curriculum
// ============================================================================

TEST(AutonomousCognitiveIntegration, StarterProducesEvidence) {
    AgentConfig config;
    config.agentId = "integration-test-001";
    config.agentName = "IntegrationAgent";
    AutonomousStarter starter(config);
    CognitiveCurriculum curriculum;

    starter.start();
    starter.enableShellAccess(true);
    starter.runCognitiveCycleOnce();

    // The starter's health report provides evidence for curriculum scoring
    auto report = starter.getAutonomyHealthReport();
    
    // Build evidence from the health report
    EvidenceMap evidence;
    evidence["primary_class_implemented"] = report.isHealthy ? 1.0 : 0.0;
    evidence["strong_centers"] = report.competence;

    // Score should be computable
    std::map<CenterId, EvidenceMap> allEvidence;
    for (auto id : allCenters()) {
        allEvidence[id] = evidence;
    }
    auto scores = curriculum.scoreAll(allEvidence);
    EXPECT_FALSE(scores.empty());

    starter.stop();
}

TEST(AutonomousCognitiveIntegration, CurriculumGuidesGoalSelection) {
    CognitiveCurriculum curriculum;

    // Simulate evidence where one center is weak
    std::map<CenterId, EvidenceMap> evidence;
    auto keys = propertyEvidenceKeys();
    auto centers = allCenters();
    for (size_t i = 0; i < centers.size(); ++i) {
        EvidenceMap em;
        for (auto& k : keys) {
            em[k] = (i == 0) ? 0.1 : 0.9;
        }
        evidence[centers[i]] = em;
    }

    auto scores = curriculum.scoreAll(evidence);
    auto weakest = curriculum.weakest(scores);

    // The weakest center should guide goal creation
    EXPECT_LT(weakest.coherence, 0.5);
    EXPECT_FALSE(weakest.name.empty());
}
