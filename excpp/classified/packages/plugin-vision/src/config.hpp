#pragma once
#include <algorithm>
#include <any>
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



// Default configuration for backward compatibility

// Configuration schema using Zod for validation
  // Camera configuration

  // Vision processing

  // Object detection

  // Pose detection

  // Update intervals

  // Vision mode

  // Screen capture

  // OCR configuration

  // Florence-2 configuration

  // Face recognition

  // Entity tracking

  // Performance

  // Logging

using VisionConfigInput = z::input<typeof VisionConfigSchema>;
using VisionConfigOutput = z::output<typeof VisionConfigSchema>;

class ConfigurationManager {
public:
    ConfigurationManager(const std::any& runtime);
    VisionConfigOutput loadConfiguration();
    std::string getSetting(const std::string& key);
    bool getBooleanSetting(const std::string& key, bool defaultValue);
    double getNumberSetting(const std::string& key, double defaultValue);
    VisionConfigOutput get();
    void update(const std::optional<VisionConfigInput>& updates);
    VisionConfig toLegacyFormat();
    static std::optional<VisionConfigInput> getPreset(const std::string& name);

private:
    VisionConfigOutput config_;
    std::any runtime_;
};


} // namespace elizaos
