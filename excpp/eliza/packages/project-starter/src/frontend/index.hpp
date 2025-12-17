#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define the interface for the ELIZA_CONFIG
struct ElizaConfig {
    std::string agentId;
    std::string apiBase;
};

// Declare global window extension for TypeScript
  struct Window {
    std::optional<ElizaConfig> ELIZA_CONFIG;
};

/**
 * Main Example route component
 */
void ExampleRoute();

/**
 * Example provider component
 */
void ExampleProvider();

// Initialize the application - no router needed for iframe

// Define types for integration with agent UI system
struct AgentPanel {
    std::string name;
    std::string path;
    ComponentType<any> component;
    std::optional<std::string> icon;
    std::optional<bool> public;
    std::optional<std::string> shortLabel;
};

struct PanelProps {
    std::string agentId;
};

/**
 * Example panel component for the plugin system
 */

// Export the panel configuration for integration with the agent UI

* from './utils';

} // namespace elizaos
