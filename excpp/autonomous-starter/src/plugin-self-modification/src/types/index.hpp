#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;

struct CharacterModification {
    UUID id;
    UUID agentId;
    double versionNumber;
    std::string diffXml;
    std::string reasoning;
    Date appliedAt;
    std::optional<Date> rolledBackAt;
    Date createdAt;
};


struct CharacterSnapshot {
    UUID id;
    UUID agentId;
    double versionNumber;
    any; // Will be the full Character object characterData;
    Date createdAt;
};


struct ModificationOperation {
    "add" | "modify" | "delete" type;
    std::string path;
    std::optional<std::any> value;
    std::optional<std::string> dataType;
};


struct CharacterDiff {
    std::vector<ModificationOperation> operations;
    std::string reasoning;
    std::string timestamp;
};


struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};


struct ModificationOptions {
    std::optional<std::vector<std::string>> focusAreas;
    std::optional<double> maxChanges;
    std::optional<bool> preserveCore;
};


} // namespace elizaos
