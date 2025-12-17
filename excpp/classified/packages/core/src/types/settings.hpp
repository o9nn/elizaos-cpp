#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Interface representing settings with string key-value pairs.
 */
struct RuntimeSettings {
};

struct Setting {
    std::string name;
    std::string description;
    std::string usageDescription;
    std::optional<bool> value;
    bool required;
    std::optional<bool> public;
    std::optional<bool> secret;
    std::optional<std::vector<std::string>> dependsOn;
};

struct WorldSettings {
};

struct OnboardingConfig {
};


} // namespace elizaos
