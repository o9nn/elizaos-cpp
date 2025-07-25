#include <gtest/gtest.h>
#include "elizaos/characters.hpp"
#include <thread>
#include <chrono>

using namespace elizaos;

class CharactersTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset character manager before each test
        globalCharacterManager->clear();
    }
    
    void TearDown() override {
        globalCharacterManager->clear();
    }
};

// =====================================================
// CharacterTrait Tests
// =====================================================

TEST_F(CharactersTest, CharacterTrait_Creation) {
    CharacterTrait trait("creativity", "Measures creative thinking ability", 
                        TraitCategory::COGNITIVE, TraitValueType::NUMERIC);
    
    EXPECT_EQ(trait.name, "creativity");
    EXPECT_EQ(trait.description, "Measures creative thinking ability");
    EXPECT_EQ(trait.category, TraitCategory::COGNITIVE);
    EXPECT_EQ(trait.valueType, TraitValueType::NUMERIC);
    EXPECT_FLOAT_EQ(trait.weight, 1.0f);
}

TEST_F(CharactersTest, CharacterTrait_NumericValues) {
    CharacterTrait trait("intelligence", "Measures cognitive ability", 
                        TraitCategory::COGNITIVE, TraitValueType::NUMERIC);
    
    trait.setNumericValue(0.8f);
    EXPECT_FLOAT_EQ(trait.getNumericValue(), 0.8f);
    
    // Test clamping
    trait.setNumericValue(1.5f);
    EXPECT_FLOAT_EQ(trait.getNumericValue(), 1.0f);
    
    trait.setNumericValue(-0.5f);
    EXPECT_FLOAT_EQ(trait.getNumericValue(), 0.0f);
}

TEST_F(CharactersTest, CharacterTrait_BooleanValues) {
    CharacterTrait trait("extroverted", "Is person extroverted", 
                        TraitCategory::PERSONALITY, TraitValueType::BOOLEAN);
    
    trait.setBooleanValue(true);
    EXPECT_TRUE(trait.getBooleanValue());
    
    trait.setBooleanValue(false);
    EXPECT_FALSE(trait.getBooleanValue());
}

TEST_F(CharactersTest, CharacterTrait_CategoricalValues) {
    CharacterTrait trait("occupation", "Person's occupation", 
                        TraitCategory::SOCIAL, TraitValueType::CATEGORICAL);
    
    trait.setCategoricalValue("scientist");
    EXPECT_EQ(trait.getCategoricalValue(), "scientist");
    
    trait.setCategoricalValue("artist");
    EXPECT_EQ(trait.getCategoricalValue(), "artist");
}

TEST_F(CharactersTest, CharacterTrait_Compatibility) {
    CharacterTrait trait1("creativity", "Creative ability", 
                         TraitCategory::COGNITIVE, TraitValueType::NUMERIC);
    CharacterTrait trait2("creativity", "Creative thinking", 
                         TraitCategory::COGNITIVE, TraitValueType::NUMERIC);
    CharacterTrait trait3("logic", "Logical thinking", 
                         TraitCategory::COGNITIVE, TraitValueType::BOOLEAN);
    
    EXPECT_TRUE(trait1.isCompatibleWith(trait2));
    EXPECT_FALSE(trait1.isCompatibleWith(trait3));
}

TEST_F(CharactersTest, CharacterTrait_Similarity) {
    CharacterTrait trait1("creativity", "Creative ability", 
                         TraitCategory::COGNITIVE, TraitValueType::NUMERIC);
    CharacterTrait trait2("creativity", "Creative thinking", 
                         TraitCategory::COGNITIVE, TraitValueType::NUMERIC);
    
    trait1.setNumericValue(0.8f);
    trait2.setNumericValue(0.8f);
    EXPECT_FLOAT_EQ(trait1.calculateSimilarity(trait2), 1.0f);
    
    trait2.setNumericValue(0.6f);
    EXPECT_FLOAT_EQ(trait1.calculateSimilarity(trait2), 0.8f);
}

