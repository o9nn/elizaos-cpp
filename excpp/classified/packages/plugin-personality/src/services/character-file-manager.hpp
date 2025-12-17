#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".types.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Validation schema for character modifications

using CharacterModification = z::infer<typeof CharacterModificationSchema>;

/**
 * Service for safely managing character file modifications
 * Handles backup, validation, and atomic updates of character files
 */
class CharacterFileManager {
public:
    CharacterFileManager(IAgentRuntime runtime);
    static std::future<CharacterFileManager> start(IAgentRuntime runtime);
    std::future<void> initialize();
    std::future<void> detectCharacterFile();
    void setupValidationRules();
    std::variant<Promise<string, null>> createBackup();
    std::future<void> cleanupOldBackups();
     validateModification(const std::any& modification);
    Promise< applyModification(CharacterModification modification);
    std::future<std::vector<std::any>> getModificationHistory(auto limit);
    Promise<Array< getAvailableBackups();
    Promise< restoreFromBackup(const std::string& backupPath);
    Promise< restoreFromHistory(double entryIndex);
    std::future<void> stop();

private:
    IAgentRuntime runtime_;
    std::string backupDir_;
};


} // namespace elizaos
