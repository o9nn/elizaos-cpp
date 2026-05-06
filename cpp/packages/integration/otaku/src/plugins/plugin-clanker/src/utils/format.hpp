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



std::string formatUsd(double amount);

std::string formatPercentage(double value);

std::string shortenAddress(const std::string& address);

std::string formatTransactionHash(const std::string& hash);

double calculatePriceImpact(bigint inputAmount, bigint outputAmount, double inputPrice, double outputPrice);

std::string formatTokenInfo(const std::any& info);

std::string formatGasPrice(bigint gasPrice);

std::string formatCompactUsd(double amount);

} // namespace elizaos
