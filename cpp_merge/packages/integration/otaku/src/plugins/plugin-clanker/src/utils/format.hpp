#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PLUGINS_PLUGIN_CLANKER_SRC_UTILS_FORMAT_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PLUGINS_PLUGIN_CLANKER_SRC_UTILS_FORMAT_H
#include "core.hpp"
#include "../types.h"

string formatUsd(double amount);

string formatPercentage(double value);

string shortenAddress(string address);

string formatTransactionHash(string hash);

double calculatePriceImpact(any inputAmount, any outputAmount, double inputPrice, double outputPrice);

string formatTokenInfo(any info);

string formatGasPrice(any gasPrice);

string formatCompactUsd(double amount);

#endif
