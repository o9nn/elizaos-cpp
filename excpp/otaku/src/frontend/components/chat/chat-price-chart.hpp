#include ".ui/chart.hpp"
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

using ChartType = std::variant<'price', 'marketcap'>;

struct PriceChartData {
    std::string token_identifier;
    string | null token_symbol;
    std::string chain;
    std::string timeframe;
    number | null current_price;
    std::optional<number | null> current_market_cap;
    std::vector<PriceDataPoint> data_points;
    std::optional<std::vector<MarketCapDataPoint>> market_cap_data_points;
    double data_points_count;
    std::optional<double> market_cap_data_points_count;
    std::optional<{> price_change;
    double value;
    double percentage;
    std::optional<{> market_cap_change;
    double value;
    double percentage;
};

struct ChatPriceChartProps {
    PriceChartData data;
};

void ChatPriceChart(ChatPriceChartProps { data });


} // namespace elizaos
