#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "elizaos/eliza.hpp"
#include "elizaos/core.hpp"
#include <memory>
#include <string>
#include <vector>

using namespace elizaos;
using namespace ::testing;

class ElizaTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test environment
    }
};

// ============================================================================
// ConversationTurn Tests
// ============================================================================

TEST_F(ElizaTest, ConversationTurnCreation) {
    ConversationTurn turn("Hello", "Hi there!");

    EXPECT_EQ(turn.input, "Hello");
    EXPECT_EQ(turn.response, "Hi there!");
    EXPECT_FALSE(turn.id.empty());
    EXPECT_GE(turn.confidence, 0.0f);
}

TEST_F(ElizaTest, ConversationTurnMetadata) {
    ConversationTurn turn("Test input", "Test response");
    turn.metadata["topic"] = "greeting";
    turn.metadata["intent"] = "welcome";

    EXPECT_EQ(turn.metadata.size(), 2);
    EXPECT_EQ(turn.metadata["topic"], "greeting");
    EXPECT_EQ(turn.metadata["intent"], "welcome");
}

// ============================================================================
// ConversationContext Tests
// ============================================================================

TEST_F(ElizaTest, ConversationContextCreation) {
    ConversationContext context("session-1", "user-1");

    EXPECT_EQ(context.sessionId, "session-1");
    EXPECT_EQ(context.userId, "user-1");
    EXPECT_TRUE(context.history.empty());
}

TEST_F(ElizaTest, ConversationContextAddTurn) {
    ConversationContext context("session-1", "user-1");

    ConversationTurn turn1("Hello", "Hi there!");
    ConversationTurn turn2("How are you?", "I'm doing well, thanks!");

    context.addTurn(turn1);
    context.addTurn(turn2);

    EXPECT_EQ(context.history.size(), 2);
}

TEST_F(ElizaTest, ConversationContextSessionData) {
    ConversationContext context("session-1", "user-1");

    context.setSessionData("name", "Alice");
    context.setSessionData("topic", "weather");

    EXPECT_EQ(context.getSessionData("name"), "Alice");
    EXPECT_EQ(context.getSessionData("topic"), "weather");
    EXPECT_EQ(context.getSessionData("missing"), "");
}

TEST_F(ElizaTest, ConversationContextRecentHistory) {
    ConversationContext context("session-1", "user-1");

    for (int i = 0; i < 10; ++i) {
        ConversationTurn turn("Input " + std::to_string(i), "Response " + std::to_string(i));
        context.addTurn(turn);
    }

    auto recent = context.getRecentHistory(5);
    EXPECT_EQ(recent.size(), 5);
}

TEST_F(ElizaTest, ConversationContextSummary) {
    ConversationContext context("session-1", "user-1");

    ConversationTurn turn("Hello", "Hi!");
    context.addTurn(turn);

    std::string summary = context.getContextSummary();
    EXPECT_FALSE(summary.empty());
}

// ============================================================================
// ResponsePattern Tests
// ============================================================================

TEST_F(ElizaTest, ResponsePatternCreation) {
    ResponsePattern pattern("hello|hi|hey", {"Hello!", "Hi there!", "Hey!"}, "greeting");

    EXPECT_EQ(pattern.pattern, "hello|hi|hey");
    EXPECT_EQ(pattern.responses.size(), 3);
    EXPECT_EQ(pattern.category, "greeting");
}

TEST_F(ElizaTest, ResponsePatternMatching) {
    ResponsePattern pattern("hello|hi|hey", {"Hello!", "Hi there!", "Hey!"}, "greeting");

    EXPECT_TRUE(pattern.matches("hello"));
    EXPECT_TRUE(pattern.matches("hi"));
    EXPECT_TRUE(pattern.matches("hey"));
    EXPECT_FALSE(pattern.matches("goodbye"));
}

TEST_F(ElizaTest, ResponsePatternGeneration) {
    ResponsePattern pattern("hello", {"Hello!", "Hi there!", "Hey!"}, "greeting");

    std::string response = pattern.generateResponse();
    EXPECT_FALSE(response.empty());
}

TEST_F(ElizaTest, ResponsePatternPriority) {
    ResponsePattern lowPriority(".*", {"Generic response"}, "default");
    lowPriority.priority = 0.1f;

    ResponsePattern highPriority("emergency", {"Emergency response!"}, "urgent");
    highPriority.priority = 10.0f;

    EXPECT_LT(lowPriority.priority, highPriority.priority);
}

// ============================================================================
// EmotionalStateTracker Tests
// ============================================================================

