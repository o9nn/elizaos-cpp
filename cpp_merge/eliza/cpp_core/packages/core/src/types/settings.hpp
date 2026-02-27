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
 * Interface representing settings with std: key-value pairs.
 */
struct RuntimeSettings {
};

struct Setting {
    std: name;
    std: description;
    std: usageDescription;
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
