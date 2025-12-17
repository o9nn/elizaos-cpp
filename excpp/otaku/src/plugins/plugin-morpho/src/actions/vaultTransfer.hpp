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



struct VaultTransferParams {
    std::string intent;
    std::string vault;
    std::string assets;
    std::optional<std::string> chain;
};

using VaultTransferInput = {

using VaultTransferActionResult = ActionResult;

      // Read parameters from state

      // Store input parameters for return

      // Validate intent

      // Validate vault

      // Validate assets

      // Determine chain - default to 'base' if not provided

      // Get services

      // Get entity wallet

      // Get viem clients for the account on the specified chain

      // Execute the transfer

      // Try to capture input params even in failure
        // If we can't get params, just use empty object


} // namespace elizaos
