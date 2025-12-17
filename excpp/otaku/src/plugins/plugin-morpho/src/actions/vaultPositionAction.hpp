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
#include ".services.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct VaultPositionsParams {
    std::optional<std::string> vault;
    std::optional<std::string> chain;
};

using VaultPositionsInput = {

using VaultPositionsActionResult = ActionResult & { input: VaultPositionsInput };

// Helper functions to format vault position data
std::string normalizeUnitsFromApi(const std::variant<std::string, double>& raw, double decimals);

void formatVaultPositionData(const std::vector<UserVaultPosition>& vaults);

 * Action: GET_MORPHO_VAULT_POSITIONS

      // Read parameters from state

      // Store input parameters for return

      // Determine chain - default to 'base' if not provided

      // Get CDP service

      // Get entity wallet

      // Get viem clients for the account on the specified chain

      // Fetch vault positions

      // Optional filter by vault name substring or exact address

      // Success message

      // Format vault position data for frontend consumption

      // Try to capture input params even in failure
        // If we can't get params, just use empty object


} // namespace elizaos
