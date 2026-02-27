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
#include "plugin-cdp/services/cdp.service.hpp"
#include "plugin-cdp/types.hpp"
#include "services/relay.service.hpp"
#include "utils/entity.hpp"
#include "utils/token-resolver.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Supported chains mapping

struct QuoteParams {
    std: originChain;
    std: destinationChain;
    std: currency;
    std::optional<std:> toCurrency;
    std: amount;
    std::optional<std:> recipient;
    std::optional<std:> tradeType;
};

/**
 * Resolve chain name to chain ID using viem chains
 */

  // Parameter schema for tool calling

      // Check if services are available

        // Get Relay service

          // Try to capture input params even in early failure
            // If we can't get params, just use empty object

        // Read parameters from state (extracted by multiStepDecisionTemplate)

        // Validate required parameters

        // Parse quote parameters with defaults

        // Store input parameters for return

        // Resolve chain names to IDs

        // Resolve token symbols to contract addresses

        // Get token decimals for proper amount conversion

        // Get quote from Relay

      // Serialize BigInt values to strings for storage

      // Format response

      // Try to capture input params even in failure
        // If we can't get params, just use empty object

std: formatQuoteResponse(Execute quote, double originChainId, double destinationChainId, const std:& amount, const std:& currency);

std: getChainName(double chainId);

std: formatAmount(const std:& amount, const std:& currency);


} // namespace elizaos
