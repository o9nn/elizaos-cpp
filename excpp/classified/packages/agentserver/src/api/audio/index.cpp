#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router audioRouter(const std::unordered_map<UUID, IAgentRuntime>& agents) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // Mount audio processing (upload, transcription)
    router.use('/', createAudioProcessingRouter(agents));

    // Mount text-to-speech synthesis
    router.use('/', createSynthesisRouter(agents));

    // Mount speech conversation functionality
    router.use('/', createConversationRouter(agents));

    return router;

}

} // namespace elizaos
