#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_CONSTANTS_CHAINS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_CONSTANTS_CHAINS_H
#include "core.h"
#include "viem/chains.h"
#include "viem/chains.h"

typedef any SupportedNetwork;

class ChainConfig;

class ChainConfig : public object, public std::enable_shared_from_this<ChainConfig> {
public:
    using std::enable_shared_from_this<ChainConfig>::shared_from_this;
    string name;

    std::shared_ptr<Chain> chain;

    std::function<string(string)> rpcUrl;

    string explorerUrl;

    object nativeToken;

    string coingeckoPlatform;

    object swap;
};

extern Record<SupportedNetwork, std::shared_ptr<ChainConfig>> CHAIN_CONFIGS;
extern array<SupportedNetwork> MAINNET_NETWORKS;
extern array<SupportedNetwork> TESTNET_NETWORKS;
extern array<SupportedNetwork> ALL_NETWORKS;
any getChainConfig(string network);

any getViemChain(string network);

any getRpcUrl(string network, string alchemyKey);

any getExplorerUrl(string network);

any getTxExplorerUrl(string network, string txHash);

any getAddressExplorerUrl(string network, string address);

any getNativeTokenInfo(string network);

any getCoingeckoPlatform(string network);

any isSupportedNetwork(string network);

boolean isMainnet(string network);

boolean isTestnet(string network);

boolean isCdpSwapSupported(string network);

array<SupportedNetwork> getCdpSwapSupportedNetworks();

extern string NATIVE_TOKEN_ADDRESS;
string normalizeTokenAddress(string token);

extern Record<string, string> UNISWAP_V3_ROUTER;
extern Record<string, string> UNISWAP_V3_QUOTER;
extern Record<string, string> WRAPPED_NATIVE_TOKEN;
extern object UNISWAP_POOL_FEES;
#endif
