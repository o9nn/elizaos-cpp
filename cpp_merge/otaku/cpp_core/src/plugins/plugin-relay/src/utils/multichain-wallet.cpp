#include "multichain-wallet.hpp"
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

MultiChainWallet createMultiChainWallet(Account account, std::optional<std:> defaultRpcUrl) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new MultiChainWallet(account, defaultRpcUrl);

}

} // namespace elizaos
