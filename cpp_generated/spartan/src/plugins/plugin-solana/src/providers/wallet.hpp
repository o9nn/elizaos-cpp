#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-SOLANA_SRC_PROVIDERS_WALLET_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-SOLANA_SRC_PROVIDERS_WALLET_H
#include "core.h"
#include "@elizaos/core.h"
#include "bignumber.js.h"
using BigNumber = _default;
#include "../constants.h"
#include "../types.h"

class ProviderResult;

class ProviderResult : public object, public std::enable_shared_from_this<ProviderResult> {
public:
    using std::enable_shared_from_this<ProviderResult>::shared_from_this;
    any data;

    Record<string, string> values;

    string text;
};

extern std::shared_ptr<Provider> walletProvider;
#endif
