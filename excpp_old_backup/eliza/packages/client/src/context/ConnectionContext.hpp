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



// Eliza client refresh functionality removed (not needed with direct client)

using ConnectionStatusType = std::variant<, 'loading', 'connected', 'reconnecting', 'error', 'unauthorized'>;

struct ConnectionContextType {
    ConnectionStatusType status;
    string | null error;
    (message: string) => void setUnauthorizedFromApi;
    (isOffline: boolean) => void setOfflineStatusFromProvider;
    (newApiKey?: string | null) => void refreshApiClient;
};

      // Update localStorage if a new API key is provided

      // Refresh the ElizaClient instance with new configuration
      // Client refresh not needed with direct client pattern


} // namespace elizaos
