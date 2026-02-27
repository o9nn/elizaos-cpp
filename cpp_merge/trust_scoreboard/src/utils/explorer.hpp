#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_TRUST_SCOREBOARD_SRC_UTILS_EXPLORER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_TRUST_SCOREBOARD_SRC_UTILS_EXPLORER_H
#include "core.hpp"
#include <string>
// External dependency removed
#include "bs58.hpp"
// Using alias removed (invalid transpilation)

template <typename P1>
any getExplorerUrl(string endpoint, P1 viewTypeOrItemAddress, string itemType = std::string("address"));

template <typename P1>
any getExplorerUrl(string endpoint, P1 viewTypeOrItemAddress, string itemType)
{
    auto getClusterUrlParam = [=]() mutable
    {
        auto cluster = string_empty;
        if (endpoint == std::string("localnet")) {
            cluster = std::string("custom&customUrl=") + encodeURIComponent(std::string("http://127.0.0.1:8899")) + string_empty;
        } else if (endpoint == std::string("https://api.devnet.solana.com")) {
            cluster = std::string("devnet");
        }
        return (cluster) ? any(std::string("?cluster=") + cluster + string_empty) (string_empty);
    };
    return std::string("https://explorer.solana.com/") + itemType + std::string("/") + viewTypeOrItemAddress + string_empty + getClusterUrlParam() + string_empty;
};


#endif
