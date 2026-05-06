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
    std::optional<std::string> contractAddress;
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
    std::variant<'sent', 'received'> direction;
    std::optional<std::optional<std::string>> icon;
    std::optional<std::optional<std::string>> contractAddress;
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
    CdpTransactionManager getInstance();
    void initializeCdpClient();
    CdpClient getCdpClient();
    std::optional<std::string> getIconFromCache(const std::optional<std::string>& contractAddress);
    void setIconInCache(const std::optional<std::string>& contractAddress, const std::optional<std::string>& icon);
    std::variant<Promise<string, null>> getOrFetchIcon(const std::string& contractAddress, const std::string& chain);
    Promise< getOrCreateWallet(const std::string& userId);
    Promise< getViemClientsForAccount(std::optional<std::any> options);
    void if(auto !chainConfig);
    void if(auto !alchemyKey);
    Promise< getTokenBalances(const std::string& userId, std::optional<std::string> chain, boolean = false forceSync, std::optional<std::string> address);
    void if(auto !forceSync);
    std::future<bigint> getOnChainTokenBalance(std::optional<std::any> params);
    std::optional<std::string> getAlchemyNetworkName(const std::string& chain);
    Promise< fetchWalletTokens(CdpClient client, const std::string& name, std::optional<std::string> chain, std::optional<std::string> providedAddress);
    void if(auto !alchemyKey);
    void if(auto chain);
    std::function<void()> filter(auto (net);
    void if(auto !response.ok);
    void for(auto const token of tokens);
    void for(auto const chainName of networksToFetch);
    void catch(auto err);
    void if(auto !chain);
    Promise< getNFTs(const std::string& userId, std::optional<std::string> chain, boolean = false forceSync, std::optional<std::string> address);
    void if(auto !forceSync);
    Promise< fetchWalletNFTs(CdpClient client, const std::string& name, std::optional<std::string> chain, std::optional<std::string> providedAddress);
    void if(auto !alchemyKey);
    void if(auto chain);
    void for(auto const network of networks);
    std::future<double> getTransactionTimestamp(const std::any& tx, const std::string& rpcUrl);
    Promise< getTransactionHistory(const std::string& userId, std::optional<std::string> providedAddress);
    void if(auto !alchemyKey);
    void for(auto const network of networks);
    std::future<SendResult> sendToken(const std::any& params);
    std::future<SendNFTResult> sendNFT(const std::any& params);
    Promise<SwapPriceResult & getSwapPrice(const std::any& params);
    void catch(auto cdpError);
    void if(auto zeroXQuote);
    void if(auto !quoterAddress);
    void if(auto !chain);
    void if(auto !alchemyKey);
    void if(auto !rpcUrl);
    void if(auto !wrappedNativeAddress);
    void catch(auto quoteError);
    std::future<SwapResult> swap(const std::any& params);
    std::future<std::any> waitForTransactionConfirmation(const std::string& transactionHash, const std::string& network, string = 'Transaction' operationName);
    Promise< executeSwapWith0x(const std::any& account, const std::string& network, const std::string& fromToken, const std::string& toToken, bigint fromAmount, double slippageBps);
    void if(auto !chain);
    void if(auto !alchemyKey);
    void if(auto !rpcUrl);
    std::future<void> ensureTokenApproval(const std::any& walletClient, const std::any& publicClient, const std::string& tokenAddress, const std::string& spenderAddress, bigint amount, const std::string& ownerAddress);
    std::future<std::string> wrapNativeToken(const std::any& walletClient, const std::any& publicClient, const std::string& wrappedTokenAddress, bigint amount);
    Promise< unwrapNativeToken(const std::any& walletClient, const std::any& publicClient, const std::string& wrappedTokenAddress, const std::string& ownerAddress);
    Promise< getUniswapQuote(const std::any& publicClient, const std::string& quoterAddress, const std::string& tokenIn, const std::string& tokenOut, bigint amountIn);
    $ info(auto `[CDP API] Uniswap quote (MEDIUM fee 0.3%);
    void catch(auto mediumError);
    $ info(auto `[CDP API] Uniswap quote (LOW fee 0.05%);
    void catch(auto lowError);
    $ info(auto `[CDP API] Uniswap quote (HIGH fee 1%);
    void catch(auto highError);
    Promise< executeUniswapSwap(const std::any& walletClient, const std::any& publicClient, const std::any& account, const std::string& network, const std::string& fromToken, const std::string& toToken, bigint fromAmount, double slippageBps);
    void if(auto !routerAddress);
    void if(auto !quoterAddress);
    void if(auto !wrappedNativeAddress);
    void if(auto isFromNative);
    void if(auto isToNative);
    Promise< get0xQuote(const std::string& network, const std::string& fromToken, const std::string& toToken, bigint fromAmount, const std::string& takerAddress);
    void if(auto !apiKey);
    void if(auto !chainId);
    void if(auto !response.ok);
    void if(auto !data.buyAmount);
    void catch(auto error);
    Promise< execute0xSwap(const std::any& walletClient, const std::any& publicClient, const std::any& account, const std::string& network, const std::string& fromToken, const std::string& toToken, bigint fromAmount, double slippageBps);
    void if(auto !apiKey);
    void if(auto !chainId);
    void if(auto !response.ok);
    void if(auto !quote.transaction || !quote.transaction.to || !quote.transaction.data || !quote.buyAmount);
    void if(auto quote.expirationTimeSeconds);
    void if(auto quote.expirationTimeSeconds);
    0n);
    
    const txParams: any = BigInt(auto tx.value);
    void if(auto tx.gas);
    Promise< searchTokens(std::optional<std::any> params);
    void if(auto !query || query.length < 2);
    void if(auto isAddress);
    void if(auto !response.ok);
    void for(auto const coin of topCoins);
    void catch(auto error);
    Promise< getTokenInfo(const std::string& contractAddress, const std::string& platform);
    void if(auto !apiKey);
    void if(auto response.ok);
    void catch(auto err);
    Promise< getTokenInfoFromDexScreener(const std::string& address, const std::string& chainId);
    void if(auto !response.ok);
    void if(auto !pair);
    void catch(auto err);
    std::future<double> getNativeTokenPrice(const std::string& coingeckoId);
    double safeBalanceToNumber(const std::string& balanceHex, double decimals);
    Promise< getTopAndTrendingTokens(std::optional<std::any> params);
    void if(auto !chain);
    void if(auto categoryId);
    void if(auto trendingResponse.ok);
    void catch(auto error);
};


} // namespace elizaos
