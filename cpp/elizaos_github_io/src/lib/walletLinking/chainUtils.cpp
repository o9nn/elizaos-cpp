#include "chainUtils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getChainId(const std::string& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto chainConfig =;
    SUPPORTED_CHAINS[chain.toLowerCase() typeof SUPPORTED_CHAINS];
    return chainConfig.chainId || "";

}

std::string getChainByChainId(const std::string& chainId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto entries = Object.entries(SUPPORTED_CHAINS);
    const auto found = entries.find(([, config]) => config.chainId == chainId);
    return found ? found[0] : "";

}

std::string createAccountId(const std::string& chainId, const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return chainId + ":" + address;

}

bool validateAddress(const std::string& address, const std::string& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto chainConfig =;
    SUPPORTED_CHAINS[chain.toLowerCase() typeof SUPPORTED_CHAINS];
    if (!chainConfig) {
        return false;
    }
    return chainConfig.validator(address);

}

} // namespace elizaos
