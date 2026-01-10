#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_CLIENT_SRC_HOOKS_USE-CONFIG-ACCOUNT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_CLIENT_SRC_HOOKS_USE-CONFIG-ACCOUNT_H
#include "core.h"
#include "@/utils/api.h"
#include "@/utils/program.h"
#include "@coral-xyz/anchor.h"
#include "@solana/web3.js.h"

extern array<string> configAccountQueryKey;
extern std::function<std::shared_ptr<Promise<any>>(any)> getConfigAccount;
#endif
