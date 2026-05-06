#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_PROGRAM_TESTS_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_PROGRAM_TESTS_UTILS_H
#include "core.h"
#include "@coral-xyz/anchor.h"
#include "@solana/spl-token.h"
#include "@solana/web3.js.h"

extern any provider;
extern any connection;
extern double fixedPoint;
extern string vaultConfigSeed;
extern string positionSeed;
extern string claimerInfoSeed;
extern string nftFaucetSeed;
extern any token0;
extern any token1;
extern any dev_nft_address;
extern any clmm_nft_address;
extern any cpmm_nft_address_0;
extern any cpmm_nft_address_1;
extern any claimer_address_0;
extern std::function<any(boolean)> getNftAddress;
extern string devnetEndpoint;
extern std::function<std::shared_ptr<Promise<void>>(any, any, any)> sendSolTo;
extern std::function<std::shared_ptr<Promise<void>>(any, any, any, any)> sendTokenTo;
extern std::function<boolean(any)> isDevnet;
extern object utils;
#endif
