#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_UTILS_BIGNUMBER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_UTILS_BIGNUMBER_H
#include "core.hpp"
#include <string>
#include "bignumber.js.h"
// Using alias removed (invalid transpilation)

template <typename P0>
std::shared_ptr<BigNumber> toBN(P0 value);

string formatBN(std::shared_ptr<BigNumber> value, double decimals = 18);

template <typename P0>
std::shared_ptr<BigNumber> toBN(P0 value)
{
    try
    {
        return std::make_shared<BigNumber>(value);
    }
    catch (const any& error)
    {
        throw any(std::make_shared<Error>(std::string("Failed to convert value to BigNumber: ") + value + string_empty));
    }
};


#endif
