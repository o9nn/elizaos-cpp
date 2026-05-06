#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SRC_LIB_GETCHAIN_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SRC_LIB_GETCHAIN_H
#include "core.hpp"
#include "viem/chains.h"
// External dependency removed

extern std::shared_ptr<Chain> anvil;
std::shared_ptr<Chain> getChain();

string getRpcUrl();

string getRpcUrlForChain(string chainType);

#endif
