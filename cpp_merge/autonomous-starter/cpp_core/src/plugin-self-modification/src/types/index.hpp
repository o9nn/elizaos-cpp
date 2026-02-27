#pragma once
#include <algorithm>
#include <any>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct CharacterModification {
    UUID id;
    UUID agentId;
    double versionNumber;
    std: diffXml;
    std: reasoning;
    std::chrono::system_clock::time_point appliedAt;
    std::optional<std::chrono::system_clock::time_point> rolledBackAt;
    std::chrono::system_clock::time_point createdAt;
};

struct CharacterSnapshot {
    UUID id;
    UUID agentId;
    double versionNumber;
    std: characterData;
    std::chrono::system_clock::time_point createdAt;
};

struct ModificationOperation {
    std: type;
    std: path;
    std::optional<std:> value;
    std::optional<std:> dataType;
};

struct CharacterDiff {
    std::vector<ModificationOperation> operations;
    std: reasoning;
    std: timestamp;
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
