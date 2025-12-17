#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "constants/chains.hpp"
#include "contexts/ModalContext.hpp"
#include "lib/number-format.hpp"
#include "ui/button.hpp"
#include "ui/chart.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct Token {
    std::string symbol;
    std::string name;
    std::string balance;
    std::string balanceFormatted;
    std::optional<double> usdValue;
    std::optional<double> usdPrice;
    std::string contractAddress;
    std::string chain;
    double decimals;
    std::optional<std::string> icon;
};

using ChainNetwork = std::string;

struct TokenDetailModalContentProps {
    Token token;
};

using TimeFrame = std::string;
using ChartType = std::string;

struct PriceDataPoint {
    double timestamp;
    double price;
    std::string date;
};

struct MarketCapDataPoint {
    double timestamp;
    double marketCap;
    std::string date;
};

void TokenDetailModalContent();

} // namespace elizaos
