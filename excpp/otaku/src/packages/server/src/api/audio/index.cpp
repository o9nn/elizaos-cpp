#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router audioRouter(ElizaOS elizaOS) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // Mount audio processing (upload, transcription)
    router.use("/", createAudioProcessingRouter(elizaOS));

    // Mount text-to-speech synthesis
    router.use("/", createSynthesisRouter(elizaOS));

    // Mount speech conversation functionality
    router.use("/", createConversationRouter(elizaOS));

    return router;

}

} // namespace elizaos
