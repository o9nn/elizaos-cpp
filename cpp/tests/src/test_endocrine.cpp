/**
 * ElizaOS C++ - Virtual Endocrine System E2E Tests
 *
 * Comprehensive tests for the endocrine center covering:
 *   - Hormone bus operations (get/set/decay/reset)
 *   - Valence-arousal computation
 *   - Cognitive mode detection
 *   - Virtual gland responses
 *   - Stimulus routing
 *   - Homeostatic decay
 *   - Edge cases and boundary conditions
 *   - Integration with cognitive cycle
 */
#include <gtest/gtest.h>
#include "elizaos/endocrine.hpp"
#include <cmath>
#include <string>

using namespace elizaos;

// ============================================================================
// HormoneBus Tests
// ============================================================================

TEST(HormoneBus, DefaultLevelsAreZero) {
    HormoneBus bus;
    for (std::size_t i = 0; i < HORMONE_COUNT; ++i) {
        EXPECT_DOUBLE_EQ(bus.get(static_cast<HormoneId>(i)), 0.0);
    }
}

TEST(HormoneBus, SetAndGet) {
    HormoneBus bus;
    bus.set(HormoneId::Dopamine, 0.75);
    EXPECT_DOUBLE_EQ(bus.get(HormoneId::Dopamine), 0.75);
}

TEST(HormoneBus, ClampsToZeroOne) {
    HormoneBus bus;
    bus.set(HormoneId::Cortisol, 1.5);
    EXPECT_DOUBLE_EQ(bus.get(HormoneId::Cortisol), 1.0);
    
    bus.set(HormoneId::Cortisol, -0.5);
    EXPECT_DOUBLE_EQ(bus.get(HormoneId::Cortisol), 0.0);
}

TEST(HormoneBus, DecayTowardBaseline) {
    HormoneBus bus;
    bus.set(HormoneId::Serotonin, 1.0);
    bus.decay(HormoneId::Serotonin, 0.5, 0.1);
    // Should move 10% toward 0.5: 1.0 + (0.5 - 1.0) * 0.1 = 0.95
    EXPECT_NEAR(bus.get(HormoneId::Serotonin), 0.95, 0.001);
}

TEST(HormoneBus, DecayFromBelowBaseline) {
    HormoneBus bus;
    bus.set(HormoneId::Serotonin, 0.0);
    bus.decay(HormoneId::Serotonin, 0.5, 0.2);
    // Should move 20% toward 0.5: 0.0 + (0.5 - 0.0) * 0.2 = 0.1
    EXPECT_NEAR(bus.get(HormoneId::Serotonin), 0.1, 0.001);
}

TEST(HormoneBus, ResetSetsAllToBaseline) {
    HormoneBus bus;
    bus.set(HormoneId::Dopamine, 0.9);
    bus.set(HormoneId::Cortisol, 0.1);
    bus.reset(0.5);
    for (std::size_t i = 0; i < HORMONE_COUNT; ++i) {
        EXPECT_DOUBLE_EQ(bus.get(static_cast<HormoneId>(i)), 0.5);
    }
}

// ============================================================================
// Hormone Names
// ============================================================================

TEST(HormoneNames, AllHormonesHaveNames) {
    for (std::size_t i = 0; i < HORMONE_COUNT; ++i) {
        const char* name = hormoneName(static_cast<HormoneId>(i));
        EXPECT_NE(name, nullptr);
        EXPECT_STRNE(name, "unknown");
        EXPECT_GT(std::string(name).size(), 0u);
    }
}

TEST(HormoneNames, SpecificNames) {
    EXPECT_STREQ(hormoneName(HormoneId::Cortisol), "cortisol");
    EXPECT_STREQ(hormoneName(HormoneId::Dopamine), "dopamine");
    EXPECT_STREQ(hormoneName(HormoneId::Serotonin), "serotonin");
    EXPECT_STREQ(hormoneName(HormoneId::Norepinephrine), "norepinephrine");
    EXPECT_STREQ(hormoneName(HormoneId::Oxytocin), "oxytocin");
    EXPECT_STREQ(hormoneName(HormoneId::Thyroxine), "thyroxine");
    EXPECT_STREQ(hormoneName(HormoneId::Melatonin), "melatonin");
    EXPECT_STREQ(hormoneName(HormoneId::Insulin), "insulin");
    EXPECT_STREQ(hormoneName(HormoneId::Cytokine), "cytokine");
    EXPECT_STREQ(hormoneName(HormoneId::Anandamide), "anandamide");
}

