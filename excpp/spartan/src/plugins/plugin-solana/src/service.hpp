#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "constants.hpp"
#include "elizaos/core.hpp"
#include "keypairUtils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Service class for interacting with the Solana blockchain and accessing wallet data.
 * @extends Service
 */
class SolanaService {
public:
    SolanaService();
    std::future<Keypair> getWalletKeypair();
    static std::future<SolanaService> start(IAgentRuntime runtime);
    static void stop(IAgentRuntime runtime);
    std::future<unknown> fetchWithRetry(const std::string& url, RequestInit options = {});
    std::future<Prices> fetchPrices();
    void getTokenAccounts();
    std::future<WalletPortfolio> updateWalletData(auto force);
    std::future<WalletPortfolio> forceUpdate();
    PublicKey getPublicKey();
    Connection getConnection();
    bool validateAddress(const std::string& address);
    void registerExchange(const std::any& provider);
    std::future<double> subscribeToAccount(const std::string& accountAddress);
    std::future<bool> unsubscribeFromAccount(const std::string& accountAddress);
    void executeBuy(const std::vector<std::any>& wallets, const std::any& signal);

/**
 * Gets either a keypair or public key based on TEE mode and runtime settings
 * @param runtime The agent runtime
 * @param requirePrivateKey Whether to return a full keypair (true) or just public key (false)
 * @returns KeypairResult containing either keypair or public key
 */
std::future<WalletResult> loadWallet(IAgentRuntime runtime, bool requirePrivateKey = true);
std::future<std::string> sendTransaction(Connection connection, const std::vector<std::any>& instructions, Keypair wallet);

} // namespace elizaos
