#pragma once

/**
 * ElizaOS C++ - Characters Module
 *
 * Personality profiles: CharacterTrait, PersonalityMatrix, CharacterProfile,
 * CharacterManager with template support and evolution.
 */

#include "elizaos.hpp"
#include <chrono>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// ============================================================================
// Enumerations
// ============================================================================

enum class TraitCategory {
    PERSONALITY,
    COGNITIVE,
    BEHAVIORAL,
    SOCIAL,
    EMOTIONAL,
    PHYSICAL,
    UNKNOWN
};

enum class TraitValueType {
    NUMERIC,
    BOOLEAN,
    CATEGORICAL,
    TEXT
};

std::string   traitCategoryToString(TraitCategory category);
TraitCategory stringToTraitCategory(const std::string& categoryStr);
std::string   traitValueTypeToString(TraitValueType type);
TraitValueType stringToTraitValueType(const std::string& typeStr);

// ============================================================================
// CharacterTrait
// ============================================================================

class CharacterTrait {
public:
    std::string    name;
    std::string    description;
    TraitCategory  category  = TraitCategory::PERSONALITY;
    TraitValueType valueType = TraitValueType::NUMERIC;
    float          weight    = 1.0f;
    std::chrono::system_clock::time_point lastModified;

    CharacterTrait() = default;
    CharacterTrait(const std::string& name, const std::string& description,
                   TraitCategory category, TraitValueType valueType);

    void setNumericValue(float val);
    void setCategoricalValue(const std::string& val);
    void setBooleanValue(bool val);
    void setTextValue(const std::string& val);

    float       getNumericValue()     const;
    std::string getCategoricalValue() const;
    bool        getBooleanValue()     const;
    std::string getTextValue()        const;

    bool  isCompatibleWith(const CharacterTrait& other) const;
    float calculateSimilarity(const CharacterTrait& other) const;

    JsonValue toJson() const;
    static CharacterTrait fromJson(const JsonValue& json);

private:
    std::variant<float, bool, std::string> value_ = 0.0f;
};

// ============================================================================
// PersonalityMatrix (Big Five OCEAN)
// ============================================================================

struct PersonalityMatrix {
    float openness          = 0.5f;
    float conscientiousness = 0.5f;
    float extraversion      = 0.5f;
    float agreeableness     = 0.5f;
    float neuroticism       = 0.5f;
    float creativity        = 0.5f;

    PersonalityMatrix() = default;
    PersonalityMatrix(float o, float c, float e, float a, float n);

    std::string              getPersonalityType() const;
    std::vector<std::string> getDominantTraits() const;
    float                    calculateCompatibility(const PersonalityMatrix& other) const;
    void                     adjustFromExperience(const std::string& experienceType, float intensity);
    void                     evolveOverTime(float timeFactorDays);

    JsonValue toJson() const;
    static PersonalityMatrix fromJson(const JsonValue& json);
};

// ============================================================================
// CharacterProfile
// ============================================================================

class CharacterProfile {
public:
    std::string id;
    std::string name;
    std::string description;
    std::string archetype;
    PersonalityMatrix personality;
    std::vector<CharacterTrait> traits;
    std::vector<std::string> experiences;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;

    CharacterProfile() = default;
    CharacterProfile(const std::string& name, const std::string& description);

    // Trait management
    void                         addTrait(const CharacterTrait& trait);
    void                         removeTrait(const std::string& traitName);
    void                         updateTrait(const std::string& traitName, const CharacterTrait& newTrait);
    std::optional<CharacterTrait> getTrait(const std::string& traitName) const;
    std::vector<CharacterTrait>  getAllTraits() const;
    std::vector<CharacterTrait>  getTraitsByCategory(TraitCategory category) const;

    // Personality
    void  adjustPersonalityDimension(const std::string& dimension, float adjustment);
    std::string generateResponse(const std::string& input, const std::string& context) const;
    std::string getEmotionalState() const;

