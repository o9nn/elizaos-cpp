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



/**
 * The COMPLETE_GOAL action allows users to mark a goal as achieved.
 */

    // Try to extract goal reference from the message

      // Determine owner context (entity vs agent)

      // Extract goal information from the message

      // Try to find which goal to complete

      // Use Claude to find the best matching goal

      // Mark the goal as completed

    // Multi-action: Complete goal then create new one to show goal lifecycle transition
    // Multi-action: Complete goal then list remaining goals to show progress


} // namespace elizaos
