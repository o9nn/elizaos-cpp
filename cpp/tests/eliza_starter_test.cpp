// eliza_starter_test.cpp - Tests for ElizaStarterAgent factory and the
// character-system wiring (setCharacter / addPersonalityTrait /
// getCharacterProfile) completed during the 2026-07-14 KSM cycle.
#include <gtest/gtest.h>
#include "elizaos/eliza_starter.hpp"

using namespace elizaos;

TEST(ElizaStarterAgent, FactoryProducesAgent) {
    auto a = createElizaStarterAgent("Eliza", "id-1");
    EXPECT_NE(a, nullptr);
}

TEST(ElizaStarterAgent, MultipleAgentsHaveDistinctIdentity) {
    auto a = createElizaStarterAgent("A", "1");
    auto b = createElizaStarterAgent("B", "2");
    EXPECT_NE(a.get(), b.get());
}

TEST(ElizaStarterAgent, NoCharacterProfileBeforeConfiguration) {
    auto a = createElizaStarterAgent("Fresh", "fresh-1");
    EXPECT_FALSE(a->getCharacterProfile().has_value());
}

TEST(ElizaStarterAgent, SetCharacterRegistersRealProfile) {
    auto a = createElizaStarterAgent("Eliza", "id-2");
    a->setCharacter("Dr. Helper", "A knowledgeable assistant",
                    "Created to help users with patience");

    auto profile = a->getCharacterProfile();
    ASSERT_TRUE(profile.has_value());
    EXPECT_EQ(profile->name, "Dr. Helper");
    EXPECT_EQ(profile->description, "A knowledgeable assistant");
    EXPECT_EQ(profile->background.backstory, "Created to help users with patience");
    EXPECT_FALSE(profile->id.empty());
}

TEST(ElizaStarterAgent, SetCharacterTwiceUpdatesInsteadOfDuplicating) {
    auto a = createElizaStarterAgent("Eliza", "id-3");
    a->setCharacter("First Name", "first bio", "first lore");
    auto first = a->getCharacterProfile();
    ASSERT_TRUE(first.has_value());
    const std::string firstId = first->id;

    a->setCharacter("Second Name", "second bio", "second lore");
    auto second = a->getCharacterProfile();
    ASSERT_TRUE(second.has_value());
    EXPECT_EQ(second->id, firstId) << "re-configuration must update in place";
    EXPECT_EQ(second->name, "Second Name");
    EXPECT_EQ(second->description, "second bio");
    EXPECT_EQ(second->background.backstory, "second lore");
}

TEST(ElizaStarterAgent, AddPersonalityTraitAttachesNumericTrait) {
    auto a = createElizaStarterAgent("Eliza", "id-4");
    a->setCharacter("Trait Holder", "bio", "lore");
    a->addPersonalityTrait("curiosity", "How curious the agent is", 0.8);

    auto profile = a->getCharacterProfile();
    ASSERT_TRUE(profile.has_value());
    ASSERT_EQ(profile->traits.size(), 1u);
    EXPECT_EQ(profile->traits[0].name, "curiosity");
    EXPECT_EQ(profile->traits[0].description, "How curious the agent is");
    EXPECT_EQ(profile->traits[0].category, TraitCategory::PERSONALITY);
    EXPECT_NEAR(profile->traits[0].getNumericValue(), 0.8f, 1e-6);
}

TEST(ElizaStarterAgent, AddPersonalityTraitIsIdempotentPerName) {
    auto a = createElizaStarterAgent("Eliza", "id-5");
    a->setCharacter("Trait Holder", "bio", "lore");
    a->addPersonalityTrait("patience", "initial", 0.5);
    a->addPersonalityTrait("patience", "revised", 0.9);

    auto profile = a->getCharacterProfile();
    ASSERT_TRUE(profile.has_value());
    ASSERT_EQ(profile->traits.size(), 1u) << "same-name trait must replace, not duplicate";
    EXPECT_EQ(profile->traits[0].description, "revised");
    EXPECT_NEAR(profile->traits[0].getNumericValue(), 0.9f, 1e-6);
}

TEST(ElizaStarterAgent, TraitStrengthIsClampedToUnitInterval) {
    auto a = createElizaStarterAgent("Eliza", "id-6");
    a->setCharacter("Clamped", "bio", "lore");
    a->addPersonalityTrait("overdrive", "beyond max", 3.7);
    a->addPersonalityTrait("underflow", "below min", -2.0);

    auto profile = a->getCharacterProfile();
    ASSERT_TRUE(profile.has_value());
    ASSERT_EQ(profile->traits.size(), 2u);
    for (const auto& t : profile->traits) {
        const float v = t.getNumericValue();
        EXPECT_GE(v, 0.0f);
        EXPECT_LE(v, 1.0f);
    }
}

TEST(ElizaStarterAgent, AddTraitWithoutCharacterAutoCreatesProfile) {
    auto a = createElizaStarterAgent("AutoProfile", "id-7");
    a->addPersonalityTrait("bootstrapped", "trait before character", 0.6);

    auto profile = a->getCharacterProfile();
    ASSERT_TRUE(profile.has_value()) << "trait insertion must bootstrap a profile";
    ASSERT_EQ(profile->traits.size(), 1u);
    EXPECT_EQ(profile->traits[0].name, "bootstrapped");
}

TEST(ElizaStarterAgent, InitializeSeedsDefaultCharacterAndTraits) {
    auto a = createElizaStarterAgent("Seeded", "id-8");
    ASSERT_TRUE(a->initialize());

    auto profile = a->getCharacterProfile();
    ASSERT_TRUE(profile.has_value());
    EXPECT_EQ(profile->name, "Eliza Starter");
    EXPECT_EQ(profile->traits.size(), 4u);

    bool sawHelpfulness = false;
    for (const auto& t : profile->traits) {
        if (t.name == "helpfulness") {
            sawHelpfulness = true;
            EXPECT_NEAR(t.getNumericValue(), 0.95f, 1e-6);
        }
    }
    EXPECT_TRUE(sawHelpfulness);
}

TEST(ElizaStarterAgent, ProcessMessageStillWorksAfterCharacterWiring) {
    auto a = createElizaStarterAgent("Conversational", "id-9");
    ASSERT_TRUE(a->initialize());
    const std::string response = a->processMessage("Hello there!", "user-1");
    EXPECT_FALSE(response.empty());
}
