#include <gtest/gtest.h>
#include "elizaos/elizas_world.hpp"
#include <chrono>
#include <thread>

namespace elizaos {
namespace test {

class ElizasWorldTest : public ::testing::Test {
protected:
    void SetUp() override {
        world = std::make_unique<ElizasWorld>();
        
        // Create test environment
        testEnv.id = "test-env";
        testEnv.name = "Test Environment";
        testEnv.description = "Environment for testing";
        testEnv.type = "test";
        testEnv.center = WorldPosition(0, 0, 0);
        testEnv.radius = 50.0;
        testEnv.createdAt = std::chrono::system_clock::now();
        testEnv.active = true;
        
        // Create test agent
        testAgent.agentId = "test-agent-001";
        testAgent.name = "Test Agent";
        testAgent.type = "test";
        testAgent.position = WorldPosition(10, 10, 0);
        testAgent.interactionRadius = 15.0;
        testAgent.lastUpdate = std::chrono::system_clock::now();
        testAgent.online = true;
    }
    
    std::unique_ptr<ElizasWorld> world;
    WorldEnvironment testEnv;
    WorldAgent testAgent;
};

TEST_F(ElizasWorldTest, WorldPositionOperations) {
    WorldPosition pos1(0, 0, 0);
    WorldPosition pos2(3, 4, 0);
    
    // Test distance calculation
    EXPECT_DOUBLE_EQ(pos1.distanceTo(pos2), 5.0);
    
    // Test interpolation
    WorldPosition mid = pos1.interpolate(pos2, 0.5);
    EXPECT_DOUBLE_EQ(mid.x, 1.5);
    EXPECT_DOUBLE_EQ(mid.y, 2.0);
    EXPECT_DOUBLE_EQ(mid.z, 0.0);
    
    // Test edge case interpolation
    WorldPosition start = pos1.interpolate(pos2, 0.0);
    EXPECT_EQ(start.x, pos1.x);
    EXPECT_EQ(start.y, pos1.y);
    
    WorldPosition end = pos1.interpolate(pos2, 1.0);
    EXPECT_EQ(end.x, pos2.x);
    EXPECT_EQ(end.y, pos2.y);
}

TEST_F(ElizasWorldTest, EnvironmentManagement) {
    // Test adding environment
    EXPECT_TRUE(world->addEnvironment(testEnv));
    EXPECT_EQ(world->getEnvironmentCount(), 1);
    
    // Test adding duplicate environment (should fail)
    EXPECT_FALSE(world->addEnvironment(testEnv));
    EXPECT_EQ(world->getEnvironmentCount(), 1);
    
    // Test getting environment
    auto retrieved = world->getEnvironment(testEnv.id);
    EXPECT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->id, testEnv.id);
    EXPECT_EQ(retrieved->name, testEnv.name);
    
    // Test getting non-existent environment
    auto nonExistent = world->getEnvironment("non-existent");
    EXPECT_FALSE(nonExistent.has_value());
    
    // Test removing environment
    EXPECT_TRUE(world->removeEnvironment(testEnv.id));
    EXPECT_EQ(world->getEnvironmentCount(), 0);
    
    // Test removing non-existent environment
    EXPECT_FALSE(world->removeEnvironment("non-existent"));
}

TEST_F(ElizasWorldTest, EnvironmentContainment) {
    // Test position inside environment
    EXPECT_TRUE(testEnv.containsPosition(WorldPosition(0, 0, 0))); // Center
    EXPECT_TRUE(testEnv.containsPosition(WorldPosition(30, 0, 0))); // Inside radius
    EXPECT_FALSE(testEnv.containsPosition(WorldPosition(60, 0, 0))); // Outside radius
    
    // Test distance calculation
    EXPECT_DOUBLE_EQ(testEnv.distanceFromCenter(WorldPosition(0, 0, 0)), 0.0);
    EXPECT_DOUBLE_EQ(testEnv.distanceFromCenter(WorldPosition(50, 0, 0)), 50.0);
}

TEST_F(ElizasWorldTest, AgentManagement) {
    // Add environment first
    world->addEnvironment(testEnv);
    
    // Test adding agent
    EXPECT_TRUE(world->addAgent(testAgent));
    EXPECT_EQ(world->getAgentCount(), 1);
    
    // Test adding duplicate agent
    EXPECT_FALSE(world->addAgent(testAgent));
    EXPECT_EQ(world->getAgentCount(), 1);
    
    // Test getting agent
    auto retrieved = world->getAgent(testAgent.agentId);
    EXPECT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->agentId, testAgent.agentId);
    EXPECT_EQ(retrieved->name, testAgent.name);
    EXPECT_EQ(retrieved->currentEnvironment, testEnv.id);
    
