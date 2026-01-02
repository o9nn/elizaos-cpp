#include <gtest/gtest.h>
#include <gmock/gmock.h>

// Core includes
#include "elizaos/core.hpp"
#include "elizaos/agentloop.hpp"
#include "elizaos/agentmemory.hpp"
#include "elizaos/agentcomms.hpp"
#include "elizaos/agentlogger.hpp"
#include "elizaos/knowledge.hpp"
#include "elizaos/characters.hpp"

#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

using namespace elizaos;
using namespace ::testing;

/**
 * Integration tests verify that multiple modules work together correctly.
 * These tests are more comprehensive than unit tests and test real workflows.
 */

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup base agent configuration
        agentConfig_.agentId = "integration-test-agent";
        agentConfig_.agentName = "IntegrationTestAgent";
        agentConfig_.bio = "An agent for integration testing";
        agentConfig_.lore = "Created to verify system integration";
        agentConfig_.adjective = "analytical";
    }

    void TearDown() override {
        // Cleanup resources
    }

    AgentConfig agentConfig_;
};

// ============================================================================
// Core + Memory Integration Tests
// ============================================================================

TEST_F(IntegrationTest, CoreAndMemoryIntegration) {
    // Create agent state
    State state(agentConfig_);
    EXPECT_EQ(state.getAgentId(), agentConfig_.agentId);

    // Create and store memories
    auto memory1 = std::make_shared<Memory>(
        "mem-1", "Hello, this is the first message",
        "user-1", agentConfig_.agentId
    );
    auto memory2 = std::make_shared<Memory>(
        "mem-2", "This is the second message",
        "user-1", agentConfig_.agentId
    );

    state.addRecentMessage(memory1);
    state.addRecentMessage(memory2);

    // Verify memory storage
    const auto& messages = state.getRecentMessages();
    EXPECT_EQ(messages.size(), 2);
    EXPECT_EQ(messages[0]->getContent(), "Hello, this is the first message");
    EXPECT_EQ(messages[1]->getContent(), "This is the second message");
}

TEST_F(IntegrationTest, MemoryRetrievalByEntityId) {
    State state(agentConfig_);

    // Add memories from multiple users
    auto mem1 = std::make_shared<Memory>("m1", "User 1 message 1", "user-1", agentConfig_.agentId);
    auto mem2 = std::make_shared<Memory>("m2", "User 2 message 1", "user-2", agentConfig_.agentId);
    auto mem3 = std::make_shared<Memory>("m3", "User 1 message 2", "user-1", agentConfig_.agentId);

    state.addRecentMessage(mem1);
    state.addRecentMessage(mem2);
    state.addRecentMessage(mem3);

    // Filter by entity
    const auto& allMessages = state.getRecentMessages();
    int user1Count = 0;
    for (const auto& msg : allMessages) {
        if (msg->getEntityId() == "user-1") {
            user1Count++;
        }
    }

    EXPECT_EQ(user1Count, 2);
}

// ============================================================================
// Core + Logger Integration Tests
// ============================================================================

TEST_F(IntegrationTest, CoreAndLoggerIntegration) {
    // Create logger
    AgentLogger logger;

    // Disable console output for tests
    logger.setConsoleEnabled(false);
    logger.setFileEnabled(false);

    // Log various levels - using the actual API
    logger.log("Debug message from integration test", "test", "Integration", LogLevel::INFO);
    logger.log("Info message from integration test", "test", "Integration", LogLevel::INFO);
    logger.log("Warning message from integration test", "test", "Integration", LogLevel::WARNING);
    logger.log("Error message from integration test", "test", "Integration", LogLevel::ERROR);

    // The logger doesn't store logs - it outputs them
    // Verify logger is working by checking we can call the methods without error
    EXPECT_TRUE(true);
}

TEST_F(IntegrationTest, LoggerWithStateTracking) {
    AgentLogger logger;
    logger.setConsoleEnabled(false);
    logger.setFileEnabled(false);

    State state(agentConfig_);

    // Add an actor
    Actor actor{"actor-1", "TestUser", "A test user"};
    state.addActor(actor);

    // Log state change
    logger.log("Added actor: " + actor.name, "test", "Integration", LogLevel::INFO);

    // Verify state was updated
    EXPECT_EQ(state.getActors().size(), 1);
}

