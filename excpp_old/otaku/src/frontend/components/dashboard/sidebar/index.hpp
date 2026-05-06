#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct Channel {
    std::string id;
    std::string name;
    std::optional<double> createdAt;
    std::optional<double> lastMessageAt;
};

  // Group channels by date

        // Format date as "MMM DD, YYYY" for items older than 30 days

    // Sort older dates in descending order (most recent first)
      // Get the first channel's timestamp from each group


} // namespace elizaos
