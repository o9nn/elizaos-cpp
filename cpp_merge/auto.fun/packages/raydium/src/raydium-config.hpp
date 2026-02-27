#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_RAYDIUM_SRC_RAYDIUM_CONFIG_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_RAYDIUM_SRC_RAYDIUM_CONFIG_H
#include "core.hpp"
// External dependency removed
// External dependency removed
// External dependency removed

typedef any Cluster;


extern std::function<string()> getRpcUrl;
extern any txVersion;
extern any raydium;
extern std::function<std::shared_ptr<Promise<any>>(object)> initSdk;
extern std::function<std::shared_ptr<Promise<any>>()> fetchTokenAccountData;
#endif
