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
#include "elizaos/core.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct WalletToken {
    std::string symbol;
    std::string name;
    std::string balance;
    std::string balanceFormatted;
    double usdValue;
    double usdPrice;
    std::string contractAddress;
    std::string chain;
    double decimals;
};

struct WalletNFT {
    std::string chain;
    std::string contractAddress;
    std::string tokenId;
    std::string name;
    std::string description;
    std::string contractName;
    std::string tokenType;
    std::optional<std::string> balance;
    std::optional<std::vector<std::any>> attributes;
};

struct WalletInfo {
    std::string address;
    std::vector<WalletToken> tokens;
    std::vector<WalletNFT> nfts;
    double totalUsdValue;
};

class CdpService {
public:
    CdpService(IAgentRuntime runtime);
    static std::future<CdpService> start(IAgentRuntime runtime);
    std::future<void> stop();
    Promise< getOrCreateWallet(const std::string& accountName);
    Promise< getViemClientsForAccount(std::optional<std::any> options);
    std::future<WalletInfo> getWalletInfoCached(const std::string& accountName, std::optional<std::string> chain, std::optional<std::string> address);
    std::future<WalletInfo> fetchWalletInfo(const std::string& accountName, std::optional<std::string> chain, std::optional<std::string> address);
    Promise< transfer(const std::any& params);
    Promise< getSwapPrice(const std::any& params);
    Promise< transferNft(const std::any& params);
    std::future<bigint> getOnChainBalance(std::optional<std::any> params);

private:
    CdpTransactionManager transactionManager_;
};


} // namespace elizaos
