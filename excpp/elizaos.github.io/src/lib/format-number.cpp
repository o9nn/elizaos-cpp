#include "format-number.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatCompactNumber(double number, auto maximumFractionDigits = 1) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto formatter = Intl.NumberFormat("en", {;
        notation: "compact",
        maximumFractionDigits,
        });
        return formatter.format(number);

}

} // namespace elizaos
