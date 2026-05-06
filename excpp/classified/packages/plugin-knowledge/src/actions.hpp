#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "service.ts.hpp"
#include "types.ts.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Action to process knowledge from files or text
 */

    // Check if the message contains knowledge-related keywords

    // Check if there's a file path mentioned

    // Check if service is available

      // Extract file path from message

        // Process file from path

        // Check if file exists

        // Read file

        // Determine content type

        // Prepare knowledge options

        // Process the document

        // Process direct text content

        // Prepare knowledge options for text

        // Process the text

/**
 * Action to search the knowledge base
 */

    // Check if the message contains search-related keywords

    // Check if service is available

      // Extract search query

      // Create search message

      // Search knowledge

        // Format results

// Export all actions

} // namespace elizaos
