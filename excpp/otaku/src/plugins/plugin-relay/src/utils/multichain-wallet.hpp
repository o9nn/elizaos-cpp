#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Map of chain IDs to viem Chain objects

// Default RPC URLs for each chain

/**
 * Multi-chain wallet adapter for Relay SDK
 * Dynamically creates wallet clients for each chain as needed
 */
class MultiChainWallet {
public:
    MultiChainWallet(Account account, std::optional<std::string> defaultRpcUrl, double initialChainId = base.id);
    WalletClient getWalletClient(double chainId);
    std::future<double> getChainId();
    std::future<void> switchChain(double chainId);
    std::future<std::string> address();
    WalletClient getCurrentWalletClient();
    Account getAccount();

private:
    Account account_;
    double currentChainId_;
};

/**
 * Create a Relay-compatible wallet adapter from an account
 */
MultiChainWallet createMultiChainWallet(Account account, std::optional<std::string> defaultRpcUrl);


} // namespace elizaos
