#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_RAYDIUM_SRC_RAYDIUM-CONFIG_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_RAYDIUM_SRC_RAYDIUM-CONFIG_H
#include "core.h"
#include "@raydium-io/raydium-sdk-v2.h"
#include "@solana/spl-token.h"
#include "@solana/web3.js.h"

typedef any Cluster;


extern std::function<string()> getRpcUrl;
extern any txVersion;
extern any raydium;
extern std::function<std::shared_ptr<Promise<any>>(object)> initSdk;
extern std::function<std::shared_ptr<Promise<any>>()> fetchTokenAccountData;
#endif
