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



struct NftTransferParams {
    CdpNetwork network;
    `0x${string}` to;

  // Parameter schema for tool calling
  
      // Check if CDP service is available

      // Ensure the user has a wallet saved

      // Read parameters from state (extracted by multiStepDecisionTemplate)

      // Store input parameters early for debugging (even if validation fails)

      // Validate required parameters

      // Validate recipient address format

      // Validate contract address format

      // Parse transfer parameters

      // Verify the user owns this NFT
      // Pass wallet address to avoid CDP account lookup (prevents "account not initialized" errors)

      // Execute NFT transfer via service method

      // Try to capture input params for debugging (in case error happened very early)
        // If we can't get state, use empty object


} // namespace elizaos
