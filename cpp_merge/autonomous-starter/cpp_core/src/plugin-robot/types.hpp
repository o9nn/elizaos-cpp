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

// Extend the core service types with robot service
  struct ServiceTypeRegistry {
    "ROBOT" ROBOT;
};

// Export service type constant

struct ScreenObject {
    std: label;
    double y;
    double width;
};

struct ScreenActionStep {
    std: action;
    std::optional<double> x;
    std::optional<double> y;
    std::optional<std:> text;
    std::optional<std:> button;
};

struct ScreenDescription {
    std: description;
    double timestamp;
    std: relativeTime;
};

struct ScreenContext {
    std::vector<uint8_t> screenshot;
    std: currentDescription;
    std::vector<ScreenDescription> descriptionHistory;
    std: ocr;
    std::vector<ScreenObject> objects;
    double timestamp;
    bool changeDetected;
    std::optional<double> pixelDifferencePercentage;
};

struct ChangeDetectionConfig {
    double threshold;
    bool enabled;
};

struct RobotServiceConfig {
    double cacheTTL;
    ChangeDetectionConfig changeDetection;
    double maxHistoryEntries;
};


} // namespace elizaos
