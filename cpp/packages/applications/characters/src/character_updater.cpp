#include "character_updater.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>

namespace elizaos::characters {
namespace {

float clamp01(float value) {
    return std::max(0.0f, std::min(1.0f, value));
}

std::string anyToString(const std::any& value) {
    if (!value.has_value()) {
        return {};
    }
    if (value.type() == typeid(std::string)) {
        return std::any_cast<std::string>(value);
    }
    if (value.type() == typeid(const char*)) {
        return std::string(std::any_cast<const char*>(value));
    }
    if (value.type() == typeid(char*)) {
        return std::string(std::any_cast<char*>(value));
    }
    if (value.type() == typeid(bool)) {
        return std::any_cast<bool>(value) ? "true" : "false";
    }
    if (value.type() == typeid(int)) {
        return std::to_string(std::any_cast<int>(value));
    }
    if (value.type() == typeid(float)) {
        return std::to_string(std::any_cast<float>(value));
    }
    if (value.type() == typeid(double)) {
        return std::to_string(std::any_cast<double>(value));
    }
    throw std::invalid_argument("unsupported value type for string conversion");
}

float anyToFloat(const std::any& value) {
    if (value.type() == typeid(float)) {
        return std::any_cast<float>(value);
    }
    if (value.type() == typeid(double)) {
        return static_cast<float>(std::any_cast<double>(value));
    }
    if (value.type() == typeid(int)) {
        return static_cast<float>(std::any_cast<int>(value));
    }
    if (value.type() == typeid(std::string)) {
        return std::stof(std::any_cast<std::string>(value));
    }
    throw std::invalid_argument("unsupported value type for numeric conversion");
}

bool isSameTrait(const CharacterTrait& trait, const std::string& name) {
    return trait.name == name;
}

void upsertTrait(CharacterProfile& profile, const CharacterTrait& trait) {
    auto it = std::find_if(profile.traits.begin(), profile.traits.end(), [&](const CharacterTrait& existing) {
        return existing.name == trait.name;
    });
    if (it == profile.traits.end()) {
        profile.traits.push_back(trait);
    } else {
        *it = trait;
    }
}

void addUnique(std::vector<std::string>& values, const std::string& value) {
    if (value.empty()) {
        return;
    }
    if (std::find(values.begin(), values.end(), value) == values.end()) {
        values.push_back(value);
    }
}

bool setPersonalityDimension(PersonalityMatrix& personality, const std::string& dimension, float value) {
    const float clamped = clamp01(value);
    if (dimension == "openness") personality.openness = clamped;
    else if (dimension == "conscientiousness") personality.conscientiousness = clamped;
    else if (dimension == "extraversion") personality.extraversion = clamped;
    else if (dimension == "agreeableness") personality.agreeableness = clamped;
    else if (dimension == "neuroticism") personality.neuroticism = clamped;
    else if (dimension == "creativity") personality.creativity = clamped;
    else if (dimension == "empathy") personality.empathy = clamped;
    else if (dimension == "assertiveness") personality.assertiveness = clamped;
    else if (dimension == "curiosity") personality.curiosity = clamped;
    else if (dimension == "loyalty") personality.loyalty = clamped;
    else return false;
    return true;
}

} // namespace

CharacterOperation CharacterOperation::addValue(std::string path, std::any value) {
    CharacterOperation op;
    op.type = CharacterOperationType::AddValue;
    op.path = std::move(path);
    op.value = std::move(value);
    return op;
}

CharacterOperation CharacterOperation::modifyValue(std::string path, std::any value) {
    CharacterOperation op;
    op.type = CharacterOperationType::ModifyValue;
    op.path = std::move(path);
    op.value = std::move(value);
    return op;
}

CharacterOperation CharacterOperation::deleteValue(std::string path) {
    CharacterOperation op;
    op.type = CharacterOperationType::DeleteValue;
    op.path = std::move(path);
    return op;
}

CharacterOperation CharacterOperation::upsertTrait(CharacterTrait trait) {
    CharacterOperation op;
    op.type = CharacterOperationType::UpsertTrait;
    op.path = trait.name;
    op.trait = std::move(trait);
    return op;
}

CharacterOperation CharacterOperation::removeTrait(std::string traitName) {
    CharacterOperation op;
    op.type = CharacterOperationType::RemoveTrait;
    op.path = std::move(traitName);
    return op;
}

CharacterOperation CharacterOperation::addExperience(std::string experience) {
    CharacterOperation op;
    op.type = CharacterOperationType::AddExperience;
    op.path = "background.experiences";
    op.value = std::move(experience);
    return op;
}

CharacterOperation CharacterOperation::addTag(std::string tag) {
    CharacterOperation op;
    op.type = CharacterOperationType::AddTag;
    op.path = "tags";
    op.value = std::move(tag);
    return op;
}

CharacterOperation CharacterOperation::setMetadata(std::string key, std::string value) {
    CharacterOperation op;
    op.type = CharacterOperationType::SetMetadata;
    op.path = std::move(key);
    op.value = std::move(value);
    return op;
}

void addValue(CharacterProfile& profile, const std::string& path, const std::any& value) {
    if (path == "name") {
        profile.name = sanitizeCharacterName(anyToString(value));
    } else if (path == "description") {
        profile.description = anyToString(value);
    } else if (path == "version") {
        profile.version = anyToString(value);
    } else if (path == "creator") {
        profile.creator = anyToString(value);
    } else if (path == "tags" || path == "tag") {
        addUnique(profile.tags, anyToString(value));
    } else if (path == "background.experiences" || path == "experience") {
        addUnique(profile.background.experiences, anyToString(value));
    } else if (path == "background.goals" || path == "goal") {
        addUnique(profile.background.goals, anyToString(value));
    } else if (path == "communication.catchphrases" || path == "catchphrase") {
        addUnique(profile.communicationStyle.catchphrases, anyToString(value));
    } else if (path.rfind("metadata.", 0) == 0) {
        profile.metadata[path.substr(9)] = anyToString(value);
    } else if (path.rfind("background.context.", 0) == 0) {
        profile.background.additionalContext[path.substr(19)] = anyToString(value);
    } else {
        modifyValue(profile, path, value);
    }
}

void modifyValue(CharacterProfile& profile, const std::string& path, const std::any& value) {
    if (path == "name") {
        profile.name = sanitizeCharacterName(anyToString(value));
    } else if (path == "description") {
        profile.description = anyToString(value);
    } else if (path == "version") {
        profile.version = anyToString(value);
    } else if (path == "creator") {
        profile.creator = anyToString(value);
    } else if (path == "background.backstory") {
        profile.background.backstory = anyToString(value);
    } else if (path == "background.origin") {
        profile.background.origin = anyToString(value);
    } else if (path == "background.occupation") {
        profile.background.occupation = anyToString(value);
    } else if (path == "communication.tone") {
        profile.communicationStyle.tone = anyToString(value);
    } else if (path == "communication.vocabulary") {
        profile.communicationStyle.vocabulary = anyToString(value);
    } else if (path == "communication.verbosity") {
        profile.communicationStyle.verbosity = clamp01(anyToFloat(value));
    } else if (path == "communication.formality") {
        profile.communicationStyle.formality = clamp01(anyToFloat(value));
    } else if (path == "communication.emotionality") {
        profile.communicationStyle.emotionality = clamp01(anyToFloat(value));
    } else if (path.rfind("personality.", 0) == 0) {
        if (!setPersonalityDimension(profile.personality, path.substr(12), anyToFloat(value))) {
            throw std::invalid_argument("unknown personality dimension: " + path);
        }
    } else if (path.rfind("metadata.", 0) == 0) {
        profile.metadata[path.substr(9)] = anyToString(value);
    } else {
        throw std::invalid_argument("unsupported character update path: " + path);
    }
}

void deleteValue(CharacterProfile& profile, const std::string& path) {
    if (path == "description") {
        profile.description.clear();
    } else if (path == "creator") {
        profile.creator.clear();
    } else if (path == "background.backstory") {
        profile.background.backstory.clear();
    } else if (path.rfind("metadata.", 0) == 0) {
        profile.metadata.erase(path.substr(9));
    } else if (path.rfind("trait.", 0) == 0) {
        const std::string traitName = path.substr(6);
        profile.traits.erase(std::remove_if(profile.traits.begin(), profile.traits.end(), [&](const CharacterTrait& trait) {
            return isSameTrait(trait, traitName);
        }), profile.traits.end());
    } else {
        throw std::invalid_argument("unsupported character delete path: " + path);
    }
}

CharacterUpdateResult applyOperationsToCharacter(const CharacterProfile& character,
                                                const std::vector<CharacterOperation>& operations) {
    CharacterUpdateResult result;
    result.profile = character;

    for (const auto& op : operations) {
        try {
            switch (op.type) {
                case CharacterOperationType::AddValue:
                    addValue(result.profile, op.path, op.value);
                    break;
                case CharacterOperationType::ModifyValue:
                    modifyValue(result.profile, op.path, op.value);
                    break;
                case CharacterOperationType::DeleteValue:
                    deleteValue(result.profile, op.path);
                    break;
                case CharacterOperationType::AddTrait:
                case CharacterOperationType::UpsertTrait:
                    if (!op.trait.has_value()) {
                        throw std::invalid_argument("trait operation missing trait value");
                    }
                    upsertTrait(result.profile, *op.trait);
                    break;
                case CharacterOperationType::RemoveTrait:
                    deleteValue(result.profile, "trait." + op.path);
                    break;
                case CharacterOperationType::AddExperience:
                    addValue(result.profile, "experience", op.value);
                    break;
                case CharacterOperationType::AddTag:
                    addValue(result.profile, "tag", op.value);
                    break;
                case CharacterOperationType::SetMetadata:
                    result.profile.metadata[op.path] = anyToString(op.value);
                    break;
            }
            ++result.appliedOperations;
        } catch (const std::exception& ex) {
            result.success = false;
            std::ostringstream warning;
            warning << "operation " << result.appliedOperations << " failed for path '" << op.path << "': " << ex.what();
            result.warnings.push_back(warning.str());
        }
    }

    std::vector<std::string> validationErrors;
    if (!validateCharacterStructure(result.profile, &validationErrors)) {
        result.success = false;
        result.warnings.insert(result.warnings.end(), validationErrors.begin(), validationErrors.end());
    }

    return result;
}

bool validateCharacterStructure(const CharacterProfile& profile, std::vector<std::string>* errors) {
    std::vector<std::string> localErrors;
    if (profile.name.empty()) {
        localErrors.push_back("character name must not be empty");
    }
    if (profile.version.empty()) {
        localErrors.push_back("character version must not be empty");
    }
    if (profile.communicationStyle.verbosity < 0.0f || profile.communicationStyle.verbosity > 1.0f) {
        localErrors.push_back("communication verbosity must be in [0, 1]");
    }
    if (profile.communicationStyle.formality < 0.0f || profile.communicationStyle.formality > 1.0f) {
        localErrors.push_back("communication formality must be in [0, 1]");
    }
    if (profile.communicationStyle.emotionality < 0.0f || profile.communicationStyle.emotionality > 1.0f) {
        localErrors.push_back("communication emotionality must be in [0, 1]");
    }
    for (const auto& trait : profile.traits) {
        if (trait.name.empty()) {
            localErrors.push_back("traits must not have empty names");
        }
        if (trait.weight < 0.0f || trait.weight > 1.0f) {
            localErrors.push_back("trait '" + trait.name + "' weight must be in [0, 1]");
        }
    }

    if (errors != nullptr) {
        errors->insert(errors->end(), localErrors.begin(), localErrors.end());
    }
    return localErrors.empty();
}

} // namespace elizaos::characters
