#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Eliza client refresh functionality removed (not needed with direct client)

using ConnectionStatusType = std::variant<, 'loading', 'connected', 'reconnecting', 'error', 'unauthorized'>;

struct ConnectionContextType {
    ConnectionStatusType status;
    std::optional<std::string> error;
};

      // Update localStorage if a new API key is provided

      // Refresh the ElizaClient instance with new configuration
      // Client refresh not needed with direct client pattern


} // namespace elizaos
