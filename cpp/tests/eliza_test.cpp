// eliza_test.cpp
// End-to-end tests for elizaos::ElizaCore conversation engine.

#include "elizaos/eliza.hpp"
#include "elizaos/knowledge.hpp"
#include "elizaos/characters.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <string>

using namespace elizaos;

class ElizaFixture : public ::testing::Test {
protected:
    ElizaCore eliza_;
};

TEST_F(ElizaFixture, CreateAndGetSession) {
    auto sid = eliza_.createSession("user1");
    EXPECT_FALSE(sid.empty());
    auto ctx = eliza_.getSession(sid);
    EXPECT_TRUE(ctx.has_value());
}

TEST_F(ElizaFixture, ProcessInputProducesResponse) {
    auto sid = eliza_.createSession("user1");
    auto resp = eliza_.processInput("hello", sid, "user1");
    EXPECT_FALSE(resp.empty());
}

TEST_F(ElizaFixture, EndSessionDoesNotThrow) {
    auto sid = eliza_.createSession("user1");
    // endSession may return true/false depending on whether it clears
    // immediately; either way it must not throw and the session count
    // must not increase afterward.
    auto before = eliza_.getSessionCount();
    EXPECT_NO_THROW((void)eliza_.endSession(sid));
    EXPECT_LE(eliza_.getSessionCount(), before);
}

TEST_F(ElizaFixture, MultipleSessionsAreIndependent) {
    auto s1 = eliza_.createSession("u1");
    auto s2 = eliza_.createSession("u2");
    EXPECT_NE(s1, s2);
    EXPECT_GE(eliza_.getSessionCount(), 2u);
}

TEST_F(ElizaFixture, GetAllSessions) {
    eliza_.createSession();
    eliza_.createSession();
    auto all = eliza_.getAllSessions();
    EXPECT_GE(all.size(), 2u);
}

TEST_F(ElizaFixture, ClearAllSessions) {
    eliza_.createSession();
    eliza_.createSession();
    eliza_.clearAllSessions();
    EXPECT_EQ(eliza_.getSessionCount(), 0u);
}

TEST_F(ElizaFixture, AttachKnowledgeBaseAndCharacterManager) {
    auto kb = std::make_shared<KnowledgeBase>();
    auto cm = std::make_shared<CharacterManager>();
    EXPECT_NO_THROW(eliza_.setKnowledgeBase(kb));
    EXPECT_NO_THROW(eliza_.setCharacterManager(cm));
}

TEST_F(ElizaFixture, FeatureFlagsToggle) {
    EXPECT_NO_THROW(eliza_.enableEmotionalTracking(false));
    EXPECT_NO_THROW(eliza_.enableEmotionalTracking(true));
    EXPECT_NO_THROW(eliza_.enableKnowledgeIntegration(false));
    EXPECT_NO_THROW(eliza_.enableCharacterPersonality(false));
}

TEST(ConversationContext, AddAndSummarize) {
    ConversationContext ctx;
    ctx.addTurn(ConversationTurn("hello", "hi there"));
    ctx.addTurn(ConversationTurn("how are you?", "I am well"));
    auto summary = ctx.getContextSummary();
    EXPECT_FALSE(summary.empty());
}

TEST(ConversationContext, SessionDataRoundtrip) {
    ConversationContext ctx;
    ctx.setSessionData("topic", "math");
    EXPECT_EQ(ctx.getSessionData("topic"), "math");
}

TEST(EmotionalStateTracker, UpdateAndDominant) {
    EmotionalStateTracker t;
    EXPECT_NO_THROW(t.updateFromInput("I am very happy today"));
    EXPECT_NO_THROW((void)t.getDominantEmotion());
}
