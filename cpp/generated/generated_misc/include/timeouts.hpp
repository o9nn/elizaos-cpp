#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-CDP_CONSTANTS_TIMEOUTS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-CDP_CONSTANTS_TIMEOUTS_H
#include "core.h"
#include "viem.h"
#include "@elizaos/core.h"

extern double TX_CONFIRMATION_TIMEOUT;
extern double BRIDGE_CONFIRMATION_TIMEOUT;
extern double BRIDGE_POLL_INTERVAL;
extern double BRIDGE_MAX_POLL_ATTEMPTS;
void waitForTxConfirmation(std::shared_ptr<PublicClient> publicClient, std::shared_ptr<> hash);

#endif