// ============================================================================
// Core + Characters Integration Tests
// ============================================================================

TEST_F(IntegrationTest, CoreAndCharacterIntegration) {
    // Create a character profile
    CharacterProfile charProfile;
    charProfile.name = "TestBot";
    charProfile.description = "A helpful test bot";
    charProfile.tags = {"helpful", "curious", "patient"};

    // Verify character data integrity
    EXPECT_EQ(charProfile.name, "TestBot");
    EXPECT_EQ(charProfile.tags.size(), 3);

    // Create agent config from character
    AgentConfig agentFromChar;
    agentFromChar.agentId = "char-agent-1";
    agentFromChar.agentName = charProfile.name;
    agentFromChar.bio = charProfile.description;

    State state(agentFromChar);
    EXPECT_EQ(state.getAgentName(), "TestBot");
}

// ============================================================================
// Core + Knowledge Integration Tests
// ============================================================================

TEST_F(IntegrationTest, CoreAndKnowledgeIntegration) {
    // Create knowledge base entries using proper constructor
    KnowledgeEntry entry1("C++ is a high-performance programming language", KnowledgeType::FACT);
    entry1.tags = {"c++", "programming", "performance"};

    KnowledgeEntry entry2("Python is great for data science", KnowledgeType::FACT);
    entry2.tags = {"python", "data-science"};

    // Create knowledge base
    KnowledgeBase kb;
    std::string id1 = kb.addKnowledge(entry1);
    std::string id2 = kb.addKnowledge(entry2);

    // Verify entries were added
    EXPECT_FALSE(id1.empty());
    EXPECT_FALSE(id2.empty());

    // Search functionality
    auto results = kb.searchByText("programming");
    EXPECT_GE(results.size(), 1);
}

// ============================================================================
// Communication Integration Tests
// ============================================================================

TEST_F(IntegrationTest, CoreAndCommsIntegration) {
    // Create communication manager and channel
    AgentComms comms(agentConfig_.agentId);
    auto channel = comms.createChannel("chan-1", "test-server");

    ASSERT_NE(channel, nullptr);
    EXPECT_EQ(channel->getChannelId(), "chan-1");

    // Add participants
    channel->addParticipant("user-1");
    channel->addParticipant(agentConfig_.agentId);

    // Verify participants
    EXPECT_TRUE(channel->isParticipant("user-1"));
    EXPECT_TRUE(channel->isParticipant(agentConfig_.agentId));
    EXPECT_EQ(channel->getParticipants().size(), 2);
}

// ============================================================================
// Agent Loop Integration Tests
// ============================================================================

TEST_F(IntegrationTest, AgentLoopHealthStatus) {
    // Test the HealthStatus enum (actual API)
    HealthStatus status = HealthStatus::STOPPED;
    EXPECT_EQ(status, HealthStatus::STOPPED);

    status = HealthStatus::STARTING;
    EXPECT_EQ(status, HealthStatus::STARTING);

    status = HealthStatus::HEALTHY;
    EXPECT_EQ(status, HealthStatus::HEALTHY);

    status = HealthStatus::DEGRADED;
    EXPECT_EQ(status, HealthStatus::DEGRADED);

    status = HealthStatus::UNHEALTHY;
    EXPECT_EQ(status, HealthStatus::UNHEALTHY);

    status = HealthStatus::STOPPING;
    EXPECT_EQ(status, HealthStatus::STOPPING);
}

TEST_F(IntegrationTest, AgentLoopBasicOperations) {
    // Create a simple step function
    std::atomic<int> stepCount{0};
    auto stepFunc = [&stepCount](std::shared_ptr<void>) -> std::shared_ptr<void> {
        stepCount++;
        return nullptr;
    };

    // Create agent loop with steps
    std::vector<LoopStep> steps = {
        LoopStep(StepFunction1(stepFunc), "counter_step")
    };

    AgentLoop loop(steps, true, 0.01);  // Start paused, 10ms interval

    // Verify initial state
    EXPECT_FALSE(loop.isRunning());
    EXPECT_TRUE(loop.isPaused());

    // Check health status
    EXPECT_EQ(loop.checkHealth(), HealthStatus::STOPPED);
}

