#include "getTokenPriceChart.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatMarketCap(double value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (value >= 1000000000) return `${(value / 1000000000).toFixed(2)}B`;
    if (value >= 1000000) return `${(value / 1000000).toFixed(2)}M`;
    if (value >= 1000) return `${(value / 1000).toFixed(2)}K`;
    return value.toFixed(2);

}

} // namespace elizaos
