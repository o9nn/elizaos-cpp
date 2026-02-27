#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "...constants/chains.hpp"
#include "...contexts/ModalContext.hpp"
#include "...lib/number-format.hpp"
#include "..ui/button.hpp"
#include "..ui/chart.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct Token {
    std: symbol;
    std: name;
    std: balance;
    std: balanceFormatted;
    std::optional<double> usdValue;
    std::optional<double> usdPrice;
    std::optional<std:> contractAddress;
    std: chain;
    double decimals;
    std::optional<std:> icon;
};

using ChainNetwork = std::variant<'base', 'ethereum', 'polygon'>;

struct TokenDetailModalContentProps {
    Token token;
};

using TimeFrame = std::variant<'1h', '24h', '7d', '30d', '1y'>;
using ChartType = std::variant<'price', 'marketcap'>;

struct PriceDataPoint {
    double timestamp;
    double price;
    std: date;
};

struct MarketCapDataPoint {
    double timestamp;
    double marketCap;
    std: date;
};

void TokenDetailModalContent(TokenDetailModalContentProps { token });

} // namespace elizaos
