#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "errors.js.hpp"
#include "retry.js.hpp"
#include "security.js.hpp"
#include "service.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Configuration schema

// Helper function to extract URL from text
std::string extractUrl(const std::string& text);

// Browser navigation action

      // Track browser usage for progression

// BROWSER_CLICK action implementation

      // Extract what to click from the message

      // Track browser usage for progression

      // Check if this is a form submission button

// BROWSER_TYPE action implementation

      // Extract what to type from the message (in quotes)

      // Extract where to type (after "in" or "into")

      // Track browser usage for progression

      // Check if this might be a form submission (typing in a form field)

// BROWSER_SELECT action implementation

      // Extract what to select (in quotes)

      // Extract from where (after "from")

// BROWSER_EXTRACT action implementation

      // Extract what to extract/find

// BROWSER_SCREENSHOT action implementation

// Browser state provider

// NOTE: For brevity, I'm only implementing two actions here.
// In the full implementation, all actions from the original plugin would be converted
// to use the WebSocket client instead of direct Stagehand calls.

// Test function to verify Stagehand works
std::future<void> testStagehandConnection(IAgentRuntime runtime);

// Export actions for testing

      // Schedule the Google test with a more robust approach
      // Check periodically if the runtime is ready before running the test

        // Check if runtime has the service available
          // Run the test after a short delay to ensure everything is ready
          // If service not available yet, check again in 1 second

      // Start checking for service availability after 3 seconds
    // TODO: Add back, forward, refresh actions

} // namespace elizaos
