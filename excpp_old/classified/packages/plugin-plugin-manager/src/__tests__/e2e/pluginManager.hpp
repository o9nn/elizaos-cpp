#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "..services/pluginManagerService.ts.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Mock database adapter since we don't need a real DB for plugin tests
class MockDatabaseAdapter {
public:
    void getMemories();
    void searchMemories();
    void createMemory();
    void getMemoriesByRoomIds();
    void getGoals();
    void getConversation();
    void createConversation();
    void saveMemory();
};

    // Create a temporary directory for testing

    // Create a mock runtime with minimal required properties

      // Core methods

    // Initialize plugin manager

    // Clean up

      // Create a test plugin

      // Register the plugin

      // Verify plugin is loaded

      // Verify action is registered

      // Execute the action

      // Unload the plugin

      // Verify action is removed

      // Verify provider is registered

      // Verify evaluator is registered

      // Test provider

      // Unload and verify cleanup

      // Emit the event

      // Wait a bit for async event handling

      // Unload and test event removal

      // Load both plugins

      // Verify both plugins' components are registered

      // Unload plugin 1

      // Verify plugin 1 components are removed but plugin 2 remains

      // Create a simple plugin in the temp directory

      // Create package.json

      // Create index.js

      // Install the local bundle

      // Load the installed plugin

      // Verify the action is available

      // Execute the action

      class TestService {
public:
    void serviceType() const;
    void start(IAgentRuntime runtime);
    void stop();
    void getData();
};

      // Verify service is started

      // Test service functionality

      // Unload and verify service is stopped

      // Verify plugin status

      // Action should throw but not crash the system

      // System should still be functional

} // namespace elizaos