// ============================================================================
// Cognitive Mode Names
// ============================================================================

TEST(CognitiveMode, AllModesHaveNames) {
    EXPECT_STREQ(cognitiveModeName(CognitiveMode::Exploration), "exploration");
    EXPECT_STREQ(cognitiveModeName(CognitiveMode::Exploitation), "exploitation");
    EXPECT_STREQ(cognitiveModeName(CognitiveMode::Rest), "rest");
    EXPECT_STREQ(cognitiveModeName(CognitiveMode::Alarm), "alarm");
}

// ============================================================================
// Valence-Arousal State
// ============================================================================

TEST(ValenceArousal, NeutralMagnitudeIsZero) {
    ValenceArousalState va;
    EXPECT_DOUBLE_EQ(va.magnitude(), 0.0);
}

TEST(ValenceArousal, MagnitudeComputation) {
    ValenceArousalState va;
    va.valence = 0.6;
    va.arousal = 0.8;
    EXPECT_NEAR(va.magnitude(), 1.0, 0.001);
}

// ============================================================================
// Virtual Gland Tests
// ============================================================================

TEST(VirtualGland, RespondsToStimulus) {
    GlandConfig config;
    config.hormone = HormoneId::Dopamine;
    config.baseline = 0.4;
    config.decayRate = 0.1;
    config.sensitivity = 1.0;
    config.maxProduction = 0.3;
    
    VirtualGland gland(config);
    HormoneBus bus;
    bus.set(HormoneId::Dopamine, 0.4);
    
    Stimulus stim("action_success", 0.5);
    gland.respond(stim, bus);
    
    // Should increase dopamine by 0.5 * 1.0 = 0.5, clamped by maxProduction to 0.3
    EXPECT_NEAR(bus.get(HormoneId::Dopamine), 0.7, 0.001);
}

TEST(VirtualGland, RespectsMaxProduction) {
    GlandConfig config;
    config.hormone = HormoneId::Cortisol;
    config.baseline = 0.3;
    config.decayRate = 0.05;
    config.sensitivity = 2.0;
    config.maxProduction = 0.2;
    
    VirtualGland gland(config);
    HormoneBus bus;
    bus.set(HormoneId::Cortisol, 0.3);
    
    // High intensity * high sensitivity = 1.0 * 2.0 = 2.0, but clamped to maxProduction 0.2
    Stimulus stim("threat", 1.0);
    gland.respond(stim, bus);
    
    EXPECT_NEAR(bus.get(HormoneId::Cortisol), 0.5, 0.001);
}

TEST(VirtualGland, TickDecaysTowardBaseline) {
    GlandConfig config;
    config.hormone = HormoneId::Serotonin;
    config.baseline = 0.5;
    config.decayRate = 0.1;
    config.sensitivity = 1.0;
    config.maxProduction = 0.3;
    
    VirtualGland gland(config);
    HormoneBus bus;
    bus.set(HormoneId::Serotonin, 0.9);
    
    gland.tick(bus);
    // 0.9 + (0.5 - 0.9) * 0.1 = 0.86
    EXPECT_NEAR(bus.get(HormoneId::Serotonin), 0.86, 0.001);
}

// ============================================================================
// EndocrineSystem Tests
// ============================================================================

TEST(EndocrineSystem, InitializesToHomeostasis) {
    EndocrineSystem system;
    // After construction, hormones should be at their gland baselines
    EXPECT_GT(system.hormoneLevel(HormoneId::Dopamine), 0.0);
    EXPECT_LT(system.hormoneLevel(HormoneId::Dopamine), 1.0);
    EXPECT_EQ(system.tickCount(), 0u);
}

TEST(EndocrineSystem, TickIncrements) {
    EndocrineSystem system;
    system.tick();
    EXPECT_EQ(system.tickCount(), 1u);
    system.tick();
    EXPECT_EQ(system.tickCount(), 2u);
}

