#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PLUGINS_PLUGIN_CDP_CONSTANTS_TIMEOUTS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PLUGINS_PLUGIN_CDP_CONSTANTS_TIMEOUTS_H
#include "core.hpp"
#include "viem.hpp"
// External dependency removed

extern double TX_CONFIRMATION_TIMEOUT;
extern double BRIDGE_CONFIRMATION_TIMEOUT;
extern double BRIDGE_POLL_INTERVAL;
extern double BRIDGE_MAX_POLL_ATTEMPTS;
void waitForTxConfirmation(std::shared_ptr<PublicClient> publicClient, std::shared_ptr<> hash);

#endif
