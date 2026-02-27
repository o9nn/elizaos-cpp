#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_FRONTEND_CONSTANTS_CHAINS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_FRONTEND_CONSTANTS_CHAINS_H
#include "core.hpp"

typedef any SupportedChain;

class ChainUIConfig;

class ChainUIConfig : public object, public std::enable_shared_from_this<ChainUIConfig> {
public:
    using std::enable_shared_from_this<ChainUIConfig>::shared_from_this;
    SupportedChain id;

    string name;

    string displayName;

    string icon;

    string walletIcon;

    object nativeToken;

    string explorerUrl;

    string color;
};

extern Record<SupportedChain, std::shared_ptr<ChainUIConfig>> CHAIN_UI_CONFIGS;
extern array<SupportedChain> SUPPORTED_CHAINS;
any getChainConfig(string chain);

any getChainIcon(string chain);

any getChainWalletIcon(string chain);

any getNativeTokenIcon(string chain);

any getNativeTokenSymbol(string chain);

string getChainColor(string chain);

any getTxExplorerUrl(string chain, string txHash);

any getAddressExplorerUrl(string chain, string address);

any isSupportedChain(string chain);

string getChainDisplayName(string chain);

extern Record<string, string> TOKEN_ICONS;
any getTokenIconBySymbol(string symbol);

any getTokenIcon(string symbol, string chain = undefined);

#endif
