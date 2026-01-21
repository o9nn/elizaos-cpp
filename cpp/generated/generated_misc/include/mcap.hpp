#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_MCAP_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_MCAP_H
#include "core.h"
#include "@pythnetwork/client.h"
#include "@solana/web3.js.h"
#include "./cache.h"
#include "./raydium.h"
#include "./tokenSupplyHelpers.h"
#include "./util.h"

extern std::shared_ptr<PythCluster> PYTHNET_CLUSTER_NAME;
extern std::string SOLUSD_SYMBOL;
extern double MAX_CONCURRENT_TOKENS;
extern double totalUpdatesProcessed;
extern double failedUpdates;
extern std::any lastUpdateTime;
std::shared_ptr<Promise<double>> getSOLPrice();

std::shared_ptr<Promise<double>> fetchSOLPriceFromPyth();

std::shared_ptr<Promise<any>> calculateTokenMarketData(std::any token, double solPrice);

std::any calculateRaydiumTokenMarketData(std::any token);

std::any getMarketDataMetrics();

#endif
