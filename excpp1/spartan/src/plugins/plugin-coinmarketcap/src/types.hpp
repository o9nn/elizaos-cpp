#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct PriceData {
    double price;
    double marketCap;
    double volume24h;
    double percentChange24h;
};

struct ApiResponse {
    double price;
    double market_cap;
    double volume_24h;
    double percent_change_24h;
};


} // namespace elizaos
