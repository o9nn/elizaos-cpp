#include "transactions.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bigint estimateGasWithBuffer(bigint estimatedGas, double buffer = 1.2) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (estimatedGas * BigInt(Math.floor(buffer * 100))) / 100n;

}

} // namespace elizaos
