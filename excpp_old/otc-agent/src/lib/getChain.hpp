#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Anvil chain with correct chain ID (31337)

/**
 * Get the appropriate chain based on environment and configuration
 * Supports: Base, BSC, Anvil/localhost
 */
Chain getChain();

/**
 * Get RPC URL for the current chain
 * Uses deployment config with env override support
 */
std::string getRpcUrl();

/**
 * Get RPC URL for a specific chain type
 * @param chainType - Chain identifier (base, bsc, localhost, etc.)
 */
std::string getRpcUrlForChain(const std::string& chainType);

} // namespace elizaos
