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
#include "config/voice-models.hpp"
#include "ui/button.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct PluginsPanelProps {
    Agent characterValue;
    std::optional<std::vector<std::string>> initialPlugins;
};

// Define a type for the essential plugin information
using EssentialPluginInfo = {

// Map of essential plugins that require confirmation when removing


} // namespace elizaos
