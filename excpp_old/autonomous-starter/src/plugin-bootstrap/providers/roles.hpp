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
 * A provider for retrieving and formatting the role hierarchy in a server.
 * @type {Provider}
 */

      // Get world data instead of using cache

      // Get roles from world metadata

      // Group users by role

      // Process roles

        // get the user from the database

        // Skip duplicates (we store both UUID and original ID)

        // Add to appropriate group

      // Format the response


} // namespace elizaos