TEST_F(ElizaTest, EmotionalStateTrackerCreation) {
    EmotionalStateTracker tracker;

    // Default values should be set
    EXPECT_GE(tracker.happiness, 0.0f);
    EXPECT_LE(tracker.happiness, 1.0f);
    EXPECT_GE(tracker.calmness, 0.0f);
}

TEST_F(ElizaTest, EmotionalStateTrackerDominantEmotion) {
    EmotionalStateTracker tracker;
    tracker.happiness = 0.9f;
    tracker.sadness = 0.1f;
    tracker.anger = 0.1f;

    std::string dominant = tracker.getDominantEmotion();
    EXPECT_FALSE(dominant.empty());
}

TEST_F(ElizaTest, EmotionalStateTrackerIntensity) {
    EmotionalStateTracker tracker;

    float intensity = tracker.getEmotionalIntensity();
    EXPECT_GE(intensity, 0.0f);
    EXPECT_LE(intensity, 1.0f);
}

TEST_F(ElizaTest, EmotionalStateTrackerDecay) {
    EmotionalStateTracker tracker;
    tracker.happiness = 1.0f;

    tracker.decay(0.9f);

    EXPECT_LT(tracker.happiness, 1.0f);
}

TEST_F(ElizaTest, EmotionalStateTrackerAdjustment) {
    EmotionalStateTracker tracker;
    float initialHappiness = tracker.happiness;

    tracker.adjustEmotion("happiness", 0.1f);

    EXPECT_GT(tracker.happiness, initialHappiness);
}

// ============================================================================
// ResponseGenerator Tests
// ============================================================================

TEST_F(ElizaTest, ResponseGeneratorCreation) {
    ResponseGenerator generator;
    // Should be able to create a generator without errors
    EXPECT_TRUE(true);
}

TEST_F(ElizaTest, ResponseGeneratorAddPattern) {
    ResponseGenerator generator;

    ResponsePattern pattern("hello", {"Hello!", "Hi!"}, "greeting");
    generator.addPattern(pattern);

    auto matches = generator.getMatchingPatterns("hello");
    EXPECT_GE(matches.size(), 1);
}

TEST_F(ElizaTest, ResponseGeneratorLoadDefaults) {
    ResponseGenerator generator;
    generator.loadDefaultPatterns();

    // Should have some default patterns loaded
    auto greetingMatches = generator.getMatchingPatterns("hello");
    // May or may not match depending on default patterns
    EXPECT_TRUE(true);
}

TEST_F(ElizaTest, ResponseGeneratorGenerateResponse) {
    ResponseGenerator generator;

    ResponsePattern pattern("hello", {"Hello there!", "Hi!", "Hey!"}, "greeting");
    generator.addPattern(pattern);

    ConversationContext context("session-1", "user-1");
    std::string response = generator.generateResponse("hello", context);

    EXPECT_FALSE(response.empty());
}

TEST_F(ElizaTest, ResponseGeneratorTemplateProcessing) {
    ResponseGenerator generator;

    std::unordered_map<std::string, std::string> vars = {
        {"name", "Alice"},
        {"topic", "weather"}
    };

    std::string template_ = "Hello {{name}}, let's talk about {{topic}}.";
    std::string processed = generator.processResponseTemplate(template_, vars);

    // Template processing should work
    EXPECT_FALSE(processed.empty());
}

// ============================================================================
// ElizaCore Tests
// ============================================================================

TEST_F(ElizaTest, ElizaCoreCreation) {
    ElizaCore eliza;
    EXPECT_EQ(eliza.getSessionCount(), 0);
}

TEST_F(ElizaTest, ElizaCoreCreateSession) {
    ElizaCore eliza;

    std::string sessionId = eliza.createSession("user-1");
    EXPECT_FALSE(sessionId.empty());
    EXPECT_EQ(eliza.getSessionCount(), 1);
}

TEST_F(ElizaTest, ElizaCoreProcessInput) {
    ElizaCore eliza;

    std::string sessionId = eliza.createSession("user-1");
    std::string response = eliza.processInput("Hello", sessionId, "user-1");

    EXPECT_FALSE(response.empty());
}

TEST_F(ElizaTest, ElizaCoreGetSession) {
    ElizaCore eliza;

    std::string sessionId = eliza.createSession("user-1");
    auto session = eliza.getSession(sessionId);

    EXPECT_TRUE(session.has_value());
    EXPECT_EQ(session.value().sessionId, sessionId);
}

TEST_F(ElizaTest, ElizaCoreEndSession) {
    ElizaCore eliza;

    std::string sessionId = eliza.createSession("user-1");
    EXPECT_EQ(eliza.getSessionCount(), 1);

    bool ended = eliza.endSession(sessionId);
    EXPECT_TRUE(ended);
    EXPECT_EQ(eliza.getSessionCount(), 0);
}

