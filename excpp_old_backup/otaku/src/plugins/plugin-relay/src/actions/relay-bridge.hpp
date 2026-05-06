#include "....utils/entity.hpp"
#include "...plugin-cdp/services/cdp.service.hpp"
#include "...plugin-cdp/types.hpp"
#include ".services/relay.service.hpp"
#include ".types.hpp"
#include ".utils/token-resolver.hpp"
#include "elizaos/core.hpp"
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



// Supported chains mapping

/**
 * Resolve chain name to chain ID using viem chains
 * Similar to how we resolve token symbols in CDP swap
 */

  // Parameter schema for tool calling

      // Check if services are available

        // Get Relay service

          // Try to capture input params even in early failure
            // If we can't get params, just use empty object

        // Read parameters from state (extracted by multiStepDecisionTemplate)

        // Validate required parameters

        // Parse bridge parameters with defaults

        // Store input parameters for return

        // Calculate USD volume for gamification
          // Get user's wallet info to calculate USD value
          // Pass wallet address to avoid CDP account lookup (prevents "account not initialized" errors)
          
          // Find the token in wallet to get price/value
            
            // Match by address if provided
            
            // Match by symbol

            // Calculate proportion of total value
          // Continue without volumeUsd - shouldn't block the bridge

        // Resolve chain names to IDs

        // Resolve token symbols to contract addresses on BOTH chains

        // Get token decimals for proper amount conversion

      // Create resolved bridge request with chain IDs and contract addresses
      // Create resolved bridge request

      // Execute bridge

      // Helper to serialize BigInt for logging

      // Track transaction hashes as they come in

          // Collect transaction hashes from progress updates

          // Extract meaningful progress information
          
          // Only send callback if there's an actual status change

      // Helper to fetch status (tries requestId, falls back to txHash)

      // Poll for final status until complete
      // Bridge operations can take longer due to cross-chain nature

      // Extract actual requestId from status if available

      // Format response (using serializeBigInt helper defined above)

      // Try to capture input params even in failure
        // If we can't get params, just use empty object

std::string formatBridgeResponse(RelayStatus | undefined status, ResolvedBridgeRequest request, const std::string& requestId, const std::vector<std::vector<{ txHash: string; chainId: number }>>& collectedTxHashes, std::optional<std::string> tokenSymbol);

std::string getChainName(double chainId);

std::string formatAmount(const std::string& amount, const std::string& currency);


} // namespace elizaos
