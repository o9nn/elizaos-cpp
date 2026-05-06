#include "validation.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

content is GetPriceContent isGetPriceContent(GetPriceContent content) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    typeof content.symbol == "string" &&;
    typeof content.currency == "string";
    );

}

} // namespace elizaos