TEST_F(CharactersTest, CharacterTrait_JsonSerialization) {
    CharacterTrait original("creativity", "Creative ability", 
                           TraitCategory::COGNITIVE, TraitValueType::NUMERIC);
    original.setNumericValue(0.7f);
    original.weight = 0.8f;
    
    JsonValue json = original.toJson();
    CharacterTrait restored = CharacterTrait::fromJson(json);
    
    EXPECT_EQ(restored.name, original.name);
    EXPECT_EQ(restored.description, original.description);
    EXPECT_EQ(restored.category, original.category);
    EXPECT_EQ(restored.valueType, original.valueType);
    EXPECT_FLOAT_EQ(restored.getNumericValue(), original.getNumericValue());
    EXPECT_FLOAT_EQ(restored.weight, original.weight);
}

// =====================================================
// PersonalityMatrix Tests
// =====================================================

TEST_F(CharactersTest, PersonalityMatrix_Creation) {
    PersonalityMatrix personality(0.8f, 0.6f, 0.7f, 0.9f, 0.3f);
    
    EXPECT_FLOAT_EQ(personality.openness, 0.8f);
    EXPECT_FLOAT_EQ(personality.conscientiousness, 0.6f);
    EXPECT_FLOAT_EQ(personality.extraversion, 0.7f);
    EXPECT_FLOAT_EQ(personality.agreeableness, 0.9f);
    EXPECT_FLOAT_EQ(personality.neuroticism, 0.3f);
}

TEST_F(CharactersTest, PersonalityMatrix_PersonalityType) {
    PersonalityMatrix extroverted(0.5f, 0.5f, 0.8f, 0.5f, 0.2f);
    std::string type = extroverted.getPersonalityType();
    
    EXPECT_EQ(type[0], 'E'); // Extroverted
    EXPECT_EQ(type.length(), 4); // Should be 4 characters
}

TEST_F(CharactersTest, PersonalityMatrix_DominantTraits) {
    PersonalityMatrix creative(0.9f, 0.4f, 0.5f, 0.6f, 0.2f);
    creative.creativity = 0.95f;
    
    auto traits = creative.getDominantTraits();
    EXPECT_FALSE(traits.empty());
    EXPECT_TRUE(std::find(traits.begin(), traits.end(), "creativity") != traits.end());
}

TEST_F(CharactersTest, PersonalityMatrix_Compatibility) {
    PersonalityMatrix person1(0.8f, 0.6f, 0.7f, 0.8f, 0.3f);
    PersonalityMatrix person2(0.8f, 0.6f, 0.7f, 0.8f, 0.3f);
    PersonalityMatrix person3(0.2f, 0.1f, 0.1f, 0.2f, 0.9f);
    
    float compatibility1 = person1.calculateCompatibility(person2);
    float compatibility2 = person1.calculateCompatibility(person3);
    
    EXPECT_GT(compatibility1, compatibility2);
    EXPECT_NEAR(compatibility1, 1.0f, 0.1f); // Very compatible
}

TEST_F(CharactersTest, PersonalityMatrix_ExperienceAdjustment) {
    PersonalityMatrix personality;
    float originalExtraversion = personality.extraversion;
    
    personality.adjustFromExperience("social_success", 0.5f);
    EXPECT_GT(personality.extraversion, originalExtraversion);
}

