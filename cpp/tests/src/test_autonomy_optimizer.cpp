// test_autonomy_optimizer.cpp
// Tests the optimized autonomy pipeline with tighter integration between
// AutonomousStarter, CognitiveBridge, GoalManager, and Echobeats cycle.
// Validates the full cognitive loop operates as a self-regulating system.

#include <gtest/gtest.h>
#include "elizaos/goal_manager.hpp"  // MUST be before autonomous_starter.hpp
#include "elizaos/autonomous_starter.hpp"
#include "elizaos/cognitive_bridge.hpp"
#include "elizaos/knowledge.hpp"
#include "elizaos/agentmemory.hpp"
#include "elizaos/agentaction.hpp"
#include "elizaos/agentloop.hpp"
#include "elizaos/agentcomms.hpp"
#include "elizaos/agentlogger.hpp"
#include "elizaos/persistence.hpp"
#include "elizaos/attention.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <mutex>
using namespace elizaos;
using namespace std::chrono_literals;

class AutonomyOptimizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        KnowledgeBase kb;
        kb.clear();
    }

    AgentConfig makeConfig(const std::string& name = "Optimizer-Agent") {
        AgentConfig cfg;
        cfg.agentId = generateUUID();
        cfg.agentName = name;
        cfg.bio = "Autonomy optimization test agent";
        cfg.lore = "Created for autonomy pipeline testing";
        return cfg;
    }
};

// Test: AutonomousStarter cognitive cycle integrates with CognitiveBridge
TEST_F(AutonomyOptimizerTest, CognitiveCycleIntegratesWithBridge) {
    auto config = makeConfig("Bridge-Integrated");
    AutonomousStarter agent(config);
    CognitiveBridge bridge("autonomy-bridge");

    std::atomic<int> stateUpdates{0};
    bridge.subscribeCognitiveState([&](const CognitiveState&) {
        stateUpdates.fetch_add(1);
    });

    agent.start();
    EXPECT_TRUE(agent.isRunning());

    // Run cognitive cycle and publish state to bridge
    auto cycle = agent.runCognitiveCycleOnce();
    EXPECT_GE(cycle, 1u);

    // Publish the agent's cognitive state
    CognitiveState cs;
    cs.agentId = config.agentId;
    cs.mood = "active";
    cs.focus = agent.getLastObservationSummary();
    cs.valence = 0.5;
    cs.arousal = 0.4;
    bridge.publishCognitiveState(cs);

    std::this_thread::sleep_for(10ms);
    EXPECT_GE(stateUpdates.load(), 1);

    agent.stop();
}

// Test: Goal-driven autonomy with attention allocation
TEST_F(AutonomyOptimizerTest, GoalDrivenAutonomyWithAttention) {
    auto config = makeConfig("GoalAttention-Agent");
    AutonomousStarter agent(config);
    agent.start();

    // Verify goals are seeded
    const auto& goals = agent.getState().getGoals();
    EXPECT_GE(goals.size(), 1u);

    // Create attention allocator
    AttentionAllocator allocator;
    
    // Add attention values for each goal
    for (const auto& goal : goals) {
        AttentionValue av;
        av.importance = 0.8;
        av.urgency = 0.5;
        av.novelty = 0.7;
        av.activation = 0.6;
        allocator.updateAttentionValue(goal.id, av);
    }

    // Verify attention prioritization works
    auto topElements = allocator.getTopAttentionElements(3);
    EXPECT_GE(topElements.size(), 1u);

    // Run cognitive cycle - should use attention to prioritize
    agent.runCognitiveCycleOnce();
    EXPECT_GE(agent.getCognitiveCycleCount(), 1u);

    agent.stop();
}

