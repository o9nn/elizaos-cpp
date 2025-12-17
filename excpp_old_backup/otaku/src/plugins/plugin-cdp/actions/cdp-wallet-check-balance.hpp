#include "...utils/entity.hpp"
#include ".services/cdp.service.hpp"
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



struct CheckBalanceParams {
    std::string token;
    std::optional<std::string> chain;
    std::optional<std::string> minAmount;
};

using CheckBalanceInput = {

using CheckBalanceActionResult = ActionResult & { input: CheckBalanceInput };

struct NormalizedTokenQuery {
    std::string canonicalSymbol;
    std::vector<std::string> searchSymbols;
    bool isPolygonNativeAlias;
    bool wasRequestedAsMatic;
};

      // Read parameters from state
      
      // Validate required parameters

      // Store input parameters (canonical symbol when possible)

      // Validate chain if provided

      // Get wallet info

      // Get CDP service

      // Use cached wallet info for speed (most cases will have recent data)
      
      // Pass wallet address to avoid CDP account lookup (prevents "account not initialized" errors)

      // Find the token in wallet

        // Token is a contract address
        // Token is a symbol - find across all chains or specific chain

      // Check if balance is sufficient (if minAmount provided)

      // Format response


} // namespace elizaos
