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

;
;

// Anvil chain with correct chain ID (31337)
const anvil: Chain = {
  ...localhost,
  id: 31337,
  name: "Anvil",
};

/**
 * Get the appropriate chain based on environment and configuration
 * Supports: Base, BSC, Anvil/localhost
 */


/**
 * Get RPC URL for the current chain
 * Uses deployment config with env override support
 */


/**
 * Get RPC URL for a specific chain type
 * @param chainType - Chain identifier (base, bsc, localhost, etc.)
 */

}

} // namespace elizaos