TEST_F(CharactersTest, PersonalityMatrix_Evolution) {
    PersonalityMatrix personality(0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
    PersonalityMatrix original = personality;
    
    personality.evolveOverTime(100.0f); // 100 days
    
    // Should have some change but not dramatic
    float totalChange = std::abs(personality.openness - original.openness) +
                       std::abs(personality.conscientiousness - original.conscientiousness) +
                       std::abs(personality.extraversion - original.extraversion) +
                       std::abs(personality.agreeableness - original.agreeableness) +
                       std::abs(personality.neuroticism - original.neuroticism);
    
    EXPECT_GT(totalChange, 0.0f);
    EXPECT_LT(totalChange, 1.0f); // Shouldn't change dramatically
}

// =====================================================
// CharacterProfile Tests
// =====================================================

TEST_F(CharactersTest, CharacterProfile_Creation) {
    CharacterProfile character("Alice", "A helpful AI assistant");
    
    EXPECT_EQ(character.name, "Alice");
    EXPECT_EQ(character.description, "A helpful AI assistant");
    EXPECT_FALSE(character.id.empty());
    EXPECT_EQ(character.version, "1.0");
}

TEST_F(CharactersTest, CharacterProfile_TraitManagement) {
    CharacterProfile character("Bob", "Test character");
    
    CharacterTrait creativity("creativity", "Creative ability", 
                             TraitCategory::COGNITIVE, TraitValueType::NUMERIC);
    creativity.setNumericValue(0.8f);
    
    character.addTrait(creativity);
    EXPECT_EQ(character.traits.size(), 1);
    
    auto retrievedTrait = character.getTrait("creativity");
    ASSERT_TRUE(retrievedTrait.has_value());
    EXPECT_FLOAT_EQ(retrievedTrait->getNumericValue(), 0.8f);
    
    character.removeTrait("creativity");
    EXPECT_EQ(character.traits.size(), 0);
}

TEST_F(CharactersTest, CharacterProfile_PersonalityAdjustment) {
    CharacterProfile character("Charlie", "Test character");
    float originalOpenness = character.personality.openness;
    
    character.adjustPersonalityDimension("openness", 0.1f);
    EXPECT_FLOAT_EQ(character.personality.openness, originalOpenness + 0.1f);
}

TEST_F(CharactersTest, CharacterProfile_ResponseGeneration) {
    CharacterProfile character("David", "Test character");
    character.personality.extraversion = 0.9f;
    character.personality.agreeableness = 0.8f;
    character.communicationStyle.formality = 0.3f;
    
    std::string response = character.generateResponse("Hello", "greeting");
    EXPECT_FALSE(response.empty());
    EXPECT_TRUE(response.find("I think") != std::string::npos || 
                response.find("I believe") != std::string::npos);
}

TEST_F(CharactersTest, CharacterProfile_EmotionalState) {
    CharacterProfile character("Eve", "Test character");
    character.personality.agreeableness = 0.9f;
    character.personality.extraversion = 0.9f;
    character.personality.neuroticism = 0.1f;
    
    std::string emotion = character.getEmotionalState();
    EXPECT_EQ(emotion, "excited");
}

TEST_F(CharactersTest, CharacterProfile_Learning) {
    CharacterProfile character("Frank", "Test character");
    float originalExtraversion = character.personality.extraversion;
    
    character.learnFromInteraction("Had a great conversation", "positive");
    EXPECT_GT(character.personality.extraversion, originalExtraversion);
    EXPECT_EQ(character.background.experiences.size(), 1);
}

TEST_F(CharactersTest, CharacterProfile_Compatibility) {
    CharacterProfile character1("Alice", "Friendly person");
    CharacterProfile character2("Bob", "Also friendly");
    CharacterProfile character3("Charlie", "Not so friendly");
    
    character1.personality.agreeableness = 0.9f;
    character1.personality.extraversion = 0.8f;
    
    character2.personality.agreeableness = 0.9f;
    character2.personality.extraversion = 0.8f;
    
    character3.personality.agreeableness = 0.1f;
    character3.personality.extraversion = 0.2f;
    character3.personality.neuroticism = 0.9f;
    
    float compatibility1 = character1.calculateCompatibility(character2);
    float compatibility2 = character1.calculateCompatibility(character3);
    
    EXPECT_GT(compatibility1, compatibility2);
}

TEST_F(CharactersTest, CharacterProfile_Validation) {
    CharacterProfile validCharacter("Alice", "Valid character");
    EXPECT_TRUE(validCharacter.validate());
    EXPECT_TRUE(validCharacter.getValidationErrors().empty());
    
    CharacterProfile invalidCharacter;
    invalidCharacter.name = "";
    invalidCharacter.id = "";
    EXPECT_FALSE(invalidCharacter.validate());
    EXPECT_FALSE(invalidCharacter.getValidationErrors().empty());
}

// =====================================================
// CharacterTemplate Tests
// =====================================================

TEST_F(CharactersTest, CharacterTemplate_Creation) {
    CharacterTemplate template_("Scientist", "Research-oriented character");
    template_.basePersonality.openness = 0.9f;
    template_.basePersonality.curiosity = 0.95f;
    
    EXPECT_EQ(template_.name, "Scientist");
    EXPECT_EQ(template_.description, "Research-oriented character");
    EXPECT_FLOAT_EQ(template_.basePersonality.openness, 0.9f);
}

TEST_F(CharactersTest, CharacterTemplate_Instantiation) {
    CharacterTemplate scientistTemplate("Scientist", "Research-oriented character");
    scientistTemplate.basePersonality.openness = 0.9f;
    scientistTemplate.basePersonality.curiosity = 0.95f;
    
    CharacterTrait analyticalTrait("analytical", "Analytical thinking", 
                                  TraitCategory::COGNITIVE, TraitValueType::NUMERIC);
    analyticalTrait.setNumericValue(0.9f);
    scientistTemplate.defaultTraits.push_back(analyticalTrait);
    
    CharacterProfile scientist = scientistTemplate.instantiate("Dr. Smith");
    
    EXPECT_EQ(scientist.name, "Dr. Smith");
    EXPECT_FLOAT_EQ(scientist.personality.openness, 0.9f);
    EXPECT_EQ(scientist.traits.size(), 1);
    EXPECT_EQ(scientist.traits[0].name, "analytical");
}

// =====================================================
// CharacterManager Tests
// =====================================================

TEST_F(CharactersTest, CharacterManager_Registration) {
    CharacterProfile character("Alice", "Test character");
    
    std::string id = globalCharacterManager->registerCharacter(character);
    EXPECT_FALSE(id.empty());
    EXPECT_EQ(globalCharacterManager->getCharacterCount(), 1);
    
    auto retrieved = globalCharacterManager->getCharacter(id);
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->name, "Alice");
}

