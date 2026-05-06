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



// Define the interface for the ELIZA_CONFIG
struct ElizaConfig {
    std::string agentId;
    std::string apiBase;
};

// Define the interface for time response
struct TimeResponse {
    std::string timestamp;
    double unix;
    std::string formatted;
    std::string timezone;
};

/**
 * Time display component that fetches from backend
 */
void TimeDisplay({ apiBase: string } { apiBase });

/**
 * Main Example route component
 */
void ExampleRoute();

/**
 * Example provider component
 */
void ExampleProvider(auto { agentId, { agentId: UUID; apiBase: string } apiBase });

// Initialize the application - no router needed for iframe

// Define types for integration with agent UI system
struct AgentPanel {
    std::string name;
    std::string path;
    React.ComponentType<any> component;
    std::optional<std::string> icon;
    std::optional<bool> public;
    std::optional<string; // Optional short label for mobile> shortLabel;
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
