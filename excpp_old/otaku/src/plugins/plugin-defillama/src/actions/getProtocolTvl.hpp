#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".services/defillama.service.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Extend Action type to support parameter schemas for tool calling

  // Parameter schema for tool calling

      // Read parameters from state (extracted by multiStepDecisionTemplate)

      // Extract and validate protocols parameter (required)

      // Parse comma-separated protocol names

      // Store input parameters for return

      // Fetch protocol TVL data

      // Try to capture input params even in failure


} // namespace elizaos
