#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Types

struct TokenBalance {
    std::string symbol;
    std::string name;
    std::string balance;
    std::string balanceFormatted;
    double usdValue;
    double usdPrice;
    std::string contractAddress;
    std::string chain;
    double decimals;
    std::optional<std::string> icon;
};

struct NFT {
    std::string chain;
    std::string contractAddress;
    std::string tokenId;
    std::string name;
    std::string description;
    std::string image;
    std::string contractName;
    std::string tokenType;
    std::optional<std::string> balance;
    std::vector<std::any> attributes;
};

struct Transaction {
    std::string chain;
    std::string hash;
    std::string from;
    std::string to;
    std::string value;
    std::string asset;
    std::string category;
    double timestamp;
    std::string blockNum;
    std::string explorerUrl;
    std::string direction;
    std::optional<std::string> icon;
    std::optional<std::string> contractAddress;
};

struct SwapPriceResult {
    bool liquidityAvailable;
    std::string toAmount;
    std::string minToAmount;
};

struct SwapResult {
    std::string transactionHash;
    std::string from;
    std::string fromToken;
    std::string toToken;
    std::string fromAmount;
    std::string toAmount;
    std::string network;
    std::string method;
};

struct SendResult {
    std::string transactionHash;
    std::string from;
    std::string to;
    std::string amount;
    std::string token;
    std::string network;
    std::string method;
};

struct SendNFTResult {
    std::string transactionHash;
    std::string from;
    std::string to;
    std::string contractAddress;
    std::string tokenId;
    std::string network;
};

// CDP Transaction Manager Class (Singleton)

class CdpTransactionManager {
public:
    CdpTransactionManager();
    static CdpTransactionManager getInstance();
    void initializeCdpClient();
    CdpClient getCdpClient();
    std::string getIconFromCache(const std::string& contractAddress);
    void setIconInCache(const std::string& contractAddress, const std::string& icon);
    std::variant<Promise<string, null>> getOrFetchIcon(const std::string& contractAddress, const std::string& chain);
    Promise< getOrCreateWallet(const std::string& userId);
    Promise< getViemClientsForAccount(std::optional<std::any> options);
    Promise< getTokenBalances(const std::string& userId, std::optional<std::string> chain, bool forceSync = false, std::optional<std::string> address);
    std::future<bigint> getOnChainTokenBalance(std::optional<std::any> params);
    std::string getAlchemyNetworkName(const std::string& chain);
    Promise< fetchWalletTokens(CdpClient client, const std::string& name, std::optional<std::string> chain, std::optional<std::string> providedAddress);
    std::function<void()> filter();
    $ warn();
    Promise< getNFTs(const std::string& userId, std::optional<std::string> chain, bool forceSync = false, std::optional<std::string> address);
    Promise< fetchWalletNFTs(CdpClient client, const std::string& name, std::optional<std::string> chain, std::optional<std::string> providedAddress);
    std::future<double> getTransactionTimestamp(const std::any& tx, const std::string& rpcUrl);
    Promise< getTransactionHistory(const std::string& userId, std::optional<std::string> providedAddress);
    std::future<SendResult> sendToken(const std::any& params);
    std::future<SendNFTResult> sendNFT(const std::any& params);
    Promise<SwapPriceResult & getSwapPrice(const std::any& params);
    std::future<SwapResult> swap(const std::any& params);
    Promise< executeSwapWith0x(const std::any& account, const std::string& network, const std::string& fromToken, const std::string& toToken, bigint fromAmount, double slippageBps);
    std::future<void> ensureTokenApproval(const std::any& walletClient, const std::any& publicClient, const std::string& tokenAddress, const std::string& spenderAddress, bigint amount, const std::string& ownerAddress);
    std::future<std::string> wrapNativeToken(const std::any& walletClient, const std::any& publicClient, const std::string& wrappedTokenAddress, bigint amount);
    Promise< unwrapNativeToken(const std::any& walletClient, const std::any& publicClient, const std::string& wrappedTokenAddress, const std::string& ownerAddress);
    Promise< getUniswapQuote(const std::any& publicClient, const std::string& quoterAddress, const std::string& tokenIn, const std::string& tokenOut, bigint amountIn);
    $ info();
    $ push();
    Promise< executeUniswapSwap(const std::any& walletClient, const std::any& publicClient, const std::any& account, const std::string& network, const std::string& fromToken, const std::string& toToken, bigint fromAmount, double slippageBps);
    Promise< get0xQuote(const std::string& network, const std::string& fromToken, const std::string& toToken, bigint fromAmount, const std::string& takerAddress);
    Promise< execute0xSwap(const std::any& walletClient, const std::any& publicClient, const std::any& account, const std::string& network, const std::string& fromToken, const std::string& toToken, bigint fromAmount, double slippageBps);
    $ Error();
    Promise< searchTokens(std::optional<std::any> params);
    Promise< getTokenInfo(const std::string& contractAddress, const std::string& platform);
    Promise< getTokenInfoFromDexScreener(const std::string& address, const std::string& chainId);
    std::future<double> getNativeTokenPrice(const std::string& coingeckoId);
    double safeBalanceToNumber(const std::string& balanceHex, double decimals);
    Promise< getTopAndTrendingTokens(std::optional<std::any> params);
};


} // namespace elizaos