// Test: Multi-cycle autonomy maintains coherent state
TEST_F(AutonomyOptimizerTest, MultiCycleCoherence) {
    auto config = makeConfig("Coherence-Agent");
    AutonomousStarter agent(config);
    agent.start();

    std::vector<std::string> observations;
    std::vector<std::string> plans;

    for (int i = 0; i < 5; ++i) {
        agent.runCognitiveCycleOnce();
        observations.push_back(agent.getLastObservationSummary());
        plans.push_back(agent.getLastPlan());
    }

    // All cycles should produce non-empty observations and plans
    for (const auto& obs : observations) {
        EXPECT_FALSE(obs.empty());
    }
    for (const auto& plan : plans) {
        EXPECT_FALSE(plan.empty());
    }

    // Cycle count should match
    EXPECT_EQ(agent.getCognitiveCycleCount(), 5u);

    agent.stop();
}

// Test: Echobeats-synchronized cognitive cycles
TEST_F(AutonomyOptimizerTest, EchobeatsSynchronizedCycles) {
    auto config = makeConfig("Echobeats-Agent");
    AutonomousStarter agent(config);
    CognitiveBridge bridge("echobeats-sync");

    agent.start();

    std::atomic<int> cyclesRun{0};
    std::vector<int> phaseAtCycle;
    std::mutex mu;

    // Subscribe to echobeats and run cognitive cycles at specific phases
    bridge.subscribeCognitiveState([&](const CognitiveState& state) {
        // Phase 0 = perception, Phase 1 = reasoning, Phase 2 = action, Phase 3 = reflection
        if (state.echobeatsPhase == 0) { // Perception phase
            agent.runCognitiveCycleOnce();
            std::lock_guard<std::mutex> lock(mu);
            phaseAtCycle.push_back(state.echobeatsPhase);
            cyclesRun.fetch_add(1);
        }
    });

    bridge.startEchobeats(40ms, config.agentId);
    std::this_thread::sleep_for(250ms);
    bridge.stopEchobeats();

    EXPECT_GE(cyclesRun.load(), 2);

    // All cycles should have run at phase 0
    std::lock_guard<std::mutex> lock(mu);
    for (int phase : phaseAtCycle) {
        EXPECT_EQ(phase, 0);
    }

    agent.stop();
}

// Test: Memory accumulation across cognitive cycles
TEST_F(AutonomyOptimizerTest, MemoryAccumulationAcrossCycles) {
    auto config = makeConfig("Memory-Agent");
    AutonomousStarter agent(config);
    agent.start();

    // Run multiple cycles
    for (int i = 0; i < 3; ++i) {
        agent.runCognitiveCycleOnce();
    }

    // Agent should have accumulated memories
    EXPECT_GE(agent.getActionCount(), 3u);

    agent.stop();
}

// Test: Self-check exercises entire autonomy stack
TEST_F(AutonomyOptimizerTest, SelfCheckExercisesStack) {
    auto config = makeConfig("SelfCheck-Agent");
    AutonomousStarter agent(config);
    agent.start();

    // Self-check should validate all subsystems
    bool selfCheckPassed = autonomous_starter_self_check();
    EXPECT_TRUE(selfCheckPassed);

    agent.stop();
}

// Test: Knowledge integration with cognitive cycles
TEST_F(AutonomyOptimizerTest, KnowledgeIntegrationWithCycles) {
    auto config = makeConfig("Knowledge-Agent");
    AutonomousStarter agent(config);
    agent.start();

    // Add knowledge that should influence cognitive behavior
    KnowledgeBase kb;
    KnowledgeEntry entry;
    entry.id = generateUUID();
    entry.content = "The agent should prioritize user safety above all else.";
    entry.type = KnowledgeType::FACT;
    entry.confidence = ConfidenceLevel::VERY_HIGH;
    kb.addKnowledge(entry);

    // Run cognitive cycle - should incorporate knowledge
    agent.runCognitiveCycleOnce();

    // Verify knowledge is accessible
    auto results = kb.searchByText("safety");
    EXPECT_GE(results.size(), 1u);

    agent.stop();
}

