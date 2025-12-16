#include "...constants/chains.hpp"
#include "...contexts/ModalContext.hpp"
#include "...lib/number-format.hpp"
#include "..ui/button.hpp"
#include "..ui/chart.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct Token {
    std::string symbol;
    std::string name;
    std::string balance;
    std::string balanceFormatted;
    number | null usdValue;
    number | null usdPrice;
    string | null contractAddress;
    std::string chain;
    double decimals;
    std::optional<std::string> icon;
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
    std::string date;
};

struct MarketCapDataPoint {
    double timestamp;
    double marketCap;
    std::string date;
};

void TokenDetailModalContent(TokenDetailModalContentProps { token });

} // namespace elizaos
