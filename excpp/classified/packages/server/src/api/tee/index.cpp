#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router teeRouter() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // Mount TEE functionality at root level
    // router.use('/', createTeeRouter(agents));

    return router;

}

} // namespace elizaos
