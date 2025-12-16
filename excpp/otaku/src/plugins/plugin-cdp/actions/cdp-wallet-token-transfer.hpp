#include "...utils/entity.hpp"
#include ".services/cdp.service.hpp"
#include ".types.hpp"
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



// WETH contract address on Polygon (bridged from Ethereum via PoS Bridge)

struct TransferParams {
    CdpNetwork network;
    `0x${string}` to;

  // Parameter schema for tool calling
  
      // Check if CDP service is available

      // Ensure the user has a wallet saved

      // Read parameters from state (extracted by multiStepDecisionTemplate)

      // Validate required parameters

      // Validate recipient address format

      // Validate that we have either amount OR percentage

      // Parse transfer parameters

      // Store input parameters for return

      // Get user's wallet info to find the token (use cached data if available)
      // Pass wallet address to avoid CDP account lookup (prevents "account not initialized" errors)

      // Determine token type for CDP API

      // Calculate amount based on percentage or use provided amount
      
        // Calculate amount from percentage

        // Convert back to formatted string for display
        
        // Calculate USD value from already-available wallet token data (no extra fetch)
        
        // Calculate USD value from already-available wallet token data (no extra fetch)
      
      // Parse amount to proper units

      // Execute transfer via service method

      // Try to capture input params even in failure


} // namespace elizaos
