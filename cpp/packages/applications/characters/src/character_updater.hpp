#pragma once

#include "character.hpp"

#include <any>
#include <optional>
#include <string>
#include <vector>

namespace elizaos::characters {

enum class CharacterOperationType {
    AddValue,
    ModifyValue,
    DeleteValue,
    AddTrait,
    UpsertTrait,
    RemoveTrait,
    AddExperience,
    AddTag,
    SetMetadata
};

struct CharacterOperation {
    CharacterOperationType type{CharacterOperationType::ModifyValue};
    std::string path;
    std::any value;
    std::optional<CharacterTrait> trait;

    static CharacterOperation addValue(std::string path, std::any value);
    static CharacterOperation modifyValue(std::string path, std::any value);
    static CharacterOperation deleteValue(std::string path);
    static CharacterOperation upsertTrait(CharacterTrait trait);
    static CharacterOperation removeTrait(std::string traitName);
    static CharacterOperation addExperience(std::string experience);
    static CharacterOperation addTag(std::string tag);
    static CharacterOperation setMetadata(std::string key, std::string value);
};

struct CharacterUpdateResult {
    CharacterProfile profile;
    bool success{true};
    int appliedOperations{0};
    std::vector<std::string> warnings;
};

CharacterUpdateResult applyOperationsToCharacter(const CharacterProfile& character,
                                                const std::vector<CharacterOperation>& operations);

bool validateCharacterStructure(const CharacterProfile& profile,
                                std::vector<std::string>* errors = nullptr);

void addValue(CharacterProfile& profile, const std::string& path, const std::any& value);
void modifyValue(CharacterProfile& profile, const std::string& path, const std::any& value);
void deleteValue(CharacterProfile& profile, const std::string& path);

} // namespace elizaos::characters