    // Test removing agent
    EXPECT_TRUE(world->removeAgent(testAgent.agentId));
    EXPECT_EQ(world->getAgentCount(), 0);
}

TEST_F(ElizasWorldTest, AgentMovement) {
    world->addAgent(testAgent);
    
    // Test moving agent
    WorldPosition target(20, 20, 0);
    EXPECT_TRUE(world->moveAgent(testAgent.agentId, target, 5.0));
    
    auto agent = world->getAgent(testAgent.agentId);
    EXPECT_TRUE(agent.has_value());
    
    // Velocity should be set towards target
    EXPECT_GT(agent->velocity.x, 0);
    EXPECT_GT(agent->velocity.y, 0);
    
    // Test teleporting agent
    WorldPosition teleportTarget(100, 100, 10);
    EXPECT_TRUE(world->teleportAgent(testAgent.agentId, teleportTarget));
    
    agent = world->getAgent(testAgent.agentId);
    EXPECT_TRUE(agent.has_value());
    EXPECT_EQ(agent->position.x, teleportTarget.x);
    EXPECT_EQ(agent->position.y, teleportTarget.y);
    EXPECT_EQ(agent->position.z, teleportTarget.z);
    
    // Velocity should be reset
    EXPECT_EQ(agent->velocity.x, 0.0);
    EXPECT_EQ(agent->velocity.y, 0.0);
    EXPECT_EQ(agent->velocity.z, 0.0);
}

TEST_F(ElizasWorldTest, AgentInteraction) {
    WorldAgent agent1 = testAgent;
    agent1.agentId = "agent-001";
    agent1.position = WorldPosition(0, 0, 0);
    agent1.interactionRadius = 10.0;
    
    WorldAgent agent2 = testAgent;
    agent2.agentId = "agent-002";
    agent2.position = WorldPosition(5, 0, 0); // Within interaction range
    agent2.interactionRadius = 10.0;
    
    // Test interaction capability - agents should be able to interact
    EXPECT_TRUE(agent1.canInteractWith(agent2));
    EXPECT_TRUE(agent2.canInteractWith(agent1));
    
    // Move agent2 out of range
    agent2.position = WorldPosition(25, 0, 0);
    EXPECT_FALSE(agent1.canInteractWith(agent2));
}

TEST_F(ElizasWorldTest, SpatialQueries) {
    world->addEnvironment(testEnv);
    
    // Add multiple agents
    WorldAgent agent1 = testAgent;
    agent1.agentId = "agent-001";
    agent1.position = WorldPosition(0, 0, 0);
    world->addAgent(agent1);
    
    WorldAgent agent2 = testAgent;
    agent2.agentId = "agent-002";
    agent2.position = WorldPosition(10, 10, 0);
    world->addAgent(agent2);
    
    WorldAgent agent3 = testAgent;
    agent3.agentId = "agent-003";
    agent3.position = WorldPosition(100, 100, 0);
    world->addAgent(agent3);
    
    // Test agents near position
    auto nearAgents = world->getAgentsNearPosition(WorldPosition(0, 0, 0), 20.0);
    EXPECT_EQ(nearAgents.size(), 2); // agent1 and agent2
    
    // Test agents in environment
    auto envAgents = world->getAgentsInEnvironment(testEnv.id);
    EXPECT_EQ(envAgents.size(), 2); // agent1 and agent2 (within environment radius)
    
    // Test finding environment for position
    std::string envId = world->findEnvironmentForPosition(WorldPosition(0, 0, 0));
    EXPECT_EQ(envId, testEnv.id);
    
    std::string noEnvId = world->findEnvironmentForPosition(WorldPosition(200, 200, 0));
    EXPECT_TRUE(noEnvId.empty());
}

