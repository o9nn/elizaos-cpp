#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_WALLETSERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_WALLETSERVICE_H
#include "core.h"
#include "@elizaos/core.h"
#include "@solana/web3.js.h"
#include "../utils/analyzeTrade.h"
#include "bs58.h"
using bs58 = _default;

class WalletOperationResult;
class WalletService;

class WalletOperationResult : public object, public std::enable_shared_from_this<WalletOperationResult> {
public:
    using std::enable_shared_from_this<WalletOperationResult>::shared_from_this;
    boolean success;

    std::string signature;

    std::string error;

    std::string outAmount;

    std::string receivedAmount;

    std::string swapUsdValue;
};

class WalletService : public object, public std::enable_shared_from_this<WalletService> {
public:
    using std::enable_shared_from_this<WalletService>::shared_from_this;
    std::shared_ptr<IAgentRuntime> runtime;

    std::any connection = nullptr;

    std::any keypair = nullptr;

    std::any CONFIRMATION_CONFIG;

    WalletService(std::shared_ptr<IAgentRuntime> runtime_);
    virtual std::shared_ptr<Promise<void>> initialize();
    virtual std::shared_ptr<Promise<void>> stop();
    virtual std::any getWallet();
    virtual std::shared_ptr<Promise<double>> getBalance();
};

#endif
