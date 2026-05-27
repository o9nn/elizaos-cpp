#include <gtest/gtest.h>
#define private public
#include "elizaos/character_json_loader.hpp"
#undef private

using namespace elizaos;

TEST(CharacterJsonLoaderHelpers, ParsesStringArraysFromSupportedAnyShapes) {
    JsonValue json;
    json["vector"] = std::vector<std::string>{" first ", "second", ""};
    json["json_literal"] = std::string("[\"alpha\", {\"text\": \"beta\"}, 42]");
    json["delimited"] = std::string("red, green;not-split, blue");
    json["any_vector"] = std::vector<std::any>{std::string("one"), 2, true};

    const auto vectorValues = CharacterJsonLoader::getStringArrayFromJson(json, "vector");
    ASSERT_EQ(vectorValues.size(), 2u);
    EXPECT_EQ(vectorValues[0], "first");
    EXPECT_EQ(vectorValues[1], "second");

    const auto literalValues = CharacterJsonLoader::getStringArrayFromJson(json, "json_literal");
    ASSERT_EQ(literalValues.size(), 3u);
    EXPECT_EQ(literalValues[0], "alpha");
    EXPECT_EQ(literalValues[1], "beta");
    EXPECT_EQ(literalValues[2], "42");

    const auto delimitedValues = CharacterJsonLoader::getStringArrayFromJson(json, "delimited");
    ASSERT_EQ(delimitedValues.size(), 3u);
    EXPECT_EQ(delimitedValues[0], "red");
    EXPECT_EQ(delimitedValues[1], "green;not-split");
    EXPECT_EQ(delimitedValues[2], "blue");

    const auto anyValues = CharacterJsonLoader::getStringArrayFromJson(json, "any_vector");
    ASSERT_EQ(anyValues.size(), 3u);
    EXPECT_EQ(anyValues[0], "one");
    EXPECT_EQ(anyValues[1], "2");
    EXPECT_EQ(anyValues[2], "true");
}

TEST(CharacterJsonLoaderHelpers, ParsesTypedScalarsAndNestedProfileHelpers) {
    JsonValue style;
    style["tone"] = std::string(" precise ");
    style["verbosity"] = 0.75;
    style["formality"] = std::string("0.80");
    style["emotionality"] = 1;
    style["catchphrases"] = std::string("[\"measure twice\", \"cut once\"]");
    style["speakingPatterns"] = std::vector<std::any>{std::string("short clauses"), std::string("clear commitments")};

    const CommunicationStyle parsedStyle = CharacterJsonLoader::parseCommunicationStyleFromJson(style);
    EXPECT_EQ(parsedStyle.tone, " precise ");
    EXPECT_FLOAT_EQ(parsedStyle.verbosity, 0.75f);
    EXPECT_FLOAT_EQ(parsedStyle.formality, 0.80f);
    EXPECT_FLOAT_EQ(parsedStyle.emotionality, 1.0f);
    ASSERT_EQ(parsedStyle.catchphrases.size(), 2u);
    EXPECT_EQ(parsedStyle.catchphrases[0], "measure twice");
    ASSERT_EQ(parsedStyle.speakingPatterns.size(), 2u);
    EXPECT_EQ(parsedStyle.speakingPatterns[1], "clear commitments");

    JsonValue background;
    background["relationships"] = std::vector<std::string>{"mentor", " collaborator "};
    background["goals"] = std::string("stability, traceability");
    background["fears"] = std::vector<std::any>{std::string("regression"), std::string("ambiguity")};
    const CharacterBackground parsedBackground = CharacterJsonLoader::parseBackgroundFromJson(background);
    ASSERT_EQ(parsedBackground.relationships.size(), 2u);
    EXPECT_EQ(parsedBackground.relationships[1], "collaborator");
    ASSERT_EQ(parsedBackground.goals.size(), 2u);
    EXPECT_EQ(parsedBackground.goals[0], "stability");
    ASSERT_EQ(parsedBackground.fears.size(), 2u);
    EXPECT_EQ(parsedBackground.fears[1], "ambiguity");
}
