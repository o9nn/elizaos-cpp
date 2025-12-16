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

/**
 * Interface representing settings with string key-value pairs.
 */
struct RuntimeSettings {
};


struct Setting {
    std::string name;
    string; // Used in chat context when discussing the setting description;
    string; // Used during settings to guide users usageDescription;
    string | boolean | null value;
    bool required;
    std::optional<boolean; // If true, shown in public channels> public;
    std::optional<boolean; // If true, value is masked and only shown during settings> secret;
    std::optional<(value: any) => boolean> validation;
    std::optional<std::vector<std::string>> dependsOn;
    std::optional<(value: any) => string> onSetAction;
    std::optional<(settings: { [key: string]: Setting }) => boolean> visibleIf;
};


struct WorldSettings {
};


struct OnboardingConfig {
    { settings;
};


} // namespace elizaos
