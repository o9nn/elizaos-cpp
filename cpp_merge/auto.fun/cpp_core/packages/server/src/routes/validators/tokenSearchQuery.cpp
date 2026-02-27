#include "tokenSearchQuery.hpp"
#include <string>
#include <iostream>
#include <stdexcept>

namespace elizaos {

SearchTokenInput parseSearchTokenRequest(const std::string& data) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return SearchTokenBodySchema.parse(data);

}

} // namespace elizaos