TEST_F(CharactersTest, CharacterManager_Unregistration) {
    CharacterProfile character("Bob", "Test character");
    std::string id = globalCharacterManager->registerCharacter(character);
    
    EXPECT_TRUE(globalCharacterManager->unregisterCharacter(id));
    EXPECT_EQ(globalCharacterManager->getCharacterCount(), 0);
    
    auto retrieved = globalCharacterManager->getCharacter(id);
    EXPECT_FALSE(retrieved.has_value());
}

TEST_F(CharactersTest, CharacterManager_Update) {
    CharacterProfile character("Charlie", "Original description");
    std::string id = globalCharacterManager->registerCharacter(character);
    
    CharacterProfile updatedChar("Charlie", "Updated description");
    EXPECT_TRUE(globalCharacterManager->updateCharacter(id, updatedChar));
    
    auto retrieved = globalCharacterManager->getCharacter(id);
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->description, "Updated description");
}

TEST_F(CharactersTest, CharacterManager_Search) {
    CharacterProfile character1("Alice", "A helpful assistant");
    CharacterProfile character2("Bob", "A creative artist");
    CharacterProfile character3("Charlie", "A helpful teacher");
    
    globalCharacterManager->registerCharacter(character1);
    globalCharacterManager->registerCharacter(character2);
    globalCharacterManager->registerCharacter(character3);
    
    auto results = globalCharacterManager->searchCharacters("helpful");
    EXPECT_EQ(results.size(), 2); // Alice and Charlie
    
    auto artistResults = globalCharacterManager->searchCharacters("artist");
    EXPECT_EQ(artistResults.size(), 1); // Bob
}

TEST_F(CharactersTest, CharacterManager_Templates) {
    CharacterTemplate scientistTemplate("Scientist", "Research-oriented");
    scientistTemplate.basePersonality.openness = 0.9f;
    
    globalCharacterManager->registerTemplate(scientistTemplate);
    
    auto retrieved = globalCharacterManager->getTemplate("Scientist");
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->name, "Scientist");
    
    auto allTemplates = globalCharacterManager->getAllTemplates();
    EXPECT_EQ(allTemplates.size(), 1);
}

TEST_F(CharactersTest, CharacterManager_CreateFromTemplate) {
    CharacterTemplate artistTemplate("Artist", "Creative individual");
    artistTemplate.basePersonality.creativity = 0.95f;
    
    globalCharacterManager->registerTemplate(artistTemplate);
    
    CharacterProfile artist = globalCharacterManager->createFromTemplate("Artist", "Leonardo");
    
    EXPECT_EQ(artist.name, "Leonardo");
    EXPECT_FLOAT_EQ(artist.personality.creativity, 0.95f);
}

