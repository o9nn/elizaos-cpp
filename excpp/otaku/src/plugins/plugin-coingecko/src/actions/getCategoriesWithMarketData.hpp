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
#include "services/coingecko.service.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



  // Parameter schema for tool calling

      // Read parameters from state (extracted by multiStepDecisionTemplate)

      // Extract and validate order parameter
      
      using OrderType = typeof validOrders[number];

      // Store input parameters for return

      // Fetch categories with market data

      // Try to capture input params even in failure


} // namespace elizaos
