#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router systemRouter() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // Mount environment management under /env
    router.use("/env", createEnvironmentRouter());

    // Mount version information under /version
    router.use("/version", createVersionRouter());

    return router;

}

} // namespace elizaos
