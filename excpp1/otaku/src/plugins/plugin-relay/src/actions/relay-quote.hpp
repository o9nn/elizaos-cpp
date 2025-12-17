#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "....utils/entity.hpp"
#include "...plugin-cdp/services/cdp.service.hpp"
#include "...plugin-cdp/types.hpp"
#include ".services/relay.service.hpp"
#include ".utils/token-resolver.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Supported chains mapping

struct QuoteParams {
    std::string originChain;
    std::string destinationChain;
    std::string currency;
    std::optional<std::string> toCurrency;
    std::string amount;
    std::optional<std::string> recipient;
    std::optional<std::variant<"EXACT_INPUT", "EXACT_OUTPUT">> tradeType;
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

std::string formatQuoteResponse(Execute quote, double originChainId, double destinationChainId, const std::string& amount, const std::string& currency);

std::string getChainName(double chainId);

std::string formatAmount(const std::string& amount, const std::string& currency);


} // namespace elizaos
