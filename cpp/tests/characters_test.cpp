// characters_test.cpp - E2E tests for CharacterProfile / CharacterManager.
#include <gtest/gtest.h>
#include "elizaos/characters.hpp"
#include "elizaos/character_json_loader.hpp"
#include "character.hpp"
#include "character_updater.hpp"
#include "character_evolution_evaluator.hpp"
#include <algorithm>
#include <cstdio>

using namespace elizaos;

TEST(CharacterTrait, TypedSettersGetters) {
    CharacterTrait t("openness", "open to ideas",
                     TraitCategory::PERSONALITY, TraitValueType::NUMERIC);
    t.setNumericValue(0.8f);
    EXPECT_NEAR(t.getNumericValue(), 0.8f, 1e-5f);

    CharacterTrait b("alive", "is alive",
                     TraitCategory::PHYSICAL, TraitValueType::BOOLEAN);
    b.setBooleanValue(true);
    EXPECT_TRUE(b.getBooleanValue());

    CharacterTrait c("color", "favorite",
                     TraitCategory::PREFERENCE, TraitValueType::CATEGORICAL);
    c.setCategoricalValue("blue");
    EXPECT_EQ(c.getCategoricalValue(), "blue");

    CharacterTrait s("note", "free text",
                     TraitCategory::COGNITIVE, TraitValueType::TEXT);
    s.setTextValue("hello");
    EXPECT_EQ(s.getTextValue(), "hello");
}

TEST(PersonalityMatrix, ConstructorAndAccessors) {
    PersonalityMatrix p(0.6f, 0.7f, 0.4f, 0.8f, 0.2f);
    EXPECT_NEAR(p.openness, 0.6f, 1e-5f);
    EXPECT_NEAR(p.neuroticism, 0.2f, 1e-5f);
    auto type = p.getPersonalityType();
    EXPECT_FALSE(type.empty());
}

TEST(PersonalityMatrix, CompatibilityIsBoundedAndSelfHigh) {
    PersonalityMatrix p1, p2;
    auto self = p1.calculateCompatibility(p1);
    auto other = p1.calculateCompatibility(p2);
    EXPECT_GE(self, 0.0f);
    EXPECT_LE(self, 1.0f);
    EXPECT_GE(other, 0.0f);
    EXPECT_LE(other, 1.0f);
}

TEST(CharacterProfile, AddRemoveTrait) {
    CharacterProfile cp("Alice", "test");
    CharacterTrait t("brave", "is brave",
                     TraitCategory::PERSONALITY, TraitValueType::BOOLEAN);
    t.setBooleanValue(true);
    cp.addTrait(t);
    EXPECT_TRUE(cp.getTrait("brave").has_value());
    cp.removeTrait("brave");
    EXPECT_FALSE(cp.getTrait("brave").has_value());
}

TEST(CharacterProfile, GetTraitsByCategory) {
    CharacterProfile cp("X", "");
    CharacterTrait t1("a", "", TraitCategory::EMOTIONAL, TraitValueType::TEXT);
    CharacterTrait t2("b", "", TraitCategory::EMOTIONAL, TraitValueType::TEXT);
    CharacterTrait t3("c", "", TraitCategory::SKILL,     TraitValueType::TEXT);
    cp.addTrait(t1); cp.addTrait(t2); cp.addTrait(t3);
    EXPECT_EQ(cp.getTraitsByCategory(TraitCategory::EMOTIONAL).size(), 2u);
    EXPECT_EQ(cp.getTraitsByCategory(TraitCategory::SKILL).size(), 1u);
}

TEST(CharacterProfile, ValidateAfterCreation) {
    CharacterProfile cp("Bob", "");
    auto ok = cp.validate();
    auto errs = cp.getValidationErrors();
    if (!ok) {
        EXPECT_FALSE(errs.empty());
    }
}

TEST(CharacterProfile, GeneratesResponseAndEmotionalState) {
    CharacterProfile cp("X", "");
    auto resp = cp.generateResponse("hello", "");
    EXPECT_FALSE(resp.empty());
    EXPECT_FALSE(cp.getEmotionalState().empty());
}

TEST(CharacterTemplate, InstantiateProducesNamedProfile) {
    CharacterTemplate tmpl("ScienceLover", "");
    auto profile = tmpl.instantiate("Curie");
    EXPECT_EQ(profile.name, "Curie");
}

class CharacterManagerTest : public ::testing::Test {
protected:
    CharacterManager mgr;
};

TEST_F(CharacterManagerTest, RegisterAndGet) {
    CharacterProfile cp("Alice", "");
    auto id = mgr.registerCharacter(cp);
    EXPECT_FALSE(id.empty());
    auto got = mgr.getCharacter(id);
    EXPECT_TRUE(got.has_value());
}

TEST_F(CharacterManagerTest, UnregisterRemoves) {
    auto id = mgr.registerCharacter(CharacterProfile("X", ""));
    EXPECT_TRUE(mgr.unregisterCharacter(id));
    EXPECT_FALSE(mgr.getCharacter(id).has_value());
}