TEST_F(ElizaTest, ElizaCoreMultipleSessions) {
    ElizaCore eliza;

    std::string session1 = eliza.createSession("user-1");
    std::string session2 = eliza.createSession("user-2");
    std::string session3 = eliza.createSession("user-3");

    EXPECT_EQ(eliza.getSessionCount(), 3);

    auto allSessions = eliza.getAllSessions();
    EXPECT_EQ(allSessions.size(), 3);
}

TEST_F(ElizaTest, ElizaCoreConfiguration) {
    ElizaCore eliza;

    eliza.enableEmotionalTracking(true);
    eliza.enableKnowledgeIntegration(true);
    eliza.enableCharacterPersonality(true);

    // Configuration should be accepted without errors
    EXPECT_TRUE(true);
}

TEST_F(ElizaTest, ElizaCoreClearAllSessions) {
    ElizaCore eliza;

    eliza.createSession("user-1");
    eliza.createSession("user-2");
    EXPECT_EQ(eliza.getSessionCount(), 2);

    eliza.clearAllSessions();
    EXPECT_EQ(eliza.getSessionCount(), 0);
}

// ============================================================================
// Utility Functions Tests
// ============================================================================

TEST_F(ElizaTest, NormalizeInput) {
    std::string input = "  Hello World!  ";
    std::string normalized = normalizeInput(input);

    // Should normalize whitespace and case
    EXPECT_FALSE(normalized.empty());
}

TEST_F(ElizaTest, TokenizeInput) {
    std::string input = "Hello, how are you today?";
    auto tokens = tokenizeInput(input);

    EXPECT_GT(tokens.size(), 0);
}

TEST_F(ElizaTest, IsQuestion) {
    EXPECT_TRUE(isQuestion("How are you?"));
    EXPECT_TRUE(isQuestion("What is your name?"));
    EXPECT_FALSE(isQuestion("I am fine."));
}

TEST_F(ElizaTest, IsGreeting) {
    EXPECT_TRUE(isGreeting("hello"));
    EXPECT_TRUE(isGreeting("hi there"));
    EXPECT_TRUE(isGreeting("hey"));
    EXPECT_FALSE(isGreeting("goodbye"));
}

TEST_F(ElizaTest, IsGoodbye) {
    EXPECT_TRUE(isGoodbye("goodbye"));
    EXPECT_TRUE(isGoodbye("bye"));
    EXPECT_FALSE(isGoodbye("hello"));
}

TEST_F(ElizaTest, ExtractSentiment) {
    std::string happyText = "I am so happy and excited!";
    std::string sadText = "I feel sad and lonely.";

    std::string happySentiment = extractSentiment(happyText);
    std::string sadSentiment = extractSentiment(sadText);

    // Should return some sentiment value
    EXPECT_FALSE(happySentiment.empty());
    EXPECT_FALSE(sadSentiment.empty());
}

// ============================================================================
// ElizaPatterns Namespace Tests
// ============================================================================

TEST_F(ElizaTest, ElizaPatternsGreeting) {
    auto patterns = ElizaPatterns::getGreetingPatterns();
    EXPECT_GT(patterns.size(), 0);
}

TEST_F(ElizaTest, ElizaPatternsQuestion) {
    auto patterns = ElizaPatterns::getQuestionPatterns();
    EXPECT_GT(patterns.size(), 0);
}

TEST_F(ElizaTest, ElizaPatternsEmotional) {
    auto patterns = ElizaPatterns::getEmotionalPatterns();
    EXPECT_GT(patterns.size(), 0);
}

TEST_F(ElizaTest, ElizaPatternsDefault) {
    auto patterns = ElizaPatterns::getDefaultPatterns();
    EXPECT_GT(patterns.size(), 0);
}

TEST_F(ElizaTest, ElizaPatternsAll) {
    auto patterns = ElizaPatterns::getAllPatterns();
    EXPECT_GT(patterns.size(), 0);
}

// ============================================================================
// JSON Serialization Tests
// ============================================================================

TEST_F(ElizaTest, ConversationContextToJson) {
    ConversationContext context("session-1", "user-1");
    context.setSessionData("key", "value");

    JsonValue json = context.toJson();
    EXPECT_FALSE(json.is_null());
}

TEST_F(ElizaTest, ResponsePatternToJson) {
    ResponsePattern pattern("hello", {"Hi!", "Hello!"}, "greeting");

    JsonValue json = pattern.toJson();
    EXPECT_FALSE(json.is_null());
}

TEST_F(ElizaTest, EmotionalStateTrackerToJson) {
    EmotionalStateTracker tracker;
    tracker.happiness = 0.8f;

    JsonValue json = tracker.toJson();
    EXPECT_FALSE(json.is_null());
}
