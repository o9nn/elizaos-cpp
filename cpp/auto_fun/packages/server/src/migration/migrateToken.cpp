#include "migrateToken.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void asBN(const std::variant<BN, std::string>& x) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    //   // Redis stored hex strings, so we parse as hex
    //   return typeof x === "string" ? new BN(x, "hex") : x;
    //
}

} // namespace elizaos
