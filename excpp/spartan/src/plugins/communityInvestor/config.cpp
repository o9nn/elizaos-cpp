#include "config.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

double getConvictionMultiplier(Conviction conviction) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    switch (conviction) {
        case Conviction.NONE:
        return 0;
        case Conviction.LOW:
        return 0.5;
        case Conviction.MEDIUM:
        return 1.0;
        case Conviction.HIGH:
        return 1.5;
        case Conviction.VERY_HIGH:
        return 2.0;
        default:
        return 1.0;
    }

}

double getLiquidityMultiplier(double liquidity) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (liquidity < 10000) return 0.5;
    if (liquidity < 50000) return 0.75;
    if (liquidity < 100000) return 1.0;
    if (liquidity < 500000) return 1.25;
    return 1.5;

}

double getMarketCapMultiplier(double marketCap) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (marketCap < 100000) return 1.5;
    if (marketCap < 1000000) return 1.25;
    if (marketCap < 10000000) return 1.0;
    if (marketCap < 50000000) return 0.75;
    return 0.5;

}

double getVolumeMultiplier(double volume) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (volume < 10000) return 0.5;
    if (volume < 50000) return 0.75;
    if (volume < 100000) return 1.0;
    if (volume < 500000) return 1.25;
    return 1.5;

}

} // namespace elizaos
