#include ".services/etherscan.service.hpp"
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



  // Parameter schema for tool calling

      // Read parameters from state (extracted by multiStepDecisionTemplate)

      // Extract transaction hash from parameters or message text

      // Fallback: Extract from message text if not in parameters
      
        // Look for Ethereum transaction hash (0x followed by 64 hex characters)

      // Extract chain if not provided

      // Store input parameters for return

      // Get Etherscan service

      // Get transaction receipt with confirmations

      // Format the response

      // Add confirmation status interpretation

      // Try to capture input params even in failure


} // namespace elizaos