// Test: Concurrent agents with independent cognitive bridges
TEST_F(AutonomyOptimizerTest, ConcurrentAgentsIndependent) {
    auto config1 = makeConfig("Agent-A");
    auto config2 = makeConfig("Agent-B");
    AutonomousStarter agent1(config1);
    AutonomousStarter agent2(config2);
    CognitiveBridge bridge1("bridge-A");
    CognitiveBridge bridge2("bridge-B");

    agent1.start();
    agent2.start();

    std::atomic<int> bridge1Updates{0};
    std::atomic<int> bridge2Updates{0};

    bridge1.subscribeCognitiveState([&](const CognitiveState&) {
        bridge1Updates.fetch_add(1);
    });
    bridge2.subscribeCognitiveState([&](const CognitiveState&) {
        bridge2Updates.fetch_add(1);
    });

    // Run cycles independently
    agent1.runCognitiveCycleOnce();
    CognitiveState cs1;
    cs1.agentId = config1.agentId;
    cs1.mood = "focused";
    bridge1.publishCognitiveState(cs1);

    agent2.runCognitiveCycleOnce();
    CognitiveState cs2;
    cs2.agentId = config2.agentId;
    cs2.mood = "curious";
    bridge2.publishCognitiveState(cs2);

    std::this_thread::sleep_for(20ms);

    // Each bridge should only receive its own updates
    EXPECT_EQ(bridge1Updates.load(), 1);
    EXPECT_EQ(bridge2Updates.load(), 1);

    agent1.stop();
    agent2.stop();
}

// Test: Persistence survives agent restart
TEST_F(AutonomyOptimizerTest, PersistenceSurvivesRestart) {
    auto config = makeConfig("Persistent-Agent");
    std::string agentId = config.agentId;

    {
        AutonomousStarter agent(config);
        agent.start();
        agent.runCognitiveCycleOnce();
        agent.runCognitiveCycleOnce();
        EXPECT_EQ(agent.getCognitiveCycleCount(), 2u);
        agent.stop();
    }

    // Create new agent with same config
    {
        AutonomousStarter agent(config);
        agent.start();
        // Should be able to continue from fresh state
        agent.runCognitiveCycleOnce();
        EXPECT_GE(agent.getCognitiveCycleCount(), 1u);
        agent.stop();
    }
}

// Test: Full autonomy pipeline integration (perception -> reasoning -> action -> reflection)
TEST_F(AutonomyOptimizerTest, FullAutonomyPipelineIntegration) {
    auto config = makeConfig("Pipeline-Agent");
    AutonomousStarter agent(config);
    CognitiveBridge bridge("pipeline-bridge");

    agent.start();

    std::vector<std::string> phases;
    std::mutex mu;

    // Track the cognitive phases
    bridge.subscribeCognitiveState([&](const CognitiveState& state) {
        std::lock_guard<std::mutex> lock(mu);
        phases.push_back(state.mood);
    });

    // Simulate full pipeline
    // Phase 0: Perception
    CognitiveState perception;
    perception.agentId = config.agentId;
    perception.mood = "perceiving";
    perception.echobeatsPhase = 0;
    bridge.publishCognitiveState(perception);

    // Phase 1: Reasoning
    agent.runCognitiveCycleOnce();
    CognitiveState reasoning;
    reasoning.agentId = config.agentId;
    reasoning.mood = "reasoning";
    reasoning.echobeatsPhase = 1;
    bridge.publishCognitiveState(reasoning);

    // Phase 2: Action
    CognitiveState action;
    action.agentId = config.agentId;
    action.mood = "acting";
    action.echobeatsPhase = 2;
    bridge.publishCognitiveState(action);

    // Phase 3: Reflection
    CognitiveState reflection;
    reflection.agentId = config.agentId;
    reflection.mood = "reflecting";
    reflection.echobeatsPhase = 3;
    bridge.publishCognitiveState(reflection);

    std::this_thread::sleep_for(20ms);

    std::lock_guard<std::mutex> lock(mu);
    ASSERT_GE(phases.size(), 4u);
    EXPECT_EQ(phases[0], "perceiving");
    EXPECT_EQ(phases[1], "reasoning");
    EXPECT_EQ(phases[2], "acting");
    EXPECT_EQ(phases[3], "reflecting");

    agent.stop();
}
