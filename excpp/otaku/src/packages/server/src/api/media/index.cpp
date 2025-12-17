#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router mediaRouter() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // Mount agent media uploads under /agents
    router.use('/agents', createAgentMediaRouter());

    // Mount channel media uploads under /channels
    router.use('/channels', createChannelMediaRouter());

    return router;

}

} // namespace elizaos
