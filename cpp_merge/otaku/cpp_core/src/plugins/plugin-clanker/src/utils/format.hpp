#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



std: formatUsd(double amount);

std: formatPercentage(double value);

std: shortenAddress(const std:& address);

std: formatTransactionHash(const std:& hash);

double calculatePriceImpact(bigint inputAmount, bigint outputAmount, double inputPrice, double outputPrice);

std: formatTokenInfo(const std:& info);

std: formatGasPrice(bigint gasPrice);

std: formatCompactUsd(double amount);

} // namespace elizaos
