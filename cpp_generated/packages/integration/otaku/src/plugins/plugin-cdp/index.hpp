#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-CDP_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-CDP_INDEX_H
#include "core.h"
#include "@elizaos/core.h"
#include "./services/cdp.service.h"
#include "./actions/cdp-wallet-info.h"
#include "./actions/cdp-wallet-check-balance.h"
#include "./actions/cdp-wallet-swap.h"
#include "./actions/cdp-wallet-token-transfer.h"
#include "./actions/cdp-wallet-nft-transfer.h"
#include "./actions/cdp-resolve-ens.h"
#include "./actions/cdp-tx-explorer-link.h"
#include "./actions/cdp-check-tx-confirmation.h"
#include "./providers/walletState.h"

extern std::shared_ptr<Plugin> cdpPlugin;
#endif
