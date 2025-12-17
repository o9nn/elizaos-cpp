#include "bignumber.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

BigNumber toBN(const std::variant<std::string, double, BigNumber>& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new BigNumber(value);

}

} // namespace elizaos
