// eliza_test.cpp - E2E tests for ElizaCore conversational engine.
#include <gtest/gtest.h>
#include "elizaos/eliza.hpp"

using namespace elizaos;

TEST(ElizaUtils, NormalizeInput) {
    EXPECT_EQ(normalizeInput("  Hello WORLD  "), "hello world");
    EXPECT_FALSE(normalizeInput("a B").empty());
}

TEST(ElizaUtils, TokenizeInput) {
    auto t = tokenizeInput("the quick brown fox");
    EXPECT_GE(t.size(), 4u);
}

TEST(ElizaUtils, IsQuestion) {
    EXPECT_TRUE(isQuestion("Why is the sky blue?"));
    EXPECT_FALSE(isQuestion("It is raining."));
}

TEST(ElizaUtils, IsGreeting) {
    // Either case-sensitive or case-insensitive impls are acceptable.
    EXPECT_TRUE(isGreeting("hello") || isGreeting("Hello"));
    EXPECT_TRUE(isGreeting("hi") || isGreeting("Hi there"));
}

TEST(ElizaUtils, IsGoodbye) {
    EXPECT_TRUE(isGoodbye("bye"));
    EXPECT_TRUE(isGoodbye("goodbye"));
}

TEST(ElizaUtils, ExtractSentiment) {
    auto s = extractSentiment("I am very happy today");
    EXPECT_FALSE(s.empty());
}

TEST(ConversationTurn, Construction) {
    ConversationTurn t("hi", "hello there");
    EXPECT_EQ(t.input, "hi");
    EXPECT_EQ(t.response, "hello there");
}

TEST(ConversationContext, AddTurnAndHistory) {
    ConversationContext ctx("s-1", "u-1");
    ctx.addTurn(ConversationTurn("a", "A"));
    ctx.addTurn(ConversationTurn("b", "B"));
    auto h = ctx.getRecentHistory(5);
    EXPECT_EQ(h.size(), 2u);
}

TEST(ConversationContext, SessionDataKV) {
    ConversationContext ctx("s", "u");
    ctx.setSessionData("k", "v");
    EXPECT_EQ(ctx.getSessionData("k"), "v");
}

TEST(ResponsePattern, MatchesAndGenerates) {
    ResponsePattern p("hello.*", {"hi"}, "greeting");
    EXPECT_TRUE(p.matches("hello world"));
    auto r = p.generateResponse({});
    EXPECT_FALSE(r.empty());
}

TEST(EmotionalStateTracker, UpdateAndDominant) {
    EmotionalStateTracker e;
    e.updateFromInput("I am very happy and excited");
    EXPECT_FALSE(e.getDominantEmotion().empty());
    EXPECT_GE(e.getEmotionalIntensity(), 0.0f);
}

TEST(EmotionalStateTracker, AdjustAndDecay) {
    EmotionalStateTracker e;
    e.adjustEmotion("happiness", 0.3f);
    e.decay(0.9f);
    SUCCEED();
}

TEST(ResponseGenerator, AddPatternsAndGenerate) {
    ResponseGenerator gen;
    gen.addPattern(ResponsePattern("hello.*", {"hi back"}, "greeting"));
    ConversationContext ctx("s", "u");
    auto out = gen.generateResponse("hello there", ctx, nullptr);
    EXPECT_FALSE(out.empty());
    auto matches = gen.getMatchingPatterns("hello world");
    EXPECT_GE(matches.size(), 1u);
}

TEST(ElizaCore, SessionLifecycle) {
    ElizaCore c;
    auto sid = c.createSession("user-1");
    EXPECT_FALSE(sid.empty());
    auto sess = c.getSession(sid);
    EXPECT_TRUE(sess.has_value());
    EXPECT_GE(c.getSessionCount(), 1u);
    EXPECT_TRUE(c.endSession(sid));
}

TEST(ElizaCore, ProcessInputProducesResponse) {
    ElizaCore c;
    auto sid = c.createSession();
    auto resp = c.processInput("hello", sid);
    EXPECT_FALSE(resp.empty());
}

TEST(ElizaCore, ConfigToggles) {
    ElizaCore c;
    EXPECT_NO_THROW(c.enableEmotionalTracking(false));
    EXPECT_NO_THROW(c.enableKnowledgeIntegration(false));
    EXPECT_NO_THROW(c.enableCharacterPersonality(false));
}

TEST(ElizaCore, ClearAllSessions) {
    ElizaCore c;
    c.createSession();
    c.createSession();
    c.clearAllSessions();
    EXPECT_EQ(c.getSessionCount(), 0u);
}

TEST(ElizaPatterns, GreetingsAndDefaults) {
    auto g = ElizaPatterns::getGreetingPatterns();
    auto d = ElizaPatterns::getDefaultPatterns();
    auto a = ElizaPatterns::getAllPatterns();
    EXPECT_GE(a.size(), g.size());
    EXPECT_GE(a.size(), d.size());
}
