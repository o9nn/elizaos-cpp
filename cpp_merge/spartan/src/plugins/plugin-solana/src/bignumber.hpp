#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_SOLANA_SRC_BIGNUMBER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_SOLANA_SRC_BIGNUMBER_H
#include "core.hpp"
#include "bignumber.js.h"
// Using alias removed (invalid transpilation)

extern any BN;
template <typename P0>
std::shared_ptr<BigNumber> toBN(P0 value);

template <typename P0>
std::shared_ptr<BigNumber> toBN(P0 value)
{
    return std::make_shared<BigNumber>(value);
};


#endif
