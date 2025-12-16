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

struct EnvVarConfig {
    std::optional<std::string> value;
    bool required;
    std::string description;
    bool canGenerate;
    std::optional<std::string> validationMethod;
    "missing" | "generating" | "validating" | "invalid" | "valid" status;
    std::optional<std::string> lastError;
    double attempts;
    std::optional<double> createdAt;
    std::optional<double> validatedAt;
    std::string plugin;
};

struct EnvVarMetadata {
};

struct GenerationScript {
    std::string variableName;
    std::string pluginName;
    std::string script;
    std::vector<std::string> dependencies;
    double attempts;
    std::optional<std::string> output;
    std::optional<std::string> error;
    "pending" | "running" | "success" | "failed" status;
    double createdAt;
};

struct GenerationScriptMetadata {
};

struct EnvVarUpdate {
    std::string pluginName;
    std::string variableName;
    std::string value;
};

struct ValidationResult {
    bool isValid;
    std::optional<std::string> error;
    std::optional<std::string> details;
};


} // namespace elizaos
