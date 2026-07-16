// =============================================================================
// ElizaCore character-persona wiring + AttentionAllocator spreading-parameter
// E2E regression tests.
//
// These tests germinate two previously-dead centers:
//   1. ElizaCore::processInput fetched a session character and then discarded
//      it ("For now, we'll generate without character-specific response").
//      The repair routes a validated CharacterProfile into
//      ResponseGenerator::generateCharacterResponse, so a session bound to a
//      persona must produce persona-derived responses and an unbound session
//      must keep classic pattern/knowledge behaviour.
//   2. AttentionAllocator::setSpreadingParameters was a dead-store stub that
//      (void)-cast both arguments. The repair propagates bounded values into
//      the live ActivationSpreadingNetwork, observable through the new
//      getSpreadingRate()/getActivationThreshold() accessors and through
//      threshold-gated spreading behaviour.
// =============================================================================

#include <gtest/gtest.h>

#include "elizaos/eliza.hpp"
#include "elizaos/characters.hpp"
#include "elizaos/attention.hpp"

#include <memory>
#include <string>

using namespace elizaos;

namespace {

CharacterProfile makeExpressivePersona(const std::string& name) {
    CharacterProfile profile(name, "An expressive, agreeable test persona");
    profile.personality.extraversion = 0.9f;   // triggers "Oh, " prefix
    profile.personality.agreeableness = 0.8f;  // triggers "I understand what you mean"
    profile.personality.openness = 0.9f;       // triggers "opens up many possibilities"
    profile.communicationStyle.formality = 0.9f; // triggers "I believe that"
    return profile;
}

} // namespace

// -----------------------------------------------------------------------------
// Character persona wiring through ElizaCore::processInput
// -----------------------------------------------------------------------------

class ElizaCharacterWiringTest : public ::testing::Test {
protected:
    void SetUp() override {
        eliza_ = std::make_shared<ElizaCore>();
        characterManager_ = std::make_shared<CharacterManager>();
        eliza_->setCharacterManager(characterManager_);
        eliza_->enableCharacterPersonality(true);
    }

    std::shared_ptr<ElizaCore> eliza_;
    std::shared_ptr<CharacterManager> characterManager_;
};

TEST_F(ElizaCharacterWiringTest, SessionBoundPersonaDrivesResponse) {
    const std::string characterId =
        characterManager_->registerCharacter(makeExpressivePersona("Aria"));
    ASSERT_FALSE(characterId.empty());

    const std::string sessionId = eliza_->createSession("user-1", characterId);
    ASSERT_FALSE(sessionId.empty());

    const std::string response =
        eliza_->processInput("Tell me about self-organizing systems", sessionId, "user-1");
    ASSERT_FALSE(response.empty());

    // The expressive persona's deterministic trait-driven phrasing must appear:
    // high formality -> "I believe that"; high openness -> "possibilities".
    EXPECT_NE(response.find("I believe that"), std::string::npos)
        << "persona formality did not shape the response: " << response;
    EXPECT_NE(response.find("possibilities"), std::string::npos)
        << "persona openness did not shape the response: " << response;
}

TEST_F(ElizaCharacterWiringTest, ProcessInputWithCharacterBindsAndResponds) {
    const std::string characterId =
        characterManager_->registerCharacter(makeExpressivePersona("Beno"));

    const std::string response = eliza_->processInputWithCharacter(
        "What do you think about robotics?", characterId, "", "user-2");
    ASSERT_FALSE(response.empty());
    EXPECT_NE(response.find("I believe that"), std::string::npos)
        << "processInputWithCharacter did not route the persona: " << response;
}

TEST_F(ElizaCharacterWiringTest, UnboundSessionKeepsClassicBehaviour) {
    const std::string sessionId = eliza_->createSession("user-3");
    const std::string response = eliza_->processInput("Hello", sessionId, "user-3");
    ASSERT_FALSE(response.empty());
    // Without a bound character the persona template must NOT leak in.
    EXPECT_EQ(response.find("I believe that this is an interesting point"),
              std::string::npos);
}

