#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_CLIENT_SRC_UTILS_SWAPUTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_CLIENT_SRC_UTILS_SWAPUTILS_H
#include "core.h"
#include "@/hooks/use-config-account.h"
#include "@/types.h"
#include "@autodotfun/types/types/autofun.ts.h"
#include "@coral-xyz/anchor.h"
#include "@solana/web3.js.h"
#include "./env.h"

double convertToBasisPoints(double feePercent);

double calculateAmountOutSell(double reserveLamport, double amount, double _tokenDecimals, double platformSellFee, double reserveToken);

extern std::function<std::shared_ptr<Promise<std::shared_ptr<...>>>(any, double, double, double, string, string, string, double, double, any, any, any, object)> launchAndSwapTx;
double calculateAmountOutBuy(double reserveToken, double amount, double _solDecimals, double reserveLamport, double platformBuyFee);

extern double FEE_BASIS_POINTS;
extern std::function<std::shared_ptr<Promise<object>>(any, double, double, double, double)> getSwapAmount;
extern std::function<std::shared_ptr<Promise<object>>(string, double, double, double)> getSwapAmountJupiter;
extern std::function<std::shared_ptr<Promise<any>>(any, any, double, double, double, any, double, double, any)> swapIx;
extern std::function<std::shared_ptr<Promise<any>>(any, any, double, double, double, any, boolean)> getJupiterSwapIx;
#endif
