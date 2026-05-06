#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_SOLANA_SRC_PROVIDERS_WALLET_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_SOLANA_SRC_PROVIDERS_WALLET_H
#include "core.hpp"
// External dependency removed
#include "bignumber.js.h"
// Using alias removed (invalid transpilation)
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
