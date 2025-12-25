#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "elizaos/eliza.hpp"
#include <memory>
#include <string>
#include <vector>

using namespace elizaos;
using namespace ::testing;

class ElizaTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_.agentId = "test-eliza-agent";
        config_.agentName = "TestEliza";
        config_.bio = "A test Eliza agent";
        config_.lore = "Created for testing purposes";
        config_.personality = "helpful";
    }

    AgentConfig config_;
};

// ============================================================================
// ElizaConfig Tests
// ============================================================================

TEST_F(ElizaTest, DefaultElizaConfigValues) {
    ElizaConfig elizaConfig;

    EXPECT_TRUE(elizaConfig.enableMemory);
    EXPECT_TRUE(elizaConfig.enableLearning);
    EXPECT_EQ(elizaConfig.maxConversationLength, 100);
    EXPECT_EQ(elizaConfig.responseTimeout, 30);
}

TEST_F(ElizaTest, CustomElizaConfigValues) {
    ElizaConfig elizaConfig;
    elizaConfig.enableMemory = false;
    elizaConfig.enableLearning = false;
    elizaConfig.maxConversationLength = 50;
    elizaConfig.responseTimeout = 60;

    EXPECT_FALSE(elizaConfig.enableMemory);
    EXPECT_FALSE(elizaConfig.enableLearning);
    EXPECT_EQ(elizaConfig.maxConversationLength, 50);
    EXPECT_EQ(elizaConfig.responseTimeout, 60);
}

// ============================================================================
// Pattern Matching Tests
// ============================================================================

TEST_F(ElizaTest, PatternCreation) {
    ElizaPattern pattern;
    pattern.regex = "hello|hi|hey";
    pattern.responses = {"Hello!", "Hi there!", "Hey!"};
    pattern.priority = 10;

    EXPECT_EQ(pattern.regex, "hello|hi|hey");
    EXPECT_EQ(pattern.responses.size(), 3);
    EXPECT_EQ(pattern.priority, 10);
}

TEST_F(ElizaTest, PatternWithCapture) {
    ElizaPattern pattern;
    pattern.regex = "my name is (.+)";
    pattern.responses = {"Nice to meet you, $1!"};
    pattern.hasCapture = true;

    EXPECT_TRUE(pattern.hasCapture);
    EXPECT_EQ(pattern.responses[0], "Nice to meet you, $1!");
}

// ============================================================================
// Conversation Context Tests
// ============================================================================

TEST_F(ElizaTest, ConversationContextCreation) {
    ConversationContext context;
    context.sessionId = "session-123";
    context.userId = "user-456";
    context.startTime = std::chrono::system_clock::now();

    EXPECT_EQ(context.sessionId, "session-123");
    EXPECT_EQ(context.userId, "user-456");
    EXPECT_TRUE(context.isActive);
}

TEST_F(ElizaTest, ConversationContextMessageHistory) {
    ConversationContext context;

    context.addMessage("user", "Hello");
    context.addMessage("agent", "Hi there!");
    context.addMessage("user", "How are you?");

    EXPECT_EQ(context.messageCount(), 3);

    auto lastMessage = context.getLastMessage();
    EXPECT_EQ(lastMessage.content, "How are you?");
    EXPECT_EQ(lastMessage.role, "user");
}

TEST_F(ElizaTest, ConversationContextVariables) {
    ConversationContext context;

    context.setVariable("name", "Alice");
    context.setVariable("topic", "weather");

    EXPECT_EQ(context.getVariable("name"), "Alice");
    EXPECT_EQ(context.getVariable("topic"), "weather");
    EXPECT_EQ(context.getVariable("missing"), ""); // Default empty string
}

// ============================================================================
// Response Generation Tests
// ============================================================================

TEST_F(ElizaTest, ResponseCreation) {
    ElizaResponse response;
    response.text = "Hello! How can I help you today?";
    response.confidence = 0.95;
    response.matchedPattern = "greeting";
    response.timestamp = std::chrono::system_clock::now();

    EXPECT_EQ(response.text, "Hello! How can I help you today?");
    EXPECT_NEAR(response.confidence, 0.95, 0.01);
    EXPECT_EQ(response.matchedPattern, "greeting");
}

