#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-SOLANA_SRC_BIGNUMBER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-SOLANA_SRC_BIGNUMBER_H
#include "core.h"
#include "bignumber.js.h"
using BigNumber = _default;

extern any BN;
template <typename P0>
std::shared_ptr<BigNumber> toBN(P0 value);

template <typename P0>
std::shared_ptr<BigNumber> toBN(P0 value)
{
    return std::make_shared<BigNumber>(value);
};


#endif
