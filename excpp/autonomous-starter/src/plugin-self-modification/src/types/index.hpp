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
    std::string diffXml;
    std::string reasoning;
    std::chrono::system_clock::time_point appliedAt;
    std::optional<std::chrono::system_clock::time_point> rolledBackAt;
    std::chrono::system_clock::time_point createdAt;
};

struct CharacterSnapshot {
    UUID id;
    UUID agentId;
    double versionNumber;
    std::any characterData;
    std::chrono::system_clock::time_point createdAt;
};

struct ModificationOperation {
    std::string type;
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
