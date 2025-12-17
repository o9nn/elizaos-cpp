#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"
#include "elizaos/core.hpp"

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
    std::optional<std::string> contractAddress;
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
    std::optional<std::vector<unknown>> attributes;
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
    std::future<CdpService> start(IAgentRuntime runtime);
    std::future<void> stop();
    Promise< getOrCreateWallet(const std::string& accountName);
    Promise< getViemClientsForAccount(std::optional<std::any> options);
    std::future<WalletInfo> getWalletInfoCached(const std::string& accountName, std::optional<std::string> chain, std::optional<std::string> address);
    std::future<WalletInfo> fetchWalletInfo(const std::string& accountName, std::optional<std::string> chain, std::optional<std::string> address);
    Promise< transfer(const std::any& params);
    * 1. CDP SDK (for supported networks) with Permit2 approval handling
   * 2. 0x API v2 (if configured)
   * 3. Uniswap V3 (direct protocol interaction)
   * 
   * Reference: https://docs.cdp.coinbase.com/trade-api/quickstart#3-execute-a-swap
   */
  async swap(params: chain(auto handled by manager);
    Promise< getSwapPrice(const std::any& params);
    Promise< transferNft(const std::any& params);
    std::future<bigint> getOnChainBalance(std::optional<std::any> params);

private:
    CdpTransactionManager transactionManager_;
};


} // namespace elizaos
