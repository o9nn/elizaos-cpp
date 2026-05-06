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


using Route = std::variant<{

/**
 * Plugin for extending agent functionality
 */

using PluginEvents = std::vector<{

struct Plugin {
    std::string name;
    std::string description;
    std::optional<(config: Record<string, string>, runtime: IAgentRuntime) => Promise<void>> init;
    std::optional<{ [key: string]: any }> config;

struct ProjectAgent {
    Character character;
    std::optional<(runtime: IAgentRuntime) => Promise<void>> init;
    std::optional<std::vector<Plugin>> plugins;
    std::optional<std::vector<TestSuite | TestSuite>> tests;
};

struct Project {
    std::vector<ProjectAgent> agents;
};


} // namespace elizaos