TEST_F(ElizasWorldTest, InteractionRecording) {
    world->addAgent(testAgent);
    
    WorldInteraction interaction;
    interaction.id = "test-interaction";
    interaction.initiatorId = testAgent.agentId;
    interaction.targetId = "target-agent";
    interaction.type = "test";
    interaction.location = WorldPosition(0, 0, 0);
    interaction.timestamp = std::chrono::system_clock::now();
    interaction.completed = false;
    
    // Test recording interaction
    EXPECT_TRUE(world->recordInteraction(interaction));
    EXPECT_EQ(world->getInteractionCount(), 1);
    
    // Test getting interaction history
    auto history = world->getInteractionHistory(testAgent.agentId);
    EXPECT_EQ(history.size(), 1);
    EXPECT_EQ(history[0].id, interaction.id);
    
    // Test getting recent interactions
    auto recent = world->getRecentInteractions(10);
    EXPECT_EQ(recent.size(), 1);
}

TEST_F(ElizasWorldTest, WorldSimulation) {
    world->addEnvironment(testEnv);
    world->addAgent(testAgent);
    
    // Set agent velocity
    WorldPosition velocity(10, 0, 0);
    world->setAgentVelocity(testAgent.agentId, velocity);
    
    auto initialAgent = world->getAgent(testAgent.agentId);
    EXPECT_TRUE(initialAgent.has_value());
    double initialX = initialAgent->position.x;
    
    // Update simulation
    world->update(0.1); // 100ms
    
    auto updatedAgent = world->getAgent(testAgent.agentId);
    EXPECT_TRUE(updatedAgent.has_value());
    
    // Agent should have moved
    EXPECT_GT(updatedAgent->position.x, initialX);
}

TEST_F(ElizasWorldTest, WorldBounds) {
    // Set world bounds
    world->setWorldBounds(WorldPosition(-100, -100, -10), WorldPosition(100, 100, 10));
    
    // Agent within bounds should be added
    WorldAgent inBoundsAgent = testAgent;
    inBoundsAgent.position = WorldPosition(50, 50, 5);
    EXPECT_TRUE(world->addAgent(inBoundsAgent));
    
    // Agent outside bounds should fail to be added
    WorldAgent outOfBoundsAgent = testAgent;
    outOfBoundsAgent.agentId = "out-of-bounds";
    outOfBoundsAgent.position = WorldPosition(200, 200, 20);
    EXPECT_FALSE(world->addAgent(outOfBoundsAgent));
}

TEST_F(ElizasWorldTest, Statistics) {
    world->addEnvironment(testEnv);
    world->addAgent(testAgent);
    
    // Add a second agent for more realistic testing
    WorldAgent secondAgent = testAgent;
    secondAgent.agentId = "second-agent";
    secondAgent.name = "Second Agent";
    secondAgent.position = WorldPosition(20, 20, 0);
    world->addAgent(secondAgent);
    
    // Test environment populations
    auto populations = world->getEnvironmentPopulations();
    EXPECT_EQ(populations[testEnv.id], 2); // Both agents should be in environment
    
    // Add interaction between actual agents
    WorldInteraction interaction;
    interaction.id = "activity-test";
    interaction.initiatorId = testAgent.agentId;
    interaction.targetId = secondAgent.agentId;  // Use actual second agent
    interaction.type = "test";
    interaction.location = WorldPosition(0, 0, 0);
    interaction.timestamp = std::chrono::system_clock::now();
    
    world->recordInteraction(interaction);
    
    EXPECT_GT(world->getWorldActivity(), 0.0);
    
    auto activeAgents = world->getMostActiveAgents(5);
    EXPECT_GE(activeAgents.size(), 1); // At least one active agent
    // Both agents should be active (both participated in interaction)
    EXPECT_EQ(activeAgents.size(), 2);
}

} // namespace test
} // namespace elizaos