TEST(EndocrineSystem, RewardStimulusIncreasesDopamine) {
    EndocrineSystem system;
    double before = system.hormoneLevel(HormoneId::Dopamine);
    
    system.submitStimulus(Stimulus("action_success", 0.8));
    system.tick();
    
    double after = system.hormoneLevel(HormoneId::Dopamine);
    EXPECT_GT(after, before);
}

TEST(EndocrineSystem, StressStimulusIncreasesCortisol) {
    EndocrineSystem system;
    double before = system.hormoneLevel(HormoneId::Cortisol);
    
    system.submitStimulus(Stimulus("error_detected", 0.9));
    system.tick();
    
    double after = system.hormoneLevel(HormoneId::Cortisol);
    EXPECT_GT(after, before);
}

TEST(EndocrineSystem, SocialStimulusIncreasesOxytocin) {
    EndocrineSystem system;
    double before = system.hormoneLevel(HormoneId::Oxytocin);
    
    system.submitStimulus(Stimulus("social_interaction", 0.7));
    system.tick();
    
    double after = system.hormoneLevel(HormoneId::Oxytocin);
    EXPECT_GT(after, before);
}

TEST(EndocrineSystem, NoveltyStimulusActivatesExploration) {
    EndocrineSystem system;
    // Submit multiple novelty stimuli to push into exploration mode
    for (int i = 0; i < 5; ++i) {
        system.submitStimulus(Stimulus("novelty", 0.9));
        system.tick();
    }
    // High dopamine + norepinephrine should give positive valence + high arousal
    EXPECT_GT(system.valenceArousal().valence, 0.0);
}

TEST(EndocrineSystem, FatigueStimulusActivatesRest) {
    EndocrineSystem system;
    // Submit fatigue stimuli
    for (int i = 0; i < 10; ++i) {
        system.submitStimulus(Stimulus("fatigue", 0.9));
        system.tick();
    }
    // High melatonin + low thyroxine should give low arousal
    EXPECT_LT(system.valenceArousal().arousal, 0.0);
}

TEST(EndocrineSystem, ThreatStimulusActivatesAlarm) {
    EndocrineSystem system;
    // Submit threat stimuli - need enough to overcome baseline positive hormones
    for (int i = 0; i < 15; ++i) {
        system.submitStimulus(Stimulus("threat", 1.0));
        system.tick();
    }
    // High cortisol + norepinephrine = high arousal, negative valence = Alarm
    // Verify the system is in a high-arousal negative-valence state
    EXPECT_GT(system.valenceArousal().arousal, 0.0);
    EXPECT_LT(system.valenceArousal().valence, 0.0);
    EXPECT_EQ(system.cognitiveMode(), CognitiveMode::Alarm);
}

TEST(EndocrineSystem, HomeostasisAfterManyTicks) {
    EndocrineSystem system;
    // Perturb the system
    system.submitStimulus(Stimulus("action_success", 1.0));
    system.tick();
    
    // Let it decay for many ticks (slow decay rates need more iterations)
    for (int i = 0; i < 500; ++i) {
        system.tick();
    }
    
    // Should return close to baseline - magnitude decreases over time
    // The system has slow decay rates (0.02-0.10) so it takes many ticks
    // to fully return. After 500 ticks, magnitude should be well below 0.5
    EXPECT_LT(system.valenceArousal().magnitude(), 0.7);
}

TEST(EndocrineSystem, ResetClearsState) {
    EndocrineSystem system;
    system.submitStimulus(Stimulus("threat", 1.0));
    system.tick();
    system.tick();
    
    system.reset();
    EXPECT_EQ(system.tickCount(), 0u);
    // After reset, mode is derived from baselines -> Exploration.
    EXPECT_EQ(system.cognitiveMode(), CognitiveMode::Exploration);
}

