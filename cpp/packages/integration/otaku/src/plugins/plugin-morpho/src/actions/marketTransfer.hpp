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
#include "constants/chains.hpp"
#include "elizaos/core.hpp"
#include "plugin-cdp/services/cdp.service.hpp"
#include "services.hpp"
#include "utils/entity.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct MarketTransferParams {
    std::optional<std::string> intent;
    std::optional<std::string> market;
    std::optional<std::string> assets;
    std::optional<bool> fullRepayment;
    std::optional<std::string> chain;
};

using MarketTransferInput = {

using MarketTransferActionResult = ActionResult;

std::string getOperationEmoji(const std::string& intent);

std::string getOperationDescription(const std::string& intent);

      // Read parameters from state

      // Store input parameters for return

      // Validate intent

      // Validate market

      // Validate assets (required for all operations except full repayment)

      // Validate assets amount

      // Get services

      // Determine chain - default to 'base' if not provided

      // Get entity wallet

      // Get viem clients for the account on the specified chain

      // Execute the appropriate operation

      // Build transaction URLs

      // Try to capture input params even in failure
        // If we can't get params, just use empty object


} // namespace elizaos
