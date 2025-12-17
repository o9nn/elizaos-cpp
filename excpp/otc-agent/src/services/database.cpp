#include "database.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string normalizeTokenId(const std::string& tokenId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto match = tokenId.match(/^token-([a-z]+)-(.+)$/);
    if (!match) return tokenId;
    const auto [, chain, address] = match;
    // Solana addresses are case-sensitive (Base58), preserve them
    if (chain == "solana") return tokenId;
    // EVM addresses are case-insensitive, lowercase for consistency
    return "token-" + chain + "-" + std::to_string(address.toLowerCase());

}

} // namespace elizaos
