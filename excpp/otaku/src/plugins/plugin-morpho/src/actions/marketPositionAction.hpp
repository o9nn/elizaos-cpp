#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "....utils/entity.hpp"
#include "...plugin-cdp/services/cdp.service.hpp"
#include ".services.hpp"
#include ".types.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct MarketPositionsParams {
    std::optional<std::string> market;
    std::optional<std::string> chain;
};

using MarketPositionsInput = {

using MarketPositionsActionResult = ActionResult & { input: MarketPositionsInput };

// Helper function to format market position data
void formatMarketPositionData(const std::vector<UserPosition>& positions);

 * Action: GET_MORPHO_MARKET_POSITIONS

      // Read parameters from state

      // Store input parameters for return

      // Determine chain - default to 'base' if not provided

      // Get CDP service

      // Get entity wallet

      // Get viem clients for the account on the specified chain

      // Fetch positions

      // Success message

      // Format market position data for frontend consumption

      // Try to capture input params even in failure
        // If we can't get params, just use empty object


} // namespace elizaos