TEST_F(CharactersTest, CharacterManager_CompatibleCharacters) {
    CharacterProfile character1("Alice", "Friendly person");
    CharacterProfile character2("Bob", "Also friendly");
    CharacterProfile character3("Charlie", "Not so friendly");
    
    character1.personality.agreeableness = 0.9f;
    character1.personality.extraversion = 0.8f;
    
    character2.personality.agreeableness = 0.9f;
    character2.personality.extraversion = 0.8f;
    
    character3.personality.agreeableness = 0.1f;
    character3.personality.extraversion = 0.2f;
    
    std::string id1 = globalCharacterManager->registerCharacter(character1);
    globalCharacterManager->registerCharacter(character2);
    globalCharacterManager->registerCharacter(character3);
    
    auto compatible = globalCharacterManager->findCompatibleCharacters(id1, 0.7f);
    EXPECT_EQ(compatible.size(), 1); // Only Bob should be compatible
    EXPECT_EQ(compatible[0].name, "Bob");
}

TEST_F(CharactersTest, CharacterManager_Evolution) {
    CharacterProfile character("David", "Test character");
    std::string id = globalCharacterManager->registerCharacter(character);
    
    auto original = globalCharacterManager->getCharacter(id);
    ASSERT_TRUE(original.has_value());
    float originalOpenness = original->personality.openness;
    
    globalCharacterManager->evolveAllCharacters(50.0f); // 50 days
    
    auto evolved = globalCharacterManager->getCharacter(id);
    ASSERT_TRUE(evolved.has_value());
    
    // Should have some change
    EXPECT_NE(evolved->personality.openness, originalOpenness);
}

TEST_F(CharactersTest, CharacterManager_Analytics) {
    CharacterProfile character1("Alice", "Test");
    CharacterProfile character2("Bob", "Test");
    
    CharacterTrait trait1("creativity", "Creative", TraitCategory::COGNITIVE, TraitValueType::NUMERIC);
    CharacterTrait trait2("empathy", "Empathetic", TraitCategory::EMOTIONAL, TraitValueType::NUMERIC);
    
    character1.addTrait(trait1);
    character2.addTrait(trait2);
    
    globalCharacterManager->registerCharacter(character1);
    globalCharacterManager->registerCharacter(character2);
    
    std::string analytics = globalCharacterManager->getCharacterAnalytics();
    EXPECT_FALSE(analytics.empty());
    EXPECT_TRUE(analytics.find("Total characters: 2") != std::string::npos);
    
    auto stats = globalCharacterManager->getTraitCategoryStats();
    EXPECT_GT(stats[TraitCategory::COGNITIVE], 0);
    EXPECT_GT(stats[TraitCategory::EMOTIONAL], 0);
}

// =====================================================
// Utility Function Tests
// =====================================================

TEST_F(CharactersTest, UtilityFunctions_TraitCategoryConversion) {
    EXPECT_EQ(traitCategoryToString(TraitCategory::PERSONALITY), "personality");
    EXPECT_EQ(traitCategoryToString(TraitCategory::COGNITIVE), "cognitive");
    EXPECT_EQ(traitCategoryToString(TraitCategory::BEHAVIORAL), "behavioral");
    
    EXPECT_EQ(stringToTraitCategory("cognitive"), TraitCategory::COGNITIVE);
    EXPECT_EQ(stringToTraitCategory("behavioral"), TraitCategory::BEHAVIORAL);
    EXPECT_EQ(stringToTraitCategory("unknown"), TraitCategory::PERSONALITY); // default
}

TEST_F(CharactersTest, UtilityFunctions_TraitValueTypeConversion) {
    EXPECT_EQ(traitValueTypeToString(TraitValueType::NUMERIC), "numeric");
    EXPECT_EQ(traitValueTypeToString(TraitValueType::BOOLEAN), "boolean");
    EXPECT_EQ(traitValueTypeToString(TraitValueType::CATEGORICAL), "categorical");
    
    EXPECT_EQ(stringToTraitValueType("boolean"), TraitValueType::BOOLEAN);
    EXPECT_EQ(stringToTraitValueType("categorical"), TraitValueType::CATEGORICAL);
    EXPECT_EQ(stringToTraitValueType("unknown"), TraitValueType::NUMERIC); // default
}

// =====================================================
// Character Archetypes Tests
// =====================================================