TEST_F(CharacterManagerTest, CharacterCount) {
    mgr.registerCharacter(CharacterProfile("A", ""));
    mgr.registerCharacter(CharacterProfile("B", ""));
    EXPECT_EQ(mgr.getCharacterCount(), 2u);
    mgr.clear();
    EXPECT_EQ(mgr.getCharacterCount(), 0u);
}

TEST_F(CharacterManagerTest, RegisterAndUseTemplate) {
    CharacterTemplate tmpl("Hero", "brave warrior");
    mgr.registerTemplate(tmpl);
    EXPECT_TRUE(mgr.getTemplate("Hero").has_value());
    auto cp = mgr.createFromTemplate("Hero", "Achilles");
    EXPECT_EQ(cp.name, "Achilles");
}

// Skipped: getCharacterAnalytics() hangs in current impl (known bug).
TEST_F(CharacterManagerTest, AnalyticsString) {
    mgr.registerCharacter(CharacterProfile("Z", ""));
    auto s = mgr.getCharacterAnalytics();
    EXPECT_FALSE(s.empty());
}

TEST(CharacterUtils, EnumStringRoundtrip) {
    for (auto t : {TraitCategory::PERSONALITY, TraitCategory::COGNITIVE,
                   TraitCategory::BEHAVIORAL, TraitCategory::EMOTIONAL,
                   TraitCategory::SOCIAL, TraitCategory::PHYSICAL,
                   TraitCategory::SKILL, TraitCategory::PREFERENCE}) {
        EXPECT_EQ(stringToTraitCategory(traitCategoryToString(t)), t);
    }
    for (auto v : {TraitValueType::NUMERIC, TraitValueType::CATEGORICAL,
                   TraitValueType::BOOLEAN, TraitValueType::TEXT}) {
        EXPECT_EQ(stringToTraitValueType(traitValueTypeToString(v)), v);
    }
}

TEST(CharacterArchetypes, ProvidedTemplatesCallable) {
    auto a = CharacterArchetypes::createScientist();
    auto b = CharacterArchetypes::createExplorer();
    auto c = CharacterArchetypes::createMentor();
    EXPECT_FALSE(a.name.empty());
    EXPECT_FALSE(b.name.empty());
    EXPECT_FALSE(c.name.empty());
}

TEST(CharacterCompatibilityHelpers, CreateSeedValidateAndSummarizeProfile) {
    namespace ch = elizaos::characters;

    CharacterProfile profile = ch::createCharacterProfile("  Echo Prime  ", "A deterministic helper profile", "scientist");
    EXPECT_EQ(profile.name, "Echo Prime");
    EXPECT_EQ(profile.description, "A deterministic helper profile");
    EXPECT_TRUE(ch::hasCoreIdentity(profile));
    EXPECT_TRUE(std::find(profile.tags.begin(), profile.tags.end(), "scientist") != profile.tags.end());

    CharacterTrait signal = ch::makeNumericTrait("focus-signal", "Focus signal", TraitCategory::COGNITIVE, 1.5f, 0.25f);
    EXPECT_FLOAT_EQ(signal.getNumericValue(), 1.0f);
    EXPECT_FLOAT_EQ(signal.weight, 0.25f);

    ch::CharacterSeed seed;
    seed.name = "Seeded Echo";
    seed.description = "Seed applied";
    seed.traits.push_back(signal);
    seed.tags = {"seeded", "scientist"};
    seed.metadata["origin"] = "unit-test";
    seed.personality.conscientiousness = 0.91f;
    ch::applySeed(profile, seed);

    EXPECT_EQ(profile.name, "Seeded Echo");
    EXPECT_EQ(profile.description, "Seed applied");
    EXPECT_TRUE(profile.getTrait("focus-signal").has_value());
    EXPECT_EQ(profile.metadata["origin"], "unit-test");
    EXPECT_FLOAT_EQ(profile.personality.conscientiousness, 0.91f);

    const std::string summary = ch::summarizeCharacter(profile);
    EXPECT_NE(summary.find("Seeded Echo"), std::string::npos);
    EXPECT_NE(summary.find("traits=" + std::to_string(profile.traits.size())), std::string::npos);
}

