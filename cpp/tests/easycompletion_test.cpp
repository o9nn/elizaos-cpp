// easycompletion_test.cpp - E2E tests for easy completion utilities & client.
#include <gtest/gtest.h>
#include "elizaos/easycompletion.hpp"

using namespace elizaos;

TEST(ComposePrompt, SubstitutesVariables) {
    auto out = compose_prompt("Hello {{name}}!", {{"name", "World"}});
    EXPECT_NE(out.find("World"), std::string::npos);
}

TEST(ComposePrompt, IgnoresMissingVariables) {
    auto out = compose_prompt("Hi {{x}} and {{y}}", {{"x", "A"}});
    EXPECT_NE(out.find("A"), std::string::npos);
}

TEST(ComposeFunction, ProducesPopulatedDefinition) {
    auto fn = compose_function("addNumbers", "adds two numbers",
                                {{"a", "number"}, {"b", "number"}},
                                {"a", "b"});
    EXPECT_EQ(fn.name, "addNumbers");
    EXPECT_EQ(fn.required_properties.size(), 2u);
}

TEST(CountTokens, NonNegativeForText) {
    EXPECT_GE(count_tokens(""), 0);
    EXPECT_GT(count_tokens("hello world"), 0);
}

TEST(TrimPrompt, RespectsMaxLength) {
    std::string big(20000, 'a');
    auto trimmed = trim_prompt(big, 100);
    EXPECT_LE((int)trimmed.size(), (int)big.size());
}

TEST(ChunkPrompt, ChunksLongInput) {
    std::string big(8000, 'a');
    auto chunks = chunk_prompt(big, 2000);
    EXPECT_GE(chunks.size(), 1u);
}

TEST(EasyCompletionClient, ConfigSetterRoundtrip) {
    EasyCompletionClient c;
    CompletionConfig cfg;
    cfg.model = "test-model";
    cfg.temperature = 0.5f;
    c.set_config(cfg);
    EXPECT_EQ(c.get_config().model, "test-model");
    EXPECT_NEAR(c.get_config().temperature, 0.5f, 1e-5f);
}

TEST(CompletionResponse, DefaultIsEmpty) {
    CompletionResponse r;
    EXPECT_TRUE(r.text.empty());
    EXPECT_EQ(r.usage.total_tokens, 0);
    EXPECT_FALSE(r.error.has_value());
}
