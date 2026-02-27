#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_SERVER_SRC_MCAP_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_SERVER_SRC_MCAP_H
#include "core.hpp"
// External dependency removed
// External dependency removed
#include "./cache.h"
#include "./raydium.h"
#include "./tokenSupplyHelpers.h"
#include "./util.h"

extern std::shared_ptr<PythCluster> PYTHNET_CLUSTER_NAME;
extern string SOLUSD_SYMBOL;
extern double MAX_CONCURRENT_TOKENS;
extern double totalUpdatesProcessed;
extern double failedUpdates;
extern any lastUpdateTime;
std::shared_ptr<Promise<double>> getSOLPrice();

std::shared_ptr<Promise<double>> fetchSOLPriceFromPyth();

std::shared_ptr<Promise<any>> calculateTokenMarketData(any token, double solPrice);

any calculateRaydiumTokenMarketData(any token);

any getMarketDataMetrics();

#endif