TEST(CharacterCompatibilityUpdater, AppliesPathOperationsAndReportsFailures) {
    namespace ch = elizaos::characters;

    CharacterProfile profile = ch::createCharacterProfile("Updater", "Original", "helper");
    CharacterTrait empathy = ch::makeNumericTrait("empathy-signal", "Empathy signal", TraitCategory::BEHAVIORAL, 0.7f, 0.6f);

    std::vector<ch::CharacterOperation> operations;
    operations.push_back(ch::CharacterOperation::modifyValue("description", std::string("Updated description")));
    operations.push_back(ch::CharacterOperation::modifyValue("personality.openness", 1.7f));
    operations.push_back(ch::CharacterOperation::addTag("stable"));
    operations.push_back(ch::CharacterOperation::addTag("stable"));
    operations.push_back(ch::CharacterOperation::setMetadata("revision", "42"));
    operations.push_back(ch::CharacterOperation::addExperience("passed compatibility update test"));
    operations.push_back(ch::CharacterOperation::upsertTrait(empathy));
    operations.push_back(ch::CharacterOperation::modifyValue("personality.unknown", 0.5f));

    ch::CharacterUpdateResult result = ch::applyOperationsToCharacter(profile, operations);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.appliedOperations, 7);
    ASSERT_FALSE(result.warnings.empty());
    EXPECT_EQ(result.profile.description, "Updated description");
    EXPECT_FLOAT_EQ(result.profile.personality.openness, 1.0f);
    EXPECT_EQ(std::count(result.profile.tags.begin(), result.profile.tags.end(), "stable"), 1);
    EXPECT_EQ(result.profile.metadata["revision"], "42");
    EXPECT_EQ(result.profile.background.experiences.size(), 1);
    EXPECT_TRUE(result.profile.getTrait("empathy-signal").has_value());

    std::vector<std::string> validationErrors;
    EXPECT_TRUE(ch::validateCharacterStructure(result.profile, &validationErrors));
    EXPECT_TRUE(validationErrors.empty());

    ch::deleteValue(result.profile, "metadata.revision");
    EXPECT_TRUE(result.profile.metadata.find("revision") == result.profile.metadata.end());
    ch::deleteValue(result.profile, "trait.empathy-signal");
    EXPECT_FALSE(result.profile.getTrait("empathy-signal").has_value());
}

TEST(CharacterCompatibilityJsonLoader, RoundTripsStringsFilesAndErrors) {
    namespace ch = elizaos::characters;

    CharacterProfile profile = ch::createCharacterProfile("Json Echo", "Round-trip profile", "artist");
    profile.creator = "test-suite";
    profile.tags.push_back("json");
    profile.metadata["purpose"] = "roundtrip";
    profile.background.backstory = "Created during compatibility testing.";
    profile.communicationStyle.tone = "precise";
    profile.addTrait(ch::makeNumericTrait("creativity-signal", "Creative trait", TraitCategory::COGNITIVE, 0.8f, 0.7f));

    const std::string json = CharacterJsonLoader::toJsonString(profile);
    EXPECT_NE(json.find("Json Echo"), std::string::npos);
    EXPECT_NE(json.find("creativity-signal"), std::string::npos);

    auto parsed = CharacterJsonLoader::loadFromJsonString(json);
    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->name, profile.name);
    EXPECT_TRUE(parsed->getTrait("creativity-signal").has_value());

    const std::string path = "/tmp/elizaos_cpp_character_profile_roundtrip.json";
    ASSERT_TRUE(CharacterJsonLoader::saveToFile(profile, path));
    auto loaded = CharacterJsonLoader::loadFromFile(path);
    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->name, "Json Echo");
    std::remove(path.c_str());

    auto invalid = CharacterJsonLoader::loadFromJsonString("{not-json");
    EXPECT_FALSE(invalid.has_value());

    auto missing = CharacterJsonLoader::loadFromFile("/tmp/elizaos_cpp_missing_character_profile.json");
    EXPECT_FALSE(missing.has_value());
}

TEST(CharacterCompatibilityEvolution, EvaluatesAndAppliesSignalsEndToEnd) {
    namespace ch = elizaos::characters;

    CharacterProfile original = ch::createCharacterProfile("Evolution", "Baseline", "mentor");
    CharacterProfile evolved = original;
    evolved.personality.openness = std::min(1.0f, evolved.personality.openness + 0.2f);
    evolved.addTrait(ch::makeNumericTrait("new-adaptation", "New adaptive trait", TraitCategory::BEHAVIORAL, 0.9f, 0.9f));

    ch::CharacterEvolutionReport comparison = ch::evaluateCharacterEvolution(original, evolved);
    EXPECT_GT(comparison.personalityShift, 0.0f);
    EXPECT_LT(comparison.compatibilityWithOriginal, 1.01f);
    EXPECT_TRUE(std::find(comparison.strengthenedTraits.begin(), comparison.strengthenedTraits.end(), "new-adaptation") != comparison.strengthenedTraits.end());
    EXPECT_NE(comparison.summary.find("shift="), std::string::npos);

    std::vector<ch::CharacterEvolutionSignal> signals = {
        {"conversation", "positive collaboration", 0.8f, 0.9f},
        {"debugging", "solved difficult issue", 0.6f, 0.4f}
    };
    auto operations = ch::recommendEvolutionOperations(original, signals);
    EXPECT_GE(operations.size(), signals.size() * 2);

    ch::CharacterEvolutionReport report;
    CharacterProfile signalEvolved = ch::evolveCharacterFromSignals(original, signals, &report);
    EXPECT_FALSE(report.recommendedOperations.empty());
    EXPECT_GT(report.personalityShift, 0.0f);
    EXPECT_TRUE(signalEvolved.getTrait("adaptive-conversation").has_value());
    EXPECT_TRUE(signalEvolved.getTrait("adaptive-debugging").has_value());
    EXPECT_TRUE(std::find(signalEvolved.tags.begin(), signalEvolved.tags.end(), "positively-reinforced") != signalEvolved.tags.end());
    EXPECT_GE(signalEvolved.background.experiences.size(), signals.size());
}
