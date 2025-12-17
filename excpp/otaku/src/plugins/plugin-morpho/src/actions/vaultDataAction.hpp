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
#include "services.hpp"
#include "utils/entity.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct VaultInfoParams {
    std::optional<std::string> vault;
    std::optional<std::string> chain;
};

using VaultInfoInput = {

using VaultInfoActionResult = ActionResult;

 * Action: GET_MORPHO_VAULT_INFO

      // Read parameters from state

      // Store input parameters for return

      // Determine chain - default to 'base' if not provided

      // Get CDP service

      // Get entity wallet

      // Get viem clients for the account on the specified chain (though not strictly needed for vault data, maintain consistency)

      // Success message

      // Try to capture input params even in failure
        // If we can't get params, just use empty object


} // namespace elizaos
