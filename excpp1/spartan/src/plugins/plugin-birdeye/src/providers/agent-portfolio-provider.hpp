#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".birdeye.hpp"
#include ".utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Agent portfolio data provider that queries Birdeye API for the agent's wallet address.
 * When a wallet address is set, this provider fetches portfolio data to give the agent
 * context about the agent's holdings when responding to queries.
 *
 * The provider:
 * - Validates the agent's wallet address
 * - Fetches current portfolio data from Birdeye including token balances and metadata
 * - Makes this portfolio context available to the agent for responding to user queries
 * about their holdings, token values, etc.
 */


} // namespace elizaos
