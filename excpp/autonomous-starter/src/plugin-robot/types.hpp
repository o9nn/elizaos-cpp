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

// Extend the core service types with robot service
  struct ServiceTypeRegistry {
    "ROBOT" ROBOT;
};

// Export service type constant

struct ScreenObject {
    std::string label;
    { x: number; y: number; width: number; height: number } bbox;

struct ScreenActionStep {
    "move" | "click" | "type" action;
    std::optional<double> x;
    std::optional<double> y;
    std::optional<std::string> text;
    std::optional<"left" | "right" | "middle"> button;
};

struct ScreenDescription {
    std::string description;
    double timestamp;
    std::string relativeTime;
};

struct ScreenContext {
    Buffer screenshot;
    std::string currentDescription;
    std::vector<ScreenDescription> descriptionHistory;
    std::string ocr;
    std::vector<ScreenObject> objects;
    double timestamp;
    bool changeDetected;
    std::optional<double> pixelDifferencePercentage;
};

struct ChangeDetectionConfig {
    number; // Percentage of pixels that must change to trigger AI processing threshold;
    bool enabled;
};

struct RobotServiceConfig {
    double cacheTTL;
    ChangeDetectionConfig changeDetection;
    double maxHistoryEntries;
};


} // namespace elizaos
