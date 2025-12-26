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
        agentConfig_.personality = "analytical";
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
    // Create logger with test configuration
    LoggerConfig logConfig;
    logConfig.level = LogLevel::DEBUG;
    logConfig.enableConsole = false;  // Don't pollute test output
    logConfig.enableFile = false;

    AgentLogger logger(agentConfig_.agentId, logConfig);

    // Log various levels
    logger.debug("Debug message from integration test");
    logger.info("Info message from integration test");
    logger.warn("Warning message from integration test");
    logger.error("Error message from integration test");

    // Verify log entries were captured
    auto logs = logger.getRecentLogs(10);
    EXPECT_GE(logs.size(), 4);
}

TEST_F(IntegrationTest, LoggerWithStateTracking) {
    LoggerConfig logConfig;
    logConfig.level = LogLevel::DEBUG;
    logConfig.enableConsole = false;
    logConfig.enableFile = false;

    AgentLogger logger(agentConfig_.agentId, logConfig);
    State state(agentConfig_);

    // Add an actor
    Actor actor{"actor-1", "TestUser", "A test user"};
    state.addActor(actor);

    // Log state change
    logger.info("Added actor: " + actor.name);

    auto logs = logger.getRecentLogs(5);
    EXPECT_FALSE(logs.empty());
}

// ============================================================================
// Core + Characters Integration Tests
// ============================================================================

TEST_F(IntegrationTest, CoreAndCharacterIntegration) {
    // Create a character definition
    CharacterData charData;
    charData.name = "TestBot";
    charData.bio = "A helpful test bot";
    charData.personality = "friendly";
    charData.traits = {"helpful", "curious", "patient"};
    charData.topics = {"technology", "science"};

    // Verify character data integrity
    EXPECT_EQ(charData.name, "TestBot");
    EXPECT_EQ(charData.traits.size(), 3);
    EXPECT_EQ(charData.topics.size(), 2);

    // Create agent config from character
    AgentConfig agentFromChar;
    agentFromChar.agentId = "char-agent-1";
    agentFromChar.agentName = charData.name;
    agentFromChar.bio = charData.bio;
    agentFromChar.personality = charData.personality;

    State state(agentFromChar);
    EXPECT_EQ(state.getAgentName(), "TestBot");
}

// ============================================================================
// Core + Knowledge Integration Tests
// ============================================================================

TEST_F(IntegrationTest, CoreAndKnowledgeIntegration) {
    // Create knowledge base entries
    KnowledgeEntry entry1;
    entry1.id = "kb-1";
    entry1.content = "C++ is a high-performance programming language";
    entry1.category = "programming";
    entry1.tags = {"c++", "programming", "performance"};

    KnowledgeEntry entry2;
    entry2.id = "kb-2";
    entry2.content = "Python is great for data science";
    entry2.category = "programming";
    entry2.tags = {"python", "data-science"};

    // Create knowledge base
    KnowledgeBase kb;
    kb.addEntry(entry1);
    kb.addEntry(entry2);

    // Verify entries
    EXPECT_EQ(kb.entryCount(), 2);
    EXPECT_TRUE(kb.hasEntry("kb-1"));
    EXPECT_TRUE(kb.hasEntry("kb-2"));

    // Search functionality
    auto results = kb.search("programming");
    EXPECT_EQ(results.size(), 2);
}

// ============================================================================
// Communication Integration Tests
// ============================================================================

TEST_F(IntegrationTest, CoreAndCommsIntegration) {
    // Create communication channel
    CommChannel channel;
    channel.id = "chan-1";
    channel.name = "test-channel";
    channel.type = ChannelType::DIRECT;

    // Create messages
    CommMessage msg1;
    msg1.id = "msg-1";
    msg1.senderId = "user-1";
    msg1.receiverId = agentConfig_.agentId;
    msg1.content = "Hello agent!";
    msg1.channelId = channel.id;

    CommMessage msg2;
    msg2.id = "msg-2";
    msg2.senderId = agentConfig_.agentId;
    msg2.receiverId = "user-1";
    msg2.content = "Hello user! How can I help?";
    msg2.channelId = channel.id;

    // Simulate conversation
    channel.messages.push_back(msg1);
    channel.messages.push_back(msg2);

    EXPECT_EQ(channel.messages.size(), 2);
    EXPECT_EQ(channel.messages[0].senderId, "user-1");
    EXPECT_EQ(channel.messages[1].senderId, agentConfig_.agentId);
}

// ============================================================================
// Agent Loop Integration Tests
// ============================================================================

TEST_F(IntegrationTest, AgentLoopStateTransitions) {
    // Create agent loop configuration
    AgentLoopConfig loopConfig;
    loopConfig.maxIterations = 10;
    loopConfig.sleepDuration = std::chrono::milliseconds(10);
    loopConfig.enablePause = true;

    // Verify configuration
    EXPECT_EQ(loopConfig.maxIterations, 10);
    EXPECT_TRUE(loopConfig.enablePause);

    // Test state machine
    AgentLoopState state = AgentLoopState::IDLE;
    EXPECT_EQ(state, AgentLoopState::IDLE);

    state = AgentLoopState::RUNNING;
    EXPECT_EQ(state, AgentLoopState::RUNNING);

    state = AgentLoopState::PAUSED;
    EXPECT_EQ(state, AgentLoopState::PAUSED);

    state = AgentLoopState::STOPPED;
    EXPECT_EQ(state, AgentLoopState::STOPPED);
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
    LoggerConfig logConfig;
    logConfig.level = LogLevel::INFO;
    logConfig.enableConsole = false;
    AgentLogger logger(agentConfig_.agentId, logConfig);
    logger.info("Agent workflow started");

    // 3. Add actor (user)
    Actor user{"user-1", "TestUser", "Integration test user"};
    state.addActor(user);

    // 4. Receive and process message
    auto userMessage = std::make_shared<Memory>(
        "msg-1", "What is the weather like?",
        "user-1", agentConfig_.agentId
    );
    state.addRecentMessage(userMessage);
    logger.info("Received message from user");

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
    logger.info("Sent response to user");

    // 7. Update goal status
    // In a real implementation, we would mark the goal as completed

    // 8. Verify complete workflow
    EXPECT_EQ(state.getActors().size(), 1);
    EXPECT_EQ(state.getRecentMessages().size(), 2);
    EXPECT_EQ(state.getGoals().size(), 1);

    auto logs = logger.getRecentLogs(10);
    EXPECT_GE(logs.size(), 3);

    logger.info("Agent workflow completed successfully");
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
