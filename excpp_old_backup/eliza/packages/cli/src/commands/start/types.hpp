#include "elizaos/core.hpp"
#include "elizaos/server.hpp"
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
 * Start command options and interfaces
 */

/**
 * Options for starting agents
 */
struct StartOptions {
    std::optional<double> port;
    std::optional<std::string> configDir;
};

/**
 * Agent start configuration
 */
struct AgentStartConfig {
    Character character;
    AgentServer server;
    std::optional<(runtime: IAgentRuntime) => Promise<void>> init;
    std::optional<std::vector<(Plugin | string)>> plugins;
    std::optional<AgentStartOptions> options;
};

/**
 * Agent start options
 */
struct AgentStartOptions {
    std::optional<bool> isTestMode;
};

/**
 * Plugin validation interface
 */
struct PluginValidation {
    bool isValid;
    std::optional<Plugin> plugin;
    std::optional<std::string> error;
};

/**
 * Plugin context information
 */
struct PluginContext {
    bool isLocalDevelopment;
    std::string name;
    std::optional<std::string> path;
};

/**
 * Server startup configuration
 */
struct ServerConfig {
    double port;
    std::optional<std::string> configDir;
    std::optional<bool> skipBuild;
};

/**
 * Runtime configuration
 */
struct RuntimeConfig {
    std::vector<Plugin> plugins;
    Character character;
    AgentServer server;
};


} // namespace elizaos
