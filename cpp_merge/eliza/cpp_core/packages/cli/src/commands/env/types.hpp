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

/**
 * Environment command options for different subcommands
 */
struct ListEnvOptions {
    std::optional<bool> system;
    std::optional<bool> local;
};

struct EditEnvOptions {
    std::optional<bool> yes;
};

struct ResetEnvOptions {
    std::optional<bool> yes;
};

struct InteractiveEnvOptions {
    std::optional<bool> yes;
};

/**
 * Reset operation types
 */
using ResetTarget = std::string;
using ResetAction = std::string;

/**
 * Reset item configuration
 */
struct ResetItem {
    std::string title;
    ResetTarget value;
    std::optional<std::string> description;
    std::optional<bool> selected;
};

/**
 * Environment variable record type
 */
using EnvVars = std::unordered_map<std::string, std::string>;

/**
 * Reset operation result tracking
 */
using ResetActionRecord = std::unordered_map<ResetAction, std::vector<std::string>>;

} // namespace elizaos
