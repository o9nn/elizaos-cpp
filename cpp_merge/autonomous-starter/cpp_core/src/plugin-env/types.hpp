#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

struct EnvVarConfig {
    std::optional<std:> value;
    bool required;
    std: description;
    bool canGenerate;
    std::optional<std:> validationMethod;
    std: status;
    std::optional<std:> lastError;
    double attempts;
    std::optional<double> createdAt;
    std::optional<double> validatedAt;
    std: plugin;
};

struct EnvVarMetadata {
};

struct GenerationScript {
    std: variableName;
    std: pluginName;
    std: script;
    std::vector<std::string> dependencies;
    double attempts;
    std::optional<std:> output;
    std::optional<std:> error;
    std: status;
    double createdAt;
};

struct GenerationScriptMetadata {
};

struct EnvVarUpdate {
    std: pluginName;
    std: variableName;
    std: value;
};

struct ValidationResult {
    bool isValid;
    std::optional<std:> error;
    std::optional<std:> details;
};


} // namespace elizaos
