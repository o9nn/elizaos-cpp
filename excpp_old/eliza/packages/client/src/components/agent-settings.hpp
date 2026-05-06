#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "avatar-panel.hpp"
#include "elizaos/core.hpp"
#include "plugins-panel.hpp"
#include "secret-panel.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



  // Use our enhanced agent update hook for more intelligent handling of JSONb fields

  // Use agent management hook for stop functionality

    // Define the actual save logic
        // Get secrets from state (or ref as fallback)

        // Get only the fields that have changed

        // Manually add secrets to changedFields if they exist
          // Ensure settings object exists in changedFields

          // Add only active secrets to the settings (exclude deleted ones)

        // No need to send update if nothing changed
          // Force include secrets if they exist even if no other changes detected

        // Always include the ID

        // Send the partial update

        // Invalidate both the agent query and the agents list

        // Call the onSaveComplete callback if provided, otherwise navigate

    // Validate required secrets if we have a secret panel ref
        // Show confirmation dialog for missing secrets

    // If validation passes or no secret panel, proceed with save

      // Set deleting state

      // Show a toast to indicate deletion is in progress

        // Set a timeout to navigate away if the deletion takes too long

        // Handle partial success response

        // Invalidate queries and navigate away regardless

        // Handle specific error codes

            // Conflict - agent has references that prevent deletion
            // Timeout - operation is still running in background

            // Still navigate away
            // Generic error
          // Unknown error
      // Outer try/catch fallback

                  // Also update the agent state so changes persist across tab switches


} // namespace elizaos
