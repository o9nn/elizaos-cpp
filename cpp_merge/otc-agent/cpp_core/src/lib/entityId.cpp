#include "entityId.hpp"
#include <string>
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isSolanaAddress(const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Solana addresses are Base58 (no 0x prefix, typically 32-44 chars)
    // They contain alphanumeric chars but no 0, O, I, l (Base58)
    return (;
    !address.substr(0, "0x") && address.size() >= 32 && address.size() <= 44;
    );

}

std::string walletToEntityId(const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto trimmed = address;
    // Preserve case for Solana addresses, lowercase for EVM
    const auto normalized = isSolanaAddress(trimmed) ? trimmed : trimmed.toLowerCase();
    return stringToUuid(normalized);

}

} // namespace elizaos
