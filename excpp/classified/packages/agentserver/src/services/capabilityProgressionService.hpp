#include "elizaos/core.hpp"
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



struct ProgressionLevel {
    std::string id;
    std::string name;
    std::string description;
    std::vector<std::string> unlockedCapabilities;
    std::vector<UnlockCondition> unlockConditions;
    bool isUnlocked;
};

struct UnlockCondition {
    'action_performed' | 'form_submitted' | 'capability_used' | 'agent_named' type;
    std::optional<std::string> capability;
    std::optional<std::string> action;
    std::string description;
};

struct ProgressionState {
    double currentLevel;
    std::vector<std::string> unlockedLevels;
    std::vector<std::string> completedActions;
    bool agentNamed;
    Date | null lastUnlockTime;
};

class CapabilityProgressionService extends Service {
  static serviceType = ServiceType.OTHER;
  static serviceName = 'CAPABILITY_PROGRESSION';
  capabilityDescription = 'Manages progressive capability unlocking and level progression';

  static async start(runtime: IAgentRuntime): Promise<CapabilityProgressionService> {
    const service = new CapabilityProgressionService(runtime);
    return service;
  }

    // Check if unlocked mode is enabled via settings

    // If unlocked mode, unlock all levels immediately

    // Register all capabilities immediately

    // Skip progression checks in unlocked mode

    // Register new plugins for this level

    // Inject system message about the unlock

    // Use the ProgressivePluginService to register capabilities

      // Get the server instance from the global scope

        // Create a system message and inject it into the message flow

        // Broadcast the message through the WebSocket system

  // Public methods for tracking actions

  // Getters for current state

      // Switching to unlocked mode - unlock everything
      // Switching to progression mode - reset to initial state

    // Update runtime setting

    // Reset state

    // Reset level states

    // No cleanup needed for this service

} // namespace elizaos
