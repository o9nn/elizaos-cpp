#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"
#include ".utils/character-updater.hpp"
#include ".utils/validation.hpp"
#include ".utils/xml-parser.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class CharacterModificationService {
public:
    std::future<void> initialize();
    std::future<void> stop();
    Promise< applyCharacterDiff(const std::string& diffXml, std::optional<ModificationOptions> options);
    std::future<bool> rollbackCharacter(const std::string& versionId);
    std::vector<CharacterModification> getCharacterHistory();
    std::vector<CharacterSnapshot> getCharacterSnapshots();
    double getCurrentVersion();
    void lockModifications();
    void unlockModifications();
    std::future<void> loadModificationHistory();
    std::future<void> saveState();
    void scheduleSaveState();
    std::future<CharacterSnapshot> createSnapshot(const std::string& reason);
    std::future<CharacterModification> storeModification(CharacterDiff diff, const std::string& originalXml);
    double getNextVersion();
    std::vector<CharacterModification> getRecentModifications(double hoursBack);
    CharacterDiff filterDiffByFocusAreas(CharacterDiff diff, const std::vector<std::string>& focusAreas);
    std::future<void> updateRuntimeCharacter(Character updatedCharacter);
    std::future<void> persistCharacterUpdate(Character character);
};


} // namespace elizaos