    // Evolution
    void learnFromInteraction(const std::string& interaction, const std::string& outcome);
    void evolvePersonality(float timeDelta);
    void addExperience(const std::string& experience);

    // Compatibility
    float                    calculateCompatibility(const CharacterProfile& other) const;
    std::vector<std::string> findCommonTraits(const CharacterProfile& other) const;
    std::string              predictInteractionStyle(const CharacterProfile& other) const;

    // Validation
    bool                     validate() const;
    std::vector<std::string> getValidationErrors() const;

    // Serialization
    JsonValue  toJson() const;
    static CharacterProfile fromJson(const JsonValue& json);
    bool exportToFile(const std::string& filename) const;
    static CharacterProfile importFromFile(const std::string& filename);

private:
    void updateTimestamp();
    std::string generateUniqueId();

    std::unordered_map<std::string, CharacterTrait> traits_;
};

// ============================================================================
// CharacterTemplate
// ============================================================================

struct CharacterTemplate {
    std::string              name;
    std::string              description;
    PersonalityMatrix        basePersonality;
    std::vector<CharacterTrait> defaultTraits;
    std::vector<std::string> requiredTraits;
    std::unordered_map<std::string, PersonalityMatrix> variations;

    CharacterTemplate() = default;
    CharacterTemplate(const std::string& name, const std::string& description);

    CharacterProfile instantiate(const std::string& characterName) const;
    void addVariation(const std::string& variationName, const PersonalityMatrix& personality);

    JsonValue toJson() const;
    static CharacterTemplate fromJson(const JsonValue& json);
};

// ============================================================================
// CharacterManager
// ============================================================================

class CharacterManager {
public:
    CharacterManager();

    // Registration
    std::string registerCharacter(const CharacterProfile& character);
    bool        unregisterCharacter(const std::string& characterId);
    bool        updateCharacter(const std::string& characterId, const CharacterProfile& character);

    // Lookup
    std::optional<CharacterProfile>       getCharacter(const std::string& characterId);
    std::vector<CharacterProfile>         getAllCharacters() const;
    std::vector<CharacterProfile>         searchCharacters(const std::string& query) const;
    std::vector<CharacterProfile>         findCharactersByTrait(const std::string& traitName,
                                                                 float minValue = 0.0f) const;
    std::vector<CharacterProfile>         findCompatibleCharacters(const std::string& characterId,
                                                                    float minCompatibility = 0.5f) const;

    // Templates
    void                              registerTemplate(const CharacterTemplate& template_);
    std::optional<CharacterTemplate>  getTemplate(const std::string& templateName) const;
    std::vector<CharacterTemplate>    getAllTemplates() const;
    CharacterProfile                  createFromTemplate(const std::string& templateName,
                                                          const std::string& characterName);

    // Evolution and bulk operations
    void evolveAllCharacters(float timeDelta);
    void saveAllCharacters(const std::string& directory) const;
    bool loadCharactersFromDirectory(const std::string& directory);

    // Analytics
    std::unordered_map<TraitCategory, int>                  getTraitCategoryStats() const;
    std::string                                             getCharacterAnalytics() const;
    std::vector<std::pair<std::string, std::string>>        findBestMatches() const;

    // Persistence
    bool exportToFile(const std::string& filename) const;
    bool importFromFile(const std::string& filename);

    // Utility
    void   clear();
    size_t getCharacterCount() const;

private:
    std::string generateCharacterId();
    void saveCharacterToMemory(const CharacterProfile& character);
    std::optional<CharacterProfile> loadCharacterFromMemory(const std::string& id);
    std::vector<CharacterProfile>   getAllCharactersFromMemory() const;

    std::unordered_map<std::string, CharacterProfile> characters_;
    std::unordered_map<std::string, CharacterTemplate> templates_;
    mutable std::mutex charactersMutex_;
};

// ============================================================================
// Global instance
// ============================================================================

extern std::shared_ptr<CharacterManager> globalCharacterManager;

} // namespace elizaos
