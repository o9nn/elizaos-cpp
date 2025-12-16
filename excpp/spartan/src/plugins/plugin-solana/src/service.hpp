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
    SolanaService(IAgentRuntime protected runtime);
    std::future<Keypair> getWalletKeypair();
    std::future<SolanaService> start(IAgentRuntime runtime);
    void stop(IAgentRuntime runtime);
    std::future<void> stop();
    std::future<unknown> fetchWithRetry(const std::string& url, RequestInit = {} options);
    std::future<Prices> fetchPrices();
    void getTokenAccounts();
    std::future<WalletPortfolio> updateWalletData(auto force = false);
    std::future<WalletPortfolio> forceUpdate();
    PublicKey getPublicKey();
    Connection getConnection();
    bool validateAddress(const std::string& address);
    void catch(auto error);
    void registerExchange(const std::any& provider);
    std::future<double> subscribeToAccount(const std::string& accountAddress);
    std::future<bool> unsubscribeFromAccount(const std::string& accountAddress);
    void if(auto priceImpact > 5);
    void catch(auto error);
    void executeBuy(const std::vector<std::any>& wallets, const std::any& signal);

/**
 * Gets either a keypair or public key based on TEE mode and runtime settings
 * @param runtime The agent runtime
 * @param requirePrivateKey Whether to return a full keypair (true) or just public key (false)
 * @returns KeypairResult containing either keypair or public key
 */
std::future<WalletResult> loadWallet(IAgentRuntime runtime, boolean = true requirePrivateKey);
std::future<std::string> sendTransaction(Connection connection, const std::vector<std::any>& instructions, Keypair wallet);

} // namespace elizaos
