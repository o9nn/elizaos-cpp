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

// Helper function to convert natural date to dd-mm-yyyy format
std::string parseDateToApiFormat(const std::string& dateStr);

      // Read parameters from state

      // Extract and validate token parameter (required)

      // Extract and validate date parameter (required)

      // Parse date to API format (dd-mm-yyyy)

      // Extract optional chain parameter (default to base for contract addresses)

      // Store input parameters for return

      // Fetch historical price data

      // Format the response

      // Create a narrative summary for the agent to format

      // Try to capture input params even in failure

**Tip**: Use GET_TOKEN_METADATA action first to retrieve the correct chain and contract address for non-native tokens.


} // namespace elizaos