TEST_F(ElizaCharacterWiringTest, DisabledPersonalityFlagBypassesPersona) {
    const std::string characterId =
        characterManager_->registerCharacter(makeExpressivePersona("Cass"));
    eliza_->enableCharacterPersonality(false);

    const std::string sessionId = eliza_->createSession("user-4", characterId);
    const std::string response =
        eliza_->processInput("Tell me something", sessionId, "user-4");
    ASSERT_FALSE(response.empty());
    EXPECT_EQ(response.find("I believe that this is an interesting point"),
              std::string::npos)
        << "persona used despite personality disabled: " << response;
}

TEST_F(ElizaCharacterWiringTest, UnknownCharacterIdDegradesGracefully) {
    const std::string sessionId = eliza_->createSession("user-5", "no-such-character");
    const std::string response =
        eliza_->processInput("Hello there", sessionId, "user-5");
    // Must not crash and must still answer.
    ASSERT_FALSE(response.empty());
}

// -----------------------------------------------------------------------------
// AttentionAllocator spreading-parameter propagation
// -----------------------------------------------------------------------------

class AttentionSpreadingParamsTest : public ::testing::Test {
protected:
    void SetUp() override {
        network_ = std::make_unique<ActivationSpreadingNetwork>();
    }

    std::unique_ptr<ActivationSpreadingNetwork> network_;
};

TEST_F(AttentionSpreadingParamsTest, NetworkSettersStoreAndClampValues) {
    network_->setSpreadingParameters(0.5, 0.2);
    EXPECT_DOUBLE_EQ(network_->getSpreadingRate(), 0.5);
    EXPECT_DOUBLE_EQ(network_->getActivationThreshold(), 0.2);

    // Out-of-range values must clamp, never destabilize the network.
    network_->setSpreadingParameters(1.7, -0.4);
    EXPECT_DOUBLE_EQ(network_->getSpreadingRate(), 1.0);
    EXPECT_DOUBLE_EQ(network_->getActivationThreshold(), 0.0);

    network_->setSpreadingParameters(-0.3, 0.05);
    EXPECT_DOUBLE_EQ(network_->getSpreadingRate(), 0.0);
    EXPECT_DOUBLE_EQ(network_->getActivationThreshold(), 0.05);
}

TEST_F(AttentionSpreadingParamsTest, HighThresholdGatesSpreading) {
    network_->addNode("src");
    network_->addNode("dst");
    network_->addEdge("src", "dst", 1.0);
    network_->injectActivation("src", 0.5);

    // Threshold above the source activation: nothing may spread.
    network_->setSpreadingParameters(0.8, 0.9);
    network_->spreadActivation(1);
    EXPECT_DOUBLE_EQ(network_->getActivation("dst"), 0.0);

    // Threshold below the source activation: spreading must occur.
    network_->setSpreadingParameters(0.8, 0.01);
    network_->spreadActivation(1);
    EXPECT_GT(network_->getActivation("dst"), 0.0);
}

TEST_F(AttentionSpreadingParamsTest, AllocatorPropagatesParametersToLiveNetwork) {
    AttentionAllocator allocator(100.0);

    // Wire two elements through the allocator's own spreading network.
    const UUID a = "element-a";
    const UUID b = "element-b";
    AttentionValue value;
    value.importance = 0.9;
    value.urgency = 0.8;
    value.novelty = 0.5;
    value.activation = 0.9;
    allocator.updateAttentionValue(a, value);
    allocator.updateAttentionValue(b, AttentionValue{});
    allocator.addAttentionLink(a, b, 1.0);

    // A prohibitive threshold must gate allocator-driven spreading entirely.
    allocator.setSpreadingParameters(0.8, 5.0);
    allocator.spreadActivation(2);
    const double gated = allocator.getAttentionValue(b).activation;
    EXPECT_DOUBLE_EQ(gated, 0.0);

    // Re-tuning to a permissive threshold on the SAME allocator must take
    // effect immediately (the historical stub only affected future networks).
    allocator.setSpreadingParameters(0.8, 0.001);
    allocator.spreadActivation(2);
    const double spread = allocator.getAttentionValue(b).activation;
    EXPECT_GT(spread, gated);
}
