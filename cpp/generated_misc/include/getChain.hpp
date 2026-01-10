#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_LIB_GETCHAIN_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_LIB_GETCHAIN_H
#include "core.h"
#include "viem/chains.h"
#include "@/config/contracts.h"

extern std::shared_ptr<Chain> anvil;
std::shared_ptr<Chain> getChain();

string getRpcUrl();

string getRpcUrlForChain(string chainType);

#endif
