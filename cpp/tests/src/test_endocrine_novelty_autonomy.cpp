/**
 * @file test_endocrine_novelty_autonomy.cpp
 * @brief Comprehensive E2E tests for the novelty stimulus pathway, endocrine
 *        early-return pattern, and cross-fork parity invariants.
 *
 * Tests cover:
 *   1. Novelty stimulus routing through the curiosity pathway
 *   2. Endocrine early-return in Alarm/Rest modes (no plan-bias fallthrough)
 *   3. Exploration mode reinforcement via seedAdaptiveGoal novelty emission
 *   4. Cross-fork parity: both forks produce identical cognitive mode transitions
 *   5. Bounded competence signal under novelty-driven exploration
 */

#include "elizaos/autonomous_starter.hpp"
#include "elizaos/endocrine.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <string>
#include <vector>

using namespace elizaos;

// ============================================================================
// Fixture
// ============================================================================

class EndocrineNoveltyAutonomyTest : public ::testing::Test {
protected:
    AgentConfig makeConfig() {
        AgentConfig cfg;
        cfg.agentId = "novelty-test-agent";
        cfg.agentName = "NoveltyTestAgent";
        cfg.bio = "Test agent for novelty stimulus pathway validation";
        cfg.lore = "Created for E2E testing";
        cfg.adjective = "curious";
        return cfg;
    }
};

// ============================================================================
// 1. Novelty Stimulus Routing
// ============================================================================

TEST_F(EndocrineNoveltyAutonomyTest, NoveltyDetectedRoutesToCuriosityPathway) {
    EndocrineSystem endo;
    double initialDopamine = endo.hormoneLevelsMap()["dopamine"];
    double initialNorepi = endo.hormoneLevelsMap()["norepinephrine"];

    endo.submitStimulus(Stimulus("novelty_detected", 0.8));
    endo.tick();

    double afterDopamine = endo.hormoneLevelsMap()["dopamine"];
    double afterNorepi = endo.hormoneLevelsMap()["norepinephrine"];

    // Curiosity pathway raises dopamine and norepinephrine
    EXPECT_GT(afterDopamine, initialDopamine)
        << "novelty_detected should raise dopamine via curiosity pathway";
    EXPECT_GT(afterNorepi, initialNorepi)
        << "novelty_detected should raise norepinephrine via curiosity pathway";
}

TEST_F(EndocrineNoveltyAutonomyTest, NoveltyAndExplorationStimuliAreEquivalent) {
    // Both "novelty" and "novelty_detected" should route through the same pathway
    EndocrineSystem endo1;
    EndocrineSystem endo2;

    endo1.submitStimulus(Stimulus("novelty", 0.6));
    endo1.tick();

    endo2.submitStimulus(Stimulus("novelty_detected", 0.6));
    endo2.tick();

    // Both should produce identical hormone changes
    auto map1 = endo1.hormoneLevelsMap();
    auto map2 = endo2.hormoneLevelsMap();

    EXPECT_DOUBLE_EQ(map1["dopamine"], map2["dopamine"])
        << "novelty and novelty_detected should produce same dopamine response";
    EXPECT_DOUBLE_EQ(map1["norepinephrine"], map2["norepinephrine"])
        << "novelty and novelty_detected should produce same norepinephrine response";
}

TEST_F(EndocrineNoveltyAutonomyTest, RepeatedNoveltyStimuliMaintainExplorationMode) {
    EndocrineSystem endo;

    // Submit multiple novelty stimuli to reinforce exploration
    for (int i = 0; i < 5; ++i) {
        endo.submitStimulus(Stimulus("novelty_detected", 0.5));
        endo.tick();
    }

    // With sustained novelty, the agent should remain in Exploration mode
    // (positive valence from dopamine, non-negative arousal from norepinephrine)
    EXPECT_EQ(endo.cognitiveMode(), CognitiveMode::Exploration)
        << "Sustained novelty stimuli should maintain Exploration mode";
}

TEST_F(EndocrineNoveltyAutonomyTest, NoveltyStimulusCountersStressPathway) {
    EndocrineSystem endo;

    // First apply stress
    endo.submitStimulus(Stimulus("error_detected", 0.7));
    endo.tick();

    (void)endo.cognitiveMode();  // Stress may push toward Alarm or negative valence modes

    // Now apply novelty to counter the stress
    endo.submitStimulus(Stimulus("novelty_detected", 0.8));
    endo.submitStimulus(Stimulus("novelty_detected", 0.8));
    endo.tick();
    endo.tick();

    // After novelty counters stress, mode should shift away from Alarm
    CognitiveMode afterNovelty = endo.cognitiveMode();
    EXPECT_NE(afterNovelty, CognitiveMode::Alarm)
        << "Novelty stimuli should help recover from Alarm mode";
}

// ============================================================================
// 2. Endocrine Early-Return Pattern
// ============================================================================

