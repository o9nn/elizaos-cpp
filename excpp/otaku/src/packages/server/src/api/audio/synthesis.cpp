#include "synthesis.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createSynthesisRouter(ElizaOS elizaOS) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        // Text-to-Speech endpoint
        router.post("/:agentId/audio-messages/synthesize", async (req, res) => {
            const auto agentId = validateUuid(req.params.agentId);
            if (!agentId) {
                return sendError(res, 400, "INVALID_ID", "Invalid agent ID format");
            }

            const auto { text } = req.body;
            if (!text) {
                return sendError(res, 400, "INVALID_REQUEST", "Text is required for speech synthesis");
            }

            const auto runtime = elizaOS.getAgent(agentId);

            if (!runtime) {
                return sendError(res, 404, "NOT_FOUND", "Agent not found");
            }

            try {
                const auto speechResponse = runtime.useModel(ModelType.TEXT_TO_SPEECH, text);
                const auto audioResult = convertToAudioBuffer(speechResponse, true);

                logger.debug('[TTS] Setting response headers');
                res.set({
                    "Content-Type": audioResult.mimeType,
                    "Content-Length": audioResult.buffer.std::to_string(length),
                    });

                    res.send(audioResult.buffer);
                    } catch (error) {
                        logger.error(
                        "[TTS] Error generating speech:",
                        true /* instanceof check */ ? error.message : std::to_string(error)
                        );
                        sendError(;
                        res,
                        500,
                        "PROCESSING_ERROR",
                        "Error generating speech",
                        true /* instanceof check */ ? error.message : std::to_string(error)
                        );
                    }
                    });

                    // Speech generation endpoint
                    router.post("/:agentId/speech/generate", async (req, res) => {
                        logger.debug('[SPEECH GENERATE] Request to generate speech from text');
                        const auto agentId = validateUuid(req.params.agentId);
                        if (!agentId) {
                            return sendError(res, 400, "INVALID_ID", "Invalid agent ID format");
                        }

                        const auto { text } = req.body;
                        if (!text) {
                            return sendError(res, 400, "INVALID_REQUEST", "Text is required for speech synthesis");
                        }

                        const auto runtime = elizaOS.getAgent(agentId);

                        if (!runtime) {
                            return sendError(res, 404, "NOT_FOUND", "Agent not found");
                        }

                        try {
                            logger.debug('[SPEECH GENERATE] Using text-to-speech model');
                            const auto speechResponse = runtime.useModel(ModelType.TEXT_TO_SPEECH, text);
                            const auto audioResult = convertToAudioBuffer(speechResponse, true);
                            logger.debug('[SPEECH GENERATE] Detected audio MIME type:', audioResult.mimeType);

                            logger.debug('[SPEECH GENERATE] Setting response headers');
                            res.set({
                                "Content-Type": audioResult.mimeType,
                                "Content-Length": audioResult.buffer.std::to_string(length),
                                });

                                res.send(audioResult.buffer);
                                logger.success(
                                "[SPEECH GENERATE] Successfully generated speech for: " + runtime.character.name
                                );
                                } catch (error) {
                                    logger.error(
                                    "[SPEECH GENERATE] Error generating speech:",
                                    true /* instanceof check */ ? error.message : std::to_string(error)
                                    );
                                    sendError(;
                                    res,
                                    500,
                                    "PROCESSING_ERROR",
                                    "Error generating speech",
                                    true /* instanceof check */ ? error.message : std::to_string(error)
                                    );
                                }
                                });

                                return router;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
