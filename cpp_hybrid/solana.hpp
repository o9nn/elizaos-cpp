#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_SOLANA_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_SOLANA_H
#include "core.h"
#include "@autodotfun/types/idl/autofun.json.h"
#include "@coral-xyz/anchor.h"
#include "@metaplex-foundation/umi-bundle-defaults.h"
#include "@solana/web3.js.h"
#include "@autodotfun/types/types/autofun.h"
#include "./util.h"

extern std::shared_ptr<Autofun> idl;
any initSolanaConfig();

extern std::function<any(any, any)> getProgram;
#endif
