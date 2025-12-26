#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "elizaos/characterfile.hpp"
#include <memory>
#include <string>
#include <fstream>

using namespace elizaos;
using namespace ::testing;

class CharacterFileTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test character data
        testCharacterJson_ = R"({
            "name": "TestBot",
            "bio": "A helpful test bot",
            "personality": "friendly",
            "lore": "Created for testing purposes",
            "traits": ["helpful", "curious", "patient"],
            "topics": ["technology", "science", "art"],
            "messageExamples": [
                {"user": "Hello", "agent": "Hi there!"},
                {"user": "How are you?", "agent": "I'm doing great, thanks!"}
            ]
        })";
    }

    void TearDown() override {
        // Cleanup temp files if any
    }

    std::string testCharacterJson_;
};

// ============================================================================
// CharacterData Tests
// ============================================================================

TEST_F(CharacterFileTest, CharacterDataCreation) {
    CharacterData data;
    data.name = "TestBot";
    data.bio = "A helpful test bot";
    data.personality = "friendly";

    EXPECT_EQ(data.name, "TestBot");
    EXPECT_EQ(data.bio, "A helpful test bot");
    EXPECT_EQ(data.personality, "friendly");
}

TEST_F(CharacterFileTest, CharacterDataTraits) {
    CharacterData data;
    data.traits = {"helpful", "curious", "patient"};

    EXPECT_EQ(data.traits.size(), 3);
    EXPECT_THAT(data.traits, Contains("helpful"));
    EXPECT_THAT(data.traits, Contains("curious"));
    EXPECT_THAT(data.traits, Contains("patient"));
}

TEST_F(CharacterFileTest, CharacterDataTopics) {
    CharacterData data;
    data.topics = {"technology", "science", "art"};

    EXPECT_EQ(data.topics.size(), 3);
    EXPECT_EQ(data.topics[0], "technology");
}

// ============================================================================
// MessageExample Tests
// ============================================================================

TEST_F(CharacterFileTest, MessageExampleCreation) {
    MessageExample example;
    example.userMessage = "Hello!";
    example.agentResponse = "Hi there! How can I help you today?";
    example.context = "greeting";

    EXPECT_EQ(example.userMessage, "Hello!");
    EXPECT_EQ(example.agentResponse, "Hi there! How can I help you today?");
    EXPECT_EQ(example.context, "greeting");
}

TEST_F(CharacterFileTest, MultipleMessageExamples) {
    std::vector<MessageExample> examples;

    MessageExample ex1;
    ex1.userMessage = "What's the weather?";
    ex1.agentResponse = "I can check the weather for you.";
    examples.push_back(ex1);

    MessageExample ex2;
    ex2.userMessage = "Tell me a joke";
    ex2.agentResponse = "Why did the programmer quit? No arrays!";
    examples.push_back(ex2);

    EXPECT_EQ(examples.size(), 2);
}

// ============================================================================
// CharacterFile Parsing Tests
// ============================================================================

TEST_F(CharacterFileTest, ParseValidJson) {
    CharacterFileParser parser;
    auto result = parser.parseFromString(testCharacterJson_);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.data.name, "TestBot");
    EXPECT_EQ(result.data.bio, "A helpful test bot");
    EXPECT_EQ(result.data.personality, "friendly");
}

TEST_F(CharacterFileTest, ParseJsonTraits) {
    CharacterFileParser parser;
    auto result = parser.parseFromString(testCharacterJson_);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.data.traits.size(), 3);
    EXPECT_THAT(result.data.traits, Contains("helpful"));
}

TEST_F(CharacterFileTest, ParseInvalidJson) {
    CharacterFileParser parser;
    auto result = parser.parseFromString("{ invalid json }");

    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.error.empty());
}

TEST_F(CharacterFileTest, ParseEmptyJson) {
    CharacterFileParser parser;
    auto result = parser.parseFromString("{}");

    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.data.name.empty());
}

// ============================================================================
// CharacterFile Serialization Tests
// ============================================================================

TEST_F(CharacterFileTest, SerializeToJson) {
    CharacterData data;
    data.name = "SerializeBot";
    data.bio = "Test serialization";
    data.traits = {"trait1", "trait2"};

    CharacterFileSerializer serializer;
    std::string json = serializer.toJson(data);

    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("SerializeBot"), std::string::npos);
    EXPECT_NE(json.find("trait1"), std::string::npos);
}

TEST_F(CharacterFileTest, RoundTripSerialization) {
    CharacterData original;
    original.name = "RoundTripBot";
    original.bio = "Testing round-trip";
    original.personality = "neutral";
    original.traits = {"a", "b", "c"};

    CharacterFileSerializer serializer;
    std::string json = serializer.toJson(original);

    CharacterFileParser parser;
    auto result = parser.parseFromString(json);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.data.name, original.name);
    EXPECT_EQ(result.data.bio, original.bio);
    EXPECT_EQ(result.data.traits.size(), original.traits.size());
}

// ============================================================================
// CharacterFile Validation Tests
// ============================================================================

TEST_F(CharacterFileTest, ValidateRequiredFields) {
    CharacterData data;
    data.name = "ValidBot";
    data.bio = "Has required fields";

    CharacterValidator validator;
    auto result = validator.validate(data);

    EXPECT_TRUE(result.isValid);
}

TEST_F(CharacterFileTest, ValidateMissingName) {
    CharacterData data;
    data.bio = "Missing name field";

    CharacterValidator validator;
    auto result = validator.validate(data);

    EXPECT_FALSE(result.isValid);
    EXPECT_THAT(result.errors, Contains(HasSubstr("name")));
}

TEST_F(CharacterFileTest, ValidateTraitCount) {
    CharacterData data;
    data.name = "TestBot";

    // Add too many traits
    for (int i = 0; i < 50; ++i) {
        data.traits.push_back("trait" + std::to_string(i));
    }

    CharacterValidator validator;
    auto result = validator.validate(data);

    EXPECT_THAT(result.warnings, Not(IsEmpty()));
}

// ============================================================================
// CharacterFile Template Tests
// ============================================================================

TEST_F(CharacterFileTest, CreateDefaultTemplate) {
    CharacterTemplate tmpl = CharacterTemplate::createDefault();

    EXPECT_FALSE(tmpl.data.name.empty());
    EXPECT_FALSE(tmpl.data.bio.empty());
}

TEST_F(CharacterFileTest, CreateCustomTemplate) {
    CharacterTemplate tmpl = CharacterTemplate::create("CustomBot", "assistant");

    EXPECT_EQ(tmpl.data.name, "CustomBot");
    EXPECT_EQ(tmpl.data.personality, "assistant");
}

// ============================================================================
// CharacterFile Merge Tests
// ============================================================================

TEST_F(CharacterFileTest, MergeCharacterData) {
    CharacterData base;
    base.name = "BaseBot";
    base.traits = {"trait1"};

    CharacterData overlay;
    overlay.bio = "Overlay bio";
    overlay.traits = {"trait2"};

    CharacterMerger merger;
    CharacterData merged = merger.merge(base, overlay);

    EXPECT_EQ(merged.name, "BaseBot");
    EXPECT_EQ(merged.bio, "Overlay bio");
    EXPECT_EQ(merged.traits.size(), 2);
}
