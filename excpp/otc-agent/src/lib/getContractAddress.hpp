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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Get the appropriate OTC contract address based on network configuration.
 * 
 * @deprecated Use `getOtcAddress()` from `@/config/contracts` directly.
 * This wrapper exists for backwards compatibility.
 */
Address getContractAddress();

} // namespace elizaos