TEST_F(CharactersTest, CharacterArchetypes_Scientist) {
    CharacterTemplate scientist = CharacterArchetypes::createScientist();
    
    EXPECT_EQ(scientist.name, "Scientist");
    EXPECT_GT(scientist.basePersonality.openness, 0.8f);
    EXPECT_GT(scientist.basePersonality.curiosity, 0.9f);
    EXPECT_FALSE(scientist.defaultTraits.empty());
}

TEST_F(CharactersTest, CharacterArchetypes_Artist) {
    CharacterTemplate artist = CharacterArchetypes::createArtist();
    
    EXPECT_EQ(artist.name, "Artist");
    EXPECT_GT(artist.basePersonality.openness, 0.9f);
    EXPECT_GT(artist.basePersonality.creativity, 0.8f);
}

TEST_F(CharactersTest, CharacterArchetypes_Leader) {
    CharacterTemplate leader = CharacterArchetypes::createLeader();
    
    EXPECT_EQ(leader.name, "Leader");
    EXPECT_GT(leader.basePersonality.extraversion, 0.7f);
    EXPECT_GT(leader.basePersonality.assertiveness, 0.8f);
}

TEST_F(CharactersTest, CharacterArchetypes_Helper) {
    CharacterTemplate helper = CharacterArchetypes::createHelper();
    
    EXPECT_EQ(helper.name, "Helper");
    EXPECT_GT(helper.basePersonality.agreeableness, 0.8f);
    EXPECT_GT(helper.basePersonality.empathy, 0.8f);
}

// =====================================================
// Thread Safety Tests
// =====================================================

TEST_F(CharactersTest, CharacterManager_ThreadSafety) {
    const int numThreads = 10;
    const int charactersPerThread = 5;
    std::vector<std::thread> threads;
    std::vector<std::string> characterIds;
    std::mutex idMutex;
    
    // Create characters from multiple threads
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < charactersPerThread; ++i) {
                CharacterProfile character("Character_" + std::to_string(t) + "_" + std::to_string(i), 
                                         "Test character");
                std::string id = globalCharacterManager->registerCharacter(character);
                
                std::lock_guard<std::mutex> lock(idMutex);
                characterIds.push_back(id);
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify all characters were created
    EXPECT_EQ(characterIds.size(), numThreads * charactersPerThread);
    EXPECT_EQ(globalCharacterManager->getCharacterCount(), numThreads * charactersPerThread);
    
    // Verify all characters can be retrieved
    for (const auto& id : characterIds) {
        auto character = globalCharacterManager->getCharacter(id);
        EXPECT_TRUE(character.has_value());
    }
}

// =====================================================
// Performance Tests (Basic)
// =====================================================

TEST_F(CharactersTest, CharacterManager_Performance_LargeScale) {
    const int numCharacters = 100;
    std::vector<std::string> ids;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create many characters
    for (int i = 0; i < numCharacters; ++i) {
        CharacterProfile character("Character_" + std::to_string(i), "Test character " + std::to_string(i));
        
        // Add some traits
        CharacterTrait trait1("trait1", "Test trait", TraitCategory::PERSONALITY, TraitValueType::NUMERIC);
        trait1.setNumericValue(static_cast<float>(i) / numCharacters);
        character.addTrait(trait1);
        
        std::string id = globalCharacterManager->registerCharacter(character);
        ids.push_back(id);
    }
    
    auto createEnd = std::chrono::high_resolution_clock::now();
    
    // Search operations
    auto searchResults = globalCharacterManager->searchCharacters("Character");
    EXPECT_EQ(searchResults.size(), numCharacters);
    
    // Compatibility search
    if (!ids.empty()) {
        auto compatibleResults = globalCharacterManager->findCompatibleCharacters(ids[0], 0.5f);
        // Should find some compatible characters
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    
    auto createDuration = std::chrono::duration_cast<std::chrono::milliseconds>(createEnd - start);
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Performance should be reasonable (adjust thresholds as needed)
    EXPECT_LT(createDuration.count(), 1000); // Less than 1 second to create 100 characters
    EXPECT_LT(totalDuration.count(), 2000);  // Less than 2 seconds total
    
    EXPECT_EQ(globalCharacterManager->getCharacterCount(), numCharacters);
}