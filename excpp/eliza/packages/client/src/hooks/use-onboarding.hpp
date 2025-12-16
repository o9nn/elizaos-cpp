#include ".lib/logger.hpp"
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



// Key for storing onboarding state in localStorage

/**
 * Custom hook to manage the onboarding state
 * Tracks whether the user has completed the onboarding tour
 */
  // Check if user has completed the onboarding
      // Try to get the stored value from localStorage
      // If there's an error (e.g. localStorage not available), assume not completed

  // Update localStorage when onboardingCompleted changes

  // Function to mark onboarding as completed

  // Function to reset onboarding state (for testing)


} // namespace elizaos