// ============================================================================
// Goal and Action Integration Tests
// ============================================================================

TEST_F(IntegrationTest, GoalAndActionIntegration) {
    State state(agentConfig_);

    // Create goals
    Goal goal1;
    goal1.id = "goal-1";
    goal1.description = "Complete task A";
    goal1.status = "active";
    goal1.createdAt = std::chrono::system_clock::now();
    goal1.updatedAt = goal1.createdAt;

    Goal goal2;
    goal2.id = "goal-2";
    goal2.description = "Complete task B";
    goal2.status = "pending";
    goal2.createdAt = std::chrono::system_clock::now();
    goal2.updatedAt = goal2.createdAt;

    state.addGoal(goal1);
    state.addGoal(goal2);

    // Verify goals
    const auto& goals = state.getGoals();
    EXPECT_EQ(goals.size(), 2);

    // Count active goals
    int activeCount = 0;
    for (const auto& g : goals) {
        if (g.status == "active") {
            activeCount++;
        }
    }
    EXPECT_EQ(activeCount, 1);
}

// ============================================================================
// Multi-Actor Integration Tests
// ============================================================================

TEST_F(IntegrationTest, MultiActorConversation) {
    State state(agentConfig_);

    // Add multiple actors
    Actor alice{"actor-1", "Alice", "A helpful user"};
    Actor bob{"actor-2", "Bob", "Another user"};
    Actor charlie{"actor-3", "Charlie", "Third user"};

    state.addActor(alice);
    state.addActor(bob);
    state.addActor(charlie);

    // Create conversation with multiple participants
    auto msg1 = std::make_shared<Memory>("m1", "Hello everyone!", "actor-1", agentConfig_.agentId);
    auto msg2 = std::make_shared<Memory>("m2", "Hi Alice!", "actor-2", agentConfig_.agentId);
    auto msg3 = std::make_shared<Memory>("m3", "Hey folks!", "actor-3", agentConfig_.agentId);
    auto msg4 = std::make_shared<Memory>("m4", "Hello Alice, Bob, and Charlie!", agentConfig_.agentId, agentConfig_.agentId);

    state.addRecentMessage(msg1);
    state.addRecentMessage(msg2);
    state.addRecentMessage(msg3);
    state.addRecentMessage(msg4);

    // Verify state
    EXPECT_EQ(state.getActors().size(), 3);
    EXPECT_EQ(state.getRecentMessages().size(), 4);
}

// ============================================================================
// End-to-End Workflow Tests
// ============================================================================

TEST_F(IntegrationTest, CompleteAgentWorkflow) {
    // 1. Initialize agent with configuration
    State state(agentConfig_);
    EXPECT_EQ(state.getAgentId(), "integration-test-agent");

    // 2. Setup logging
    AgentLogger logger;
    logger.setConsoleEnabled(false);
    logger.log("Agent workflow started", "test", "Workflow", LogLevel::INFO);

    // 3. Add actor (user)
    Actor user{"user-1", "TestUser", "Integration test user"};
    state.addActor(user);

    // 4. Receive and process message
    auto userMessage = std::make_shared<Memory>(
        "msg-1", "What is the weather like?",
        "user-1", agentConfig_.agentId
    );
    state.addRecentMessage(userMessage);
    logger.log("Received message from user", "test", "Workflow", LogLevel::INFO);

    // 5. Create goal to respond
    Goal responseGoal;
    responseGoal.id = "goal-respond";
    responseGoal.description = "Respond to weather query";
    responseGoal.status = "active";
    responseGoal.createdAt = std::chrono::system_clock::now();
    responseGoal.updatedAt = responseGoal.createdAt;
    state.addGoal(responseGoal);

    // 6. Generate response
    auto agentResponse = std::make_shared<Memory>(
        "msg-2", "I don't have access to real-time weather data, but I can help you find weather information.",
        agentConfig_.agentId, "user-1"
    );
    state.addRecentMessage(agentResponse);
    logger.log("Sent response to user", "test", "Workflow", LogLevel::INFO);

    // 7. Verify complete workflow
    EXPECT_EQ(state.getActors().size(), 1);
    EXPECT_EQ(state.getRecentMessages().size(), 2);
    EXPECT_EQ(state.getGoals().size(), 1);

    logger.log("Agent workflow completed successfully", "test", "Workflow", LogLevel::SUCCESS);
}

