#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-RELAY_SRC_UTILS_MULTICHAIN-WALLET_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-RELAY_SRC_UTILS_MULTICHAIN-WALLET_H
#include "core.h"
#include "viem.h"
#include "viem/chains.h"
#include "viem/accounts.h"

class MultiChainWallet;

extern Record<double, std::shared_ptr<Chain>> CHAIN_MAP;
extern Record<double, string> DEFAULT_RPC_URLS;
class MultiChainWallet : public object, public std::enable_shared_from_this<MultiChainWallet> {
public:
    using std::enable_shared_from_this<MultiChainWallet>::shared_from_this;
    std::shared_ptr<Account> account;

    double currentChainId;

    std::shared_ptr<Map<double, std::shared_ptr<WalletClient>>> walletClients = std::make_shared<Map>();

    string defaultRpcUrl;

    MultiChainWallet(std::shared_ptr<Account> account, string defaultRpcUrl = undefined, double initialChainId = base->id);
    virtual std::shared_ptr<WalletClient> getWalletClient(double chainId);
    virtual std::shared_ptr<Promise<double>> getChainId();
    virtual std::shared_ptr<Promise<void>> switchChain(double chainId);
    virtual std::shared_ptr<Promise<string>> address();
    virtual std::shared_ptr<WalletClient> getCurrentWalletClient();
    virtual std::shared_ptr<Account> getAccount();
};

std::shared_ptr<MultiChainWallet> createMultiChainWallet(std::shared_ptr<Account> account, string defaultRpcUrl = undefined);

#endif
