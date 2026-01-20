#include "coingecko.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isEvmAddress(const std::string& s) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return /^0x[a-fA-F0-9]{40}$/.test(s);

}

bool isSolanaAddress(const std::string& s) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return /^[1-9A-HJ-NP-Za-km-z]{32,44}$/.test(s);

}

} // namespace elizaos