TEST_F(ElizaTest, ResponseWithMetadata) {
    ElizaResponse response;
    response.text = "I understand you're feeling sad.";
    response.metadata["emotion"] = "empathy";
    response.metadata["topic"] = "feelings";

    EXPECT_EQ(response.metadata.size(), 2);
    EXPECT_EQ(response.metadata["emotion"], "empathy");
}

// ============================================================================
// Rule Set Tests
// ============================================================================

TEST_F(ElizaTest, RuleSetCreation) {
    ElizaRuleSet ruleSet;
    ruleSet.name = "default";
    ruleSet.description = "Default conversation rules";

    ElizaPattern pattern1;
    pattern1.regex = "hello";
    pattern1.responses = {"Hello!", "Hi!"};
    ruleSet.patterns.push_back(pattern1);

    ElizaPattern pattern2;
    pattern2.regex = "goodbye|bye";
    pattern2.responses = {"Goodbye!", "See you later!"};
    ruleSet.patterns.push_back(pattern2);

    EXPECT_EQ(ruleSet.name, "default");
    EXPECT_EQ(ruleSet.patterns.size(), 2);
}

TEST_F(ElizaTest, RuleSetPriorityOrdering) {
    ElizaRuleSet ruleSet;

    ElizaPattern lowPriority;
    lowPriority.regex = ".*";
    lowPriority.priority = 1;

    ElizaPattern highPriority;
    highPriority.regex = "emergency";
    highPriority.priority = 100;

    ruleSet.addPattern(lowPriority);
    ruleSet.addPattern(highPriority);
    ruleSet.sortByPriority();

    EXPECT_EQ(ruleSet.patterns[0].priority, 100);
    EXPECT_EQ(ruleSet.patterns[1].priority, 1);
}

// ============================================================================
// Keyword Extraction Tests
// ============================================================================

TEST_F(ElizaTest, KeywordExtraction) {
    std::string input = "I am feeling very sad today because of work";
    std::vector<std::string> keywords = ElizaKeywords::extract(input);

    EXPECT_THAT(keywords, Contains("feeling"));
    EXPECT_THAT(keywords, Contains("sad"));
    EXPECT_THAT(keywords, Contains("work"));
}

TEST_F(ElizaTest, StopWordRemoval) {
    std::string input = "I am the one who is going to the store";
    std::vector<std::string> keywords = ElizaKeywords::extractWithoutStopWords(input);

    EXPECT_THAT(keywords, Not(Contains("am")));
    EXPECT_THAT(keywords, Not(Contains("the")));
    EXPECT_THAT(keywords, Not(Contains("is")));
    EXPECT_THAT(keywords, Contains("store"));
}

// ============================================================================
// Sentiment Analysis Tests
// ============================================================================

TEST_F(ElizaTest, PositiveSentiment) {
    std::string text = "I am so happy and excited today!";
    double sentiment = ElizaSentiment::analyze(text);

    EXPECT_GT(sentiment, 0.0);
}

TEST_F(ElizaTest, NegativeSentiment) {
    std::string text = "I feel terrible and sad about everything";
    double sentiment = ElizaSentiment::analyze(text);

    EXPECT_LT(sentiment, 0.0);
}

TEST_F(ElizaTest, NeutralSentiment) {
    std::string text = "The weather is cloudy today";
    double sentiment = ElizaSentiment::analyze(text);

    EXPECT_NEAR(sentiment, 0.0, 0.3);
}

// ============================================================================
// Text Transformation Tests
// ============================================================================

TEST_F(ElizaTest, PronounTransformation) {
    std::string input = "I think you are wrong about me";
    std::string transformed = ElizaTransform::transformPronouns(input);

    EXPECT_EQ(transformed, "you think I am wrong about you");
}

TEST_F(ElizaTest, ReflectionTransformation) {
    EXPECT_EQ(ElizaTransform::reflect("I"), "you");
    EXPECT_EQ(ElizaTransform::reflect("me"), "you");
    EXPECT_EQ(ElizaTransform::reflect("you"), "I");
    EXPECT_EQ(ElizaTransform::reflect("your"), "my");
    EXPECT_EQ(ElizaTransform::reflect("my"), "your");
}
