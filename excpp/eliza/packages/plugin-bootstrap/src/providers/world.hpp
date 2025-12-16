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
 * Provider that exposes relevant world/environment information to agents.
 * Includes details like channel list, world name, and other world metadata.
 */

      // Get the current room from the message

      // Get the world for the current room

      // Get all rooms in the current world

      // Get participants for the current room

      // Format rooms by type
      using RoomInfo = {

      // Categorize rooms by type

        // Group channels by their purpose

      // Create formatted text for display

      // Build the world information object with formatted data

      // Use addHeader like in entitiesProvider


} // namespace elizaos
