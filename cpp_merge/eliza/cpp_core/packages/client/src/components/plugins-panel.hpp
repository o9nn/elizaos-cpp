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
    std::optional<[&](pluginId: std::string) { return void> addPlugin; };
    std::optional<[&](index) { return void> removePlugin; };
    std::optional<[&](plugins: std::string[]) { return void> setPlugins; };
    std::optional<<T>[&](path: std::string, value: T) { return void> updateField; };
    std::optional<std::vector<std::string>> initialPlugins;
};

// Define a type for the essential plugin information
using EssentialPluginInfo = {

// Map of essential plugins that require confirmation when removing


} // namespace elizaos
