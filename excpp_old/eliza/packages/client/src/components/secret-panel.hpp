#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using EnvVariable = {

struct SecretPanelProps {
    Agent characterValue;
};

struct SecretPanelRef {
};

    // Raw editor modal state

    // Get required secrets based on enabled plugins

    // Function to get current secrets

    // Function to validate if all required secrets are provided

      // If we're still loading required secrets, check against current envs marked as required

    // Expose methods to get current secrets state and validate

    // Toggle visibility for a specific secret

    // Convert envs to raw editor format - include ALL variables, even empty required ones

      // Add a header comment

      // Group by required vs optional

      // Add required secrets section

      // Add optional secrets section

    // Parse raw editor content back to envs

    // Handle raw editor save - this should trigger the parent onChange

      // Create a new envs array with updates from raw editor
          // Keep required secrets even if not in parsed content
          // Remove non-required secrets that aren't in parsed content

      // Add new secrets from parsed content

      // Sort: required secrets first, then alphabetically

      // The useEffect watching envs will trigger onChange to parent

    // Open raw editor modal

    // Copy env content to clipboard

    // Load initial secrets from characterValue and merge with required secrets
      // Skip if still loading secrets

      // Only reset if we're switching to a different agent or this is the first load
      // or if envs is empty (meaning we haven't initialized yet)
        // Decrypt secrets from the server using the core decryption function
        // Ensure we're working with a plain object

        // Create a map for quick lookup

        // Add required secrets that don't exist yet

            // Update existing secret with required metadata

        // Sort: required secrets first, then alphabetically

        // Also reset the required secrets tracking

    // Sync secrets when plugins change (not just when agent changes)
      // Skip only if still loading secrets

      // Create a stable key for comparison

      // Only update if the required secrets actually changed

      // Get current required secret names

      // Update existing envs
            // Check if this secret is still required

            // If it was required but no longer is, remove it entirely (regardless of value)
              // Mark this secret for deletion in parent component immediately

            // Update metadata if still required

            // Keep non-required envs that have values

        // Add new required secrets that don't exist

        // Sort: required secrets first, then alphabetically

    // Force cleanup of non-required secrets (alternative approach)

            // Mark for deletion

    // Notify parent of changes

      // Create a debounced version to avoid rapid fire updates

        // Also check for secrets that should be removed
        // Get all env names that are currently in the list

        // Check if there are any secrets in the character settings that are no longer in our envs
        // This happens when a plugin is removed and its required secrets are cleaned up
            // Mark this secret for deletion

        // Only call onChange if secrets actually changed
        // Sort keys to ensure consistent comparison

            // Sort: required secrets first, then alphabetically

          // Sort: required secrets first, then alphabetically

      // Close any other editing

      // Show the secret when editing

      // Hide the secret after saving

      // Hide the secret after canceling

    // Get missing required secrets

    // Custom scrollbar styles for input containers - visible scrollbar like Railway

                        // Focus on value input on mobile

// Also provide a default for backward compatibility

} // namespace elizaos
