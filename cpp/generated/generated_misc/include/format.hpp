#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-CLANKER_SRC_UTILS_FORMAT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-CLANKER_SRC_UTILS_FORMAT_H
#include "core.h"
#include "../types.h"

std::string formatUsd(double amount);

std::string formatPercentage(double value);

std::string shortenAddress(std::string address);

std::string formatTransactionHash(std::string hash);

double calculatePriceImpact(std::any inputAmount, std::any outputAmount, double inputPrice, double outputPrice);

std::string formatTokenInfo(std::any info);

std::string formatGasPrice(std::any gasPrice);

std::string formatCompactUsd(double amount);

#endif
