#include "tokenSearchQuery.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

SearchTokenInput parseSearchTokenRequest(unknown data) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return SearchTokenBodySchema.parse(data);

}

} // namespace elizaos
