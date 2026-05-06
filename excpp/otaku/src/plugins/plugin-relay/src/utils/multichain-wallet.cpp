#include "multichain-wallet.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

MultiChainWallet createMultiChainWallet(Account account, std::optional<std::string> defaultRpcUrl) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new MultiChainWallet(account, defaultRpcUrl);

}

} // namespace elizaos