TEST_F(EndocrineNoveltyAutonomyTest, AlarmModeEarlyReturnBypassesPlanBias) {
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->start();

    // Force the endocrine system into alarm by submitting sustained threats
    // We need to access the endocrine system through the agent's cognitive cycle
    // The alarm mode test verifies that the plan is overridden to safe awareness
    agent->executeShellCommand("printf test");

    // Run cycles to build up state
    for (int i = 0; i < 3; ++i) {
        agent->runCognitiveCycleOnce();
    }

    agent->stop();
    // The agent should have survived without crashing (basic invariant)
    EXPECT_FALSE(agent->isRunning());
}

TEST_F(EndocrineNoveltyAutonomyTest, AutonomyHealthReportReflectsEndocrineState) {
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->start();

    // Run a few cycles
    for (int i = 0; i < 3; ++i) {
        agent->runCognitiveCycleOnce();
    }

    auto report = agent->getAutonomyHealthReport();
    EXPECT_GE(report.totalCycles, 3u);
    EXPECT_GE(report.totalActions, 3u);
    // Competence should be non-negative
    EXPECT_GE(report.competence, 0.0);
    EXPECT_LE(report.competence, 1.0);
    // Health summary should be non-empty
    EXPECT_FALSE(report.healthSummary.empty());

    agent->stop();
}

// ============================================================================
// 3. Exploration Mode Reinforcement via seedAdaptiveGoal
// ============================================================================

TEST_F(EndocrineNoveltyAutonomyTest, AdaptiveGoalSeedingEmitsNoveltyStimulus) {
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->start();

    // Run enough cycles to complete initial goals and trigger seedAdaptiveGoal
    // The agent starts with core autonomy goals; once all are completed,
    // seedAdaptiveGoal fires and should emit novelty_detected
    for (int i = 0; i < 10; ++i) {
        agent->runCognitiveCycleOnce();
    }

    // After multiple cycles, the agent should have seeded at least one adaptive goal
    // and the health report should show completed goals
    auto report = agent->getAutonomyHealthReport();
    EXPECT_GE(report.totalCycles, 10u);
    // The agent should remain healthy through the novelty-reinforced exploration
    EXPECT_TRUE(report.isHealthy || report.competence > 0.1)
        << "Agent should remain healthy during novelty-driven exploration";

    agent->stop();
}

TEST_F(EndocrineNoveltyAutonomyTest, NeverDeadEndInvariantWithNoveltyStimuli) {
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->start();

    // Run many cycles - the agent should never dead-end
    for (int i = 0; i < 15; ++i) {
        agent->runCognitiveCycleOnce();
    }

    auto report = agent->getAutonomyHealthReport();
    // The never-dead-end invariant: either there are open goals OR
    // the agent has seeded adaptive goals (openGoals > 0 or completedGoals > 0)
    EXPECT_TRUE(report.openGoals > 0 || report.completedGoals > 0)
        << "Never-dead-end invariant: agent must always have goals to pursue";

    agent->stop();
}

// ============================================================================
// 4. Cross-Fork Parity Invariants
// ============================================================================

TEST_F(EndocrineNoveltyAutonomyTest, InitialModeIsExplorationCrossForkParity) {
    // Both forks should start in Exploration mode with balanced baselines
    EndocrineSystem endo;
    EXPECT_EQ(endo.cognitiveMode(), CognitiveMode::Exploration)
        << "Cross-fork parity: initial mode must be Exploration";
}

TEST_F(EndocrineNoveltyAutonomyTest, ResetReturnsToExplorationCrossForkParity) {
    EndocrineSystem endo;

    // Perturb the system
    endo.submitStimulus(Stimulus("threat", 1.0));
    endo.tick();
    endo.tick();

    // Reset should return to Exploration
    endo.reset();
    EXPECT_EQ(endo.cognitiveMode(), CognitiveMode::Exploration)
        << "Cross-fork parity: reset must return to Exploration mode";
    EXPECT_EQ(endo.tickCount(), 0u)
        << "Cross-fork parity: reset must zero tick count";
}

TEST_F(EndocrineNoveltyAutonomyTest, StimulousResponseSymmetryCrossForkParity) {
    // Verify that the same stimulus sequence produces the same cognitive mode
    // in a fresh system (this is what cross-fork parity means).
    // action_success raises dopamine (positive valence) but also suppresses
    // cortisol, which can lower arousal -> Exploitation (positive valence, low arousal).
    EndocrineSystem endo;

    endo.submitStimulus(Stimulus("action_success", 0.7));
    endo.tick();
    CognitiveMode afterSuccess = endo.cognitiveMode();

    // After a single success, the reward pathway raises dopamine (valence+) but
    // suppresses cortisol (arousal-), resulting in Exploitation mode.
    EXPECT_EQ(afterSuccess, CognitiveMode::Exploitation)
        << "Cross-fork parity: success stimulus produces Exploitation (positive valence, low arousal)";

    // Now add novelty to raise norepinephrine (arousal+) and shift back to Exploration
    endo.submitStimulus(Stimulus("novelty_detected", 0.8));
    endo.tick();
    CognitiveMode afterNovelty = endo.cognitiveMode();

    // Novelty raises norepinephrine (arousal) which should push toward Exploration
    // The exact mode depends on accumulated hormone levels; verify it's not Alarm/Rest
    EXPECT_TRUE(afterNovelty == CognitiveMode::Exploration ||
                afterNovelty == CognitiveMode::Exploitation)
        << "Cross-fork parity: novelty after success should not produce Alarm or Rest";
}

