#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTOFUNTRADER_UTILS_WALLET_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTOFUNTRADER_UTILS_WALLET_H
#include "core.hpp"
// External dependency removed
// External dependency removed
#include "./utils.h"

class TokenBalance;

std::shared_ptr<Keypair> getWalletKeypair(std::shared_ptr<IAgentRuntime> runtime = std::nullopt);

std::shared_ptr<Promise<double>> getWalletBalance(std::shared_ptr<IAgentRuntime> runtime);

std::shared_ptr<Promise<std::shared_ptr<Connection>>> getConnection(std::shared_ptr<IAgentRuntime> runtime);

extern object CONFIRMATION_CONFIG;
double calculateDynamicSlippage(string amount, any quoteData);

std::shared_ptr<Promise<object>> executeTrade(std::shared_ptr<IAgentRuntime> runtime, object params);

std::shared_ptr<Promise<object>> executeRaydiumTrade(std::shared_ptr<IAgentRuntime> runtime, object params);

std::shared_ptr<Promise<double>> getChainWalletBalance(std::shared_ptr<IAgentRuntime> runtime, string _tokenAddress);

std::shared_ptr<Promise<string>> simulateTransaction(any client, any tx);

class TokenBalance : public object, public std::enable_shared_from_this<TokenBalance> {
public:
    using std::enable_shared_from_this<TokenBalance>::shared_from_this;
    string mint;

    double balance;

    double decimals;

    double uiAmount;
};

any getWalletBalances(std::shared_ptr<IAgentRuntime> runtime);

std::shared_ptr<Promise<any>> getTokenBalance(std::shared_ptr<IAgentRuntime> runtime, string tokenMint);

std::shared_ptr<Promise<boolean>> hasTokenBalance(std::shared_ptr<IAgentRuntime> runtime, string tokenMint);

#endif
