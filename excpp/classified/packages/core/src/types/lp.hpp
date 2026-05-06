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
#include "service.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * A standardized representation of a liquidity pool from any DEX.
 */
using PoolInfo = {

/**
 * A standardized representation of a user's position in a liquidity pool.
 */
using LpPositionDetails = {

/**
 * A standardized result for blockchain transactions.
 */
using TransactionResult = {

/**
 * Abstract interface for a Liquidity Pool Service.
 * DEX-specific plugins (e.g., for Orca, Raydium) must implement this service
 * to allow the LP Manager to interact with them in a standardized way.
 */


} // namespace elizaos
