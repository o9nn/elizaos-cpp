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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Entity ID utilities using Eliza's stringToUuid

/**
 * Detect if an address is a Solana address (Base58 encoded)
 * Solana addresses are case-sensitive and don't start with 0x
 */
bool isSolanaAddress(const std::string& address);

/**
 * Convert wallet address to deterministic UUID entity ID
 * Uses Eliza's built-in stringToUuid for consistency with runtime
 *
 * IMPORTANT: EVM addresses are case-insensitive (lowercased for consistency)
 * Solana addresses are case-sensitive (Base58 encoded, preserved as-is)
 */
std::string walletToEntityId(const std::string& address);

} // namespace elizaos
