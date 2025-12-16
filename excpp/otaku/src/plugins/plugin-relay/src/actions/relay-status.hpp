#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".services/relay.service.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct StatusParams {
    std::optional<std::string> requestId;
    std::optional<std::string> txHash;
    std::optional<std::string> user;
};

  // Parameter schema for tool calling

      // Check if services are available

      // Get Relay service

        // Try to capture input params even in early failure
          // If we can't get params, just use empty object

      // First, check if requestId and txHashes are available from previous action (bridge completion)
      // Check multiple possible locations where the data might be passed

        // If requestId is "pending" or not available, try using transaction hash
          // Use the first transaction hash (origin chain)

      // Try to find in recent messages if not found yet
          // Also check for tx hashes

      // If no direct data found, try to extract from actionParams

      // Validate that at least one identifier is provided

      // Store input parameters for return

      // Get status from Relay

      // Format response

      // Try to capture input params even in failure
        // If we can't get params, just use empty object

std::string formatStatusResponse(const std::vector<RelayStatus>& statuses);

std::string formatSingleStatus(RelayStatus status);

std::string getStatusIndicator(const std::string& status);

std::string getChainName(double chainId);


} // namespace elizaos
