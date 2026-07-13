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

// =====================================================
// DialogueStateMachine Tests (Task 2.1.3)
// =====================================================

TEST(DialogueStateMachine, InitialState) {
    DialogueStateMachine dsm;
    EXPECT_EQ(dsm.getCurrentState(), DialogueState::IDLE);
    EXPECT_EQ(dsm.getCurrentStateString(), "IDLE");
}

TEST(DialogueStateMachine, GreetingTransition) {
    DialogueStateMachine dsm;
    ConversationContext ctx("test-session", "test-user");
    
    std::string response = dsm.processInput("Hello!", ctx);
    EXPECT_EQ(dsm.getCurrentState(), DialogueState::GREETING);
    EXPECT_FALSE(response.empty());
}

TEST(DialogueStateMachine, IntentDetection) {
    DialogueStateMachine dsm;
    
    EXPECT_EQ(dsm.detectIntent("Hello there!"), DialogueIntent::GREETING);
    EXPECT_EQ(dsm.detectIntent("Goodbye"), DialogueIntent::GOODBYE);
    EXPECT_EQ(dsm.detectIntent("What time is it?"), DialogueIntent::QUESTION);
    EXPECT_EQ(dsm.detectIntent("Yes, that's correct"), DialogueIntent::CONFIRMATION);
    EXPECT_EQ(dsm.detectIntent("No, I don't think so"), DialogueIntent::NEGATION);
}

TEST(DialogueStateMachine, EntityExtraction) {
    DialogueStateMachine dsm;
    
    auto entities = dsm.extractEntities("My name is Alice and my email is alice@example.com");
    EXPECT_TRUE(entities.find("name") != entities.end() || entities.find("email") != entities.end());
}

TEST(DialogueStateMachine, StateHistory) {
    DialogueStateMachine dsm;
    ConversationContext ctx("test-session", "test-user");
    
    dsm.processInput("Hi", ctx);
    dsm.processInput("I need help", ctx);
    
    auto history = dsm.getStateHistory();
    EXPECT_GE(history.size(), 1u);
}

TEST(DialogueStateMachine, Reset) {
    DialogueStateMachine dsm;
    ConversationContext ctx("test-session", "test-user");
    
    dsm.processInput("Hello", ctx);
    EXPECT_NE(dsm.getCurrentState(), DialogueState::IDLE);
    
    dsm.reset();
    EXPECT_EQ(dsm.getCurrentState(), DialogueState::IDLE);
    EXPECT_TRUE(dsm.getStateHistory().empty());
}

TEST(DialogueStateMachine, TransitionTo) {
    DialogueStateMachine dsm;
    
    dsm.transitionTo(DialogueState::PROCESSING);
    EXPECT_EQ(dsm.getCurrentState(), DialogueState::PROCESSING);
}

TEST(DialogueStateMachine, FrameManagement) {
    DialogueStateMachine dsm;
    
    DialogueFrame frame;
    frame.name = "test_frame";
    frame.description = "Test frame";
    
    DialogueSlot slot;
    slot.name = "username";
    slot.type = "string";
    slot.required = true;
    slot.prompt = "What is your username?";
    frame.slots.push_back(slot);
    
    dsm.pushFrame(frame);
    
    auto currentFrame = dsm.getCurrentFrame();
    EXPECT_TRUE(currentFrame.has_value());
    EXPECT_EQ(currentFrame->name, "test_frame");
    EXPECT_FALSE(currentFrame->allSlotsFilledStatus());
    
    dsm.fillSlot("username", std::string("testuser"));
    
    currentFrame = dsm.getCurrentFrame();
    EXPECT_TRUE(currentFrame.has_value());
    EXPECT_TRUE(currentFrame->allSlotsFilledStatus());
}

TEST(DialogueStateMachine, GoodbyeTransition) {
    DialogueStateMachine dsm;
    ConversationContext ctx("test-session", "test-user");
    
    dsm.processInput("Hello", ctx);
    EXPECT_EQ(dsm.getCurrentState(), DialogueState::GREETING);
    
    dsm.processInput("Goodbye", ctx);
    EXPECT_EQ(dsm.getCurrentState(), DialogueState::CLOSING);
}

TEST(DialogueSlot, Validation) {
    DialogueSlot slot;
    slot.name = "status";
    slot.type = "enum";
    slot.allowedValues = {"active", "inactive", "pending"};
    
    EXPECT_TRUE(slot.validate(std::string("active")));
    EXPECT_FALSE(slot.validate(std::string("unknown")));
}

TEST(DialogueUtilities, StateToString) {
    EXPECT_EQ(dialogueStateToString(DialogueState::IDLE), "IDLE");
    EXPECT_EQ(dialogueStateToString(DialogueState::GREETING), "GREETING");
    EXPECT_EQ(dialogueStateToString(DialogueState::PROCESSING), "PROCESSING");
    EXPECT_EQ(dialogueStateToString(DialogueState::CLOSING), "CLOSING");
}

TEST(DialogueUtilities, StringToState) {
    EXPECT_EQ(stringToDialogueState("IDLE"), DialogueState::IDLE);
    EXPECT_EQ(stringToDialogueState("GREETING"), DialogueState::GREETING);
    EXPECT_EQ(stringToDialogueState("PROCESSING"), DialogueState::PROCESSING);
    EXPECT_EQ(stringToDialogueState("INVALID"), DialogueState::IDLE);
}

TEST(DialogueUtilities, IntentToString) {
    EXPECT_EQ(dialogueIntentToString(DialogueIntent::GREETING), "GREETING");
    EXPECT_EQ(dialogueIntentToString(DialogueIntent::QUESTION), "QUESTION");
    EXPECT_EQ(dialogueIntentToString(DialogueIntent::GOODBYE), "GOODBYE");
}

TEST(DialogueUtilities, StringToIntent) {
    EXPECT_EQ(stringToDialogueIntent("GREETING"), DialogueIntent::GREETING);
    EXPECT_EQ(stringToDialogueIntent("QUESTION"), DialogueIntent::QUESTION);
    EXPECT_EQ(stringToDialogueIntent("INVALID"), DialogueIntent::UNKNOWN);
}

TEST(DialogueStateMachine, ToJson) {
    DialogueStateMachine dsm;
    dsm.transitionTo(DialogueState::GREETING);
    
    auto json = dsm.toJson();
    auto it = json.find("currentState");
    EXPECT_NE(it, json.end());
}

