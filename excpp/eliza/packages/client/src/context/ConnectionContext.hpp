#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Eliza client refresh functionality removed (not needed with direct client)

using ConnectionStatusType = std::variant<, std::string>;

struct ConnectionContextType {
    ConnectionStatusType status;
    std::string error;
};

      // Update localStorage if a new API key is provided

      // Refresh the ElizaClient instance with new configuration
      // Client refresh not needed with direct client pattern


} // namespace elizaos