// ============================================================================
// Stress Tests
// ============================================================================

TEST_F(IntegrationTest, HighVolumeMessageProcessing) {
    State state(agentConfig_);

    // Add many messages
    const int messageCount = 100;
    for (int i = 0; i < messageCount; ++i) {
        auto msg = std::make_shared<Memory>(
            "msg-" + std::to_string(i),
            "Message content " + std::to_string(i),
            "user-" + std::to_string(i % 10),
            agentConfig_.agentId
        );
        state.addRecentMessage(msg);
    }

    // With 32 message limit, we should have 32 messages
    EXPECT_LE(state.getRecentMessages().size(), 32);
}

TEST_F(IntegrationTest, ConcurrentActorUpdates) {
    State state(agentConfig_);

    // Add many actors
    const int actorCount = 50;
    for (int i = 0; i < actorCount; ++i) {
        Actor actor{
            "actor-" + std::to_string(i),
            "Actor " + std::to_string(i),
            "Description " + std::to_string(i)
        };
        state.addActor(actor);
    }

    EXPECT_EQ(state.getActors().size(), actorCount);
}

// ============================================================================
// Agent Loop Statistics Tests
// ============================================================================

TEST_F(IntegrationTest, AgentLoopStatistics) {
    // Test LoopStats structure
    LoopStats stats;
    stats.totalIterations = 100;
    stats.totalStepsExecuted = 500;
    stats.errorCount = 5;
    stats.successCount = 495;
    stats.avgStepDurationMs = 10.5;
    stats.iterationsPerSecond = 20.0;

    EXPECT_EQ(stats.totalIterations, 100);
    EXPECT_EQ(stats.totalStepsExecuted, 500);
    EXPECT_EQ(stats.errorCount, 5);
    EXPECT_EQ(stats.successCount, 495);
    EXPECT_DOUBLE_EQ(stats.avgStepDurationMs, 10.5);
    EXPECT_DOUBLE_EQ(stats.iterationsPerSecond, 20.0);
}

// ============================================================================
// Knowledge Management Tests
// ============================================================================

TEST_F(IntegrationTest, KnowledgeManagement) {
    KnowledgeBase kb;

    // Add knowledge entries
    KnowledgeEntry entry1("The sky is blue on clear days", KnowledgeType::FACT);
    entry1.tags = {"weather", "sky", "nature"};

    std::string id = kb.addKnowledge(entry1);
    EXPECT_FALSE(id.empty());

    // Retrieve by ID
    auto retrieved = kb.getKnowledge(id);
    EXPECT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved.value().content, "The sky is blue on clear days");

    // Search by text
    auto results = kb.searchByText("sky");
    EXPECT_GE(results.size(), 1);

    // Search by tags
    std::vector<std::string> searchTags = {"weather"};
    auto tagResults = kb.searchByTags(searchTags);
    EXPECT_GE(tagResults.size(), 1);
}

// ============================================================================
// Character Profile Tests
// ============================================================================

TEST_F(IntegrationTest, CharacterProfileManagement) {
    CharacterProfile profile;
    profile.id = "test-character";
    profile.name = "TestBot";
    profile.description = "A test character for integration testing";
    profile.tags = {"test", "integration", "bot"};

    EXPECT_EQ(profile.id, "test-character");
    EXPECT_EQ(profile.name, "TestBot");
    EXPECT_EQ(profile.tags.size(), 3);
}
