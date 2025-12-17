#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".ui/chart.hpp"

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

using ChartType = std::variant<std::string, std::string>;

struct PriceChartData {
    std::string token_identifier;
    std::string token_symbol;
    std::string chain;
    std::string timeframe;
    std::optional<double> current_price;
    std::optional<std::optional<double>> current_market_cap;
    std::vector<PriceDataPoint> data_points;
    std::optional<std::vector<MarketCapDataPoint>> market_cap_data_points;
    double data_points_count;
    std::optional<double> market_cap_data_points_count;
    double value;
    double percentage;
    double value;
    double percentage;
};

struct ChatPriceChartProps {
    PriceChartData data;
};

void ChatPriceChart();


} // namespace elizaos