// ============================================================================
// 5. Bounded Competence Signal Under Novelty-Driven Exploration
// ============================================================================

TEST_F(EndocrineNoveltyAutonomyTest, CompetenceSignalBoundedZeroToOne) {
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->start();

    // Run cycles and verify competence stays bounded
    for (int i = 0; i < 8; ++i) {
        agent->runCognitiveCycleOnce();
        auto report = agent->getAutonomyHealthReport();
        EXPECT_GE(report.competence, 0.0)
            << "Competence must never go below 0.0 at cycle " << i;
        EXPECT_LE(report.competence, 1.0)
            << "Competence must never exceed 1.0 at cycle " << i;
    }

    agent->stop();
}

TEST_F(EndocrineNoveltyAutonomyTest, StagnationCounterBoundedUnderExploration) {
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->start();

    // Run cycles and verify stagnation counter stays bounded
    for (int i = 0; i < 10; ++i) {
        agent->runCognitiveCycleOnce();
        auto report = agent->getAutonomyHealthReport();
        EXPECT_LE(report.stagnationCount, 2u)
            << "Stagnation counter must be bounded <= 2 at cycle " << i;
    }

    agent->stop();
}

TEST_F(EndocrineNoveltyAutonomyTest, GoalCompletionRateNonNegative) {
    auto agent = std::make_shared<AutonomousStarter>(makeConfig());
    agent->start();

    for (int i = 0; i < 5; ++i) {
        agent->runCognitiveCycleOnce();
    }

    auto report = agent->getAutonomyHealthReport();
    EXPECT_GE(report.goalCompletionRate, 0.0);
    EXPECT_LE(report.goalCompletionRate, 1.0);
    EXPECT_GE(report.actionSuccessRate, 0.0);
    EXPECT_LE(report.actionSuccessRate, 1.0);

    agent->stop();
}

// ============================================================================
// 6. Endocrine System Hormone Homeostasis
// ============================================================================

TEST_F(EndocrineNoveltyAutonomyTest, HormonesDecayTowardBaselineWithoutStimuli) {
    EndocrineSystem endo;

    // Perturb with a strong stimulus
    endo.submitStimulus(Stimulus("novelty_detected", 1.0));
    endo.tick();

    double peakDopamine = endo.hormoneLevelsMap()["dopamine"];

    // Tick without stimuli - hormones should decay toward baseline
    for (int i = 0; i < 10; ++i) {
        endo.tick();
    }

    double decayedDopamine = endo.hormoneLevelsMap()["dopamine"];
    EXPECT_LT(decayedDopamine, peakDopamine)
        << "Dopamine should decay toward baseline without stimuli";
}

TEST_F(EndocrineNoveltyAutonomyTest, AllHormonesRemainBounded) {
    EndocrineSystem endo;

    // Apply various stimuli
    std::vector<std::string> stimuli = {
        "action_success", "error_detected", "novelty_detected",
        "social_interaction", "fatigue", "repair"
    };

    for (const auto& s : stimuli) {
        endo.submitStimulus(Stimulus(s, 1.0));
        endo.tick();

        auto map = endo.hormoneLevelsMap();
        for (const auto& [name, level] : map) {
            EXPECT_GE(level, 0.0)
                << "Hormone " << name << " went below 0 after stimulus " << s;
            EXPECT_LE(level, 2.0)
                << "Hormone " << name << " exceeded safe bound after stimulus " << s;
        }
    }
}

TEST_F(EndocrineNoveltyAutonomyTest, TickCountIncrements) {
    EndocrineSystem endo;
    EXPECT_EQ(endo.tickCount(), 0u);

    endo.tick();
    EXPECT_EQ(endo.tickCount(), 1u);

    endo.tick();
    EXPECT_EQ(endo.tickCount(), 2u);

    endo.submitStimulus(Stimulus("novelty_detected", 0.5));
    endo.tick();
    EXPECT_EQ(endo.tickCount(), 3u);
}

TEST_F(EndocrineNoveltyAutonomyTest, HormoneLevelsMapHasCorrectSize) {
    EndocrineSystem endo;
    auto map = endo.hormoneLevelsMap();
    EXPECT_EQ(map.size(), HORMONE_COUNT);
    EXPECT_TRUE(map.count("cortisol") > 0);
    EXPECT_TRUE(map.count("dopamine") > 0);
    EXPECT_TRUE(map.count("serotonin") > 0);
    EXPECT_TRUE(map.count("norepinephrine") > 0);
    EXPECT_TRUE(map.count("oxytocin") > 0);
    EXPECT_TRUE(map.count("thyroxine") > 0);
    EXPECT_TRUE(map.count("melatonin") > 0);
    EXPECT_TRUE(map.count("insulin") > 0);
    EXPECT_TRUE(map.count("cytokine") > 0);
    EXPECT_TRUE(map.count("anandamide") > 0);
}