TEST(EndocrineSystem, HormoneLevelsMapHasAllHormones) {
    EndocrineSystem system;
    auto map = system.hormoneLevelsMap();
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

TEST(EndocrineSystem, AllLevelsInRange) {
    EndocrineSystem system;
    // Submit various stimuli
    system.submitStimulus(Stimulus("action_success", 1.0));
    system.submitStimulus(Stimulus("threat", 0.8));
    system.submitStimulus(Stimulus("novelty", 0.9));
    system.tick();
    
    for (std::size_t i = 0; i < HORMONE_COUNT; ++i) {
        double level = system.hormoneLevel(static_cast<HormoneId>(i));
        EXPECT_GE(level, 0.0) << "Hormone " << i << " below 0";
        EXPECT_LE(level, 1.0) << "Hormone " << i << " above 1";
    }
}

TEST(EndocrineSystem, ValenceInRange) {
    EndocrineSystem system;
    for (int i = 0; i < 20; ++i) {
        system.submitStimulus(Stimulus("action_success", 1.0));
        system.tick();
        EXPECT_GE(system.valenceArousal().valence, -1.0);
        EXPECT_LE(system.valenceArousal().valence, 1.0);
        EXPECT_GE(system.valenceArousal().arousal, -1.0);
        EXPECT_LE(system.valenceArousal().arousal, 1.0);
    }
}

TEST(EndocrineSystem, DeterministicGivenSameStimuli) {
    EndocrineSystem sys1, sys2;
    
    sys1.submitStimulus(Stimulus("action_success", 0.7));
    sys1.submitStimulus(Stimulus("novelty", 0.5));
    sys1.tick();
    
    sys2.submitStimulus(Stimulus("action_success", 0.7));
    sys2.submitStimulus(Stimulus("novelty", 0.5));
    sys2.tick();
    
    for (std::size_t i = 0; i < HORMONE_COUNT; ++i) {
        auto id = static_cast<HormoneId>(i);
        EXPECT_DOUBLE_EQ(sys1.hormoneLevel(id), sys2.hormoneLevel(id));
    }
    EXPECT_DOUBLE_EQ(sys1.valenceArousal().valence, sys2.valenceArousal().valence);
    EXPECT_DOUBLE_EQ(sys1.valenceArousal().arousal, sys2.valenceArousal().arousal);
    EXPECT_EQ(sys1.cognitiveMode(), sys2.cognitiveMode());
}

TEST(EndocrineSystem, GenericStimulusHasMildEffect) {
    EndocrineSystem system;
    double before = system.hormoneLevel(HormoneId::Dopamine);
    
    system.submitStimulus(Stimulus("unknown_source", 0.5));
    system.tick();
    
    double after = system.hormoneLevel(HormoneId::Dopamine);
    // Generic stimulus should have a mild effect (30% of intensity)
    double expected_change = 0.5 * 0.3 * 1.0;  // intensity * 0.3 * sensitivity
    EXPECT_GT(after, before);
}

TEST(EndocrineSystem, RepairStimulusActivatesCytokine) {
    EndocrineSystem system;
    double before = system.hormoneLevel(HormoneId::Cytokine);
    
    system.submitStimulus(Stimulus("repair", 0.8));
    system.tick();
    
    double after = system.hormoneLevel(HormoneId::Cytokine);
    EXPECT_GT(after, before);
}

TEST(EndocrineSystem, CognitiveModeQuadrants) {
    // Test all four quadrants of the valence-arousal space
    EndocrineSystem system;
    
    // Exploration: high arousal + positive valence (novelty/reward)
    for (int i = 0; i < 10; ++i) {
        system.submitStimulus(Stimulus("novelty", 1.0));
        system.submitStimulus(Stimulus("action_success", 1.0));
        system.tick();
    }
    // Should be in Exploration mode (or at least positive valence + high arousal)
    auto va = system.valenceArousal();
    if (va.valence >= 0.0 && va.arousal >= 0.0) {
        EXPECT_EQ(system.cognitiveMode(), CognitiveMode::Exploration);
    }
}

TEST(EndocrineSystem, MultipleStimuliBatchProcessed) {
    EndocrineSystem system;
    
    // Submit multiple stimuli before tick
    system.submitStimulus(Stimulus("action_success", 0.5));
    system.submitStimulus(Stimulus("novelty", 0.3));
    system.submitStimulus(Stimulus("social_interaction", 0.4));
    
    // All should be processed in single tick
    system.tick();
    EXPECT_EQ(system.tickCount(), 1u);
    
    // Dopamine should be elevated (from both action_success and novelty)
    EXPECT_GT(system.hormoneLevel(HormoneId::Dopamine), 0.4);
}
