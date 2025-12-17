#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "background/token-metadata.hpp"
#include "database.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct TokenAccount {
    std::string mint;
    double amount;
    double decimals;
    std::optional<std::string> symbol;
    std::optional<std::string> name;
    std::optional<double> usdValue;
};

struct WalletData {
    std::string address;
    double solBalance;
    std::vector<TokenAccount> tokens;
    double totalUsdValue;
    std::string lastUpdated;
};

struct PriceData {
    double price;
    std::string symbol;
    std::string name;
    std::optional<double> change24h;
    std::optional<double> marketCap;
};

class SolanaService {
public:
    SolanaService();
    std::future<void> processQueue();
    std::future<double> getSOLBalance(const std::string& address);
    std::future<std::vector<TokenAccount>> getTokenAccounts(const std::string& address);
    Promise< getTokenMetadata(const std::vector<std::string>& mints);
    std::future<double> getSOLPrice();
    std::future<PriceData> getTokenPrices(const std::vector<std::string>& mints);
    std::future<WalletData> getWalletData(const std::string& address, const std::string& walletId);
    Promise< processWalletBatch(const std::vector<std::any>& wallets, double batchSize = 5);
    std::future<void> refreshStaleTokenPrices();
    std::vector<TokenPrice> getCachedTokenPrices();
    Promise< healthCheck();
    bool isValidSolanaAddress(const std::string& address);

private:
    Connection connection_;
    Connection fallbackConnection_;
    std::string rpcUrl_;
    std::string jupiterApiUrl_;
    std::string jupiterTokensApiUrl_;
};

// Export singleton instance

} // namespace elizaos
