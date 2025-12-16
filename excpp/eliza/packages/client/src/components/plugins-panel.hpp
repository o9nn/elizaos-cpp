#include ".config/voice-models.hpp"
#include "ui/button.hpp"
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



struct PluginsPanelProps {
    Agent characterValue;
    { setCharacterValue;
    std::optional<(pluginId: string) => void> addPlugin;
    std::optional<(index: number) => void> removePlugin;
    std::optional<(plugins: string[]) => void> setPlugins;
    std::optional<<T>(path: string, value: T) => void> updateField;
    std::optional<std::vector<std::string>> initialPlugins;
};

// Define a type for the essential plugin information
using EssentialPluginInfo = {

// Map of essential plugins that require confirmation when removing


} // namespace elizaos
