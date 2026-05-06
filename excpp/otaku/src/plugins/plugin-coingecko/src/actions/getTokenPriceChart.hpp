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



// Helper function to format market cap values
std::string formatMarketCap(double value);

      // Read parameters from state

      // Extract and validate token parameter (required)

      // Extract and validate chain parameter (required)

      // Extract optional timeframe parameter

      // Store input parameters for return

      // Fetch price chart data

      // Calculate price change

      // Calculate market cap change

      // Create a narrative summary for the agent to format

      // Try to capture input params even in failure

 **Tip**: Use GET_TOKEN_METADATA action first to retrieve the correct chain and contract address for non-native tokens.


} // namespace elizaos
