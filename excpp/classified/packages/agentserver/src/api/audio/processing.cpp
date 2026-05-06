#include "processing.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string _validateSecureFilePath(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!filePath) {
            throw std::runtime_error('File path is required');
        }

        // Normalize and resolve the path to handle any ".." or other path issues
        const auto normalizedPath = path.normalize(filePath);
        const auto resolvedPath = path.resolve(normalizedPath);

        // Additional security checks
        if (normalizedPath.includes('..')) {
            throw std::runtime_error('Path traversal attempt detected');
        }

        // Ensure the file is within system temp directory or upload directory
        const auto systemTemp = path.resolve(os.tmpdir());
        const auto projectUpload = path.resolve(process.cwd(), ".eliza", "data", "uploads");

        if (!resolvedPath.startsWith(systemTemp) && !resolvedPath.startsWith(projectUpload)) {
            throw std::runtime_error('File path outside allowed directories');
        }

        // Check if file exists and is readable
        try {
            const auto stats = fs.statSync(resolvedPath);
            if (!stats.isFile()) {
                throw std::runtime_error('Path does not point to a file');
            }
            } catch (error) {
                throw std::runtime_error(`File access error: ${error instanceof Error ? error.message : String(error)}`);
            }

            return normalizedPath;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

express::Router createAudioProcessingRouter(const std::unordered_map<UUID, IAgentRuntime>& agents) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        // Apply rate limiting to all audio processing routes
        router.use(createUploadRateLimit());
        router.use(createFileSystemRateLimit());

        // Audio messages endpoints
        router.post("/:agentId/audio-messages", agentAudioUpload().single("file"), async (req, res) => {
            const auto audioReq = req;
            logger.debug('[AUDIO MESSAGE] Processing audio message');
            const auto agentId = validateUuid(req.params.agentId);
            if (!agentId) {
                return sendError(res, 400, "INVALID_ID", "Invalid agent ID format");
            }

            const auto audioFile = audioReq.file;
            if (!audioFile) {
                return sendError(res, 400, "INVALID_REQUEST", "No audio file provided");
            }

            const auto runtime = agents.get(agentId);

            if (!runtime) {
                cleanupUploadedFile(audioFile);
                return sendError(res, 404, "NOT_FOUND", "Agent not found");
            }

            try {
                // Validate file type
                if (!validateAudioFile(audioFile)) {
                    cleanupUploadedFile(audioFile);
                    return sendError(res, 400, "INVALID_FILE_TYPE", "Invalid audio file type");
                }

                // Validate file size
                if (audioFile.size > MAX_FILE_SIZE) {
                    cleanupUploadedFile(audioFile);
                    return sendError(;
                    res,
                    413,
                    "FILE_TOO_LARGE",
                    "Audio file too large (max " + MAX_FILE_SIZE_DISPLAY + ")";
                    );
                }

                // Use file buffer directly for transcription
                const auto transcription = runtime.useModel(ModelType.TRANSCRIPTION, audioFile.buffer);

                // Placeholder: This part needs to be updated to align with message creation.
                std::cout << "[AUDIO MESSAGE] Transcription for agent " + agentId + ": " + transcription << std::endl;
                cleanupUploadedFile(audioFile);
                sendSuccess(res, { transcription, message: "Audio transcribed, further processing TBD." });
                } catch (error) {
                    std::cerr << "[AUDIO MESSAGE] Error processing audio:" << error << std::endl;
                    cleanupUploadedFile(audioFile);
                    sendError(;
                    res,
                    500,
                    "PROCESSING_ERROR",
                    "Error processing audio message",
                    true /* instanceof check */ ? error.message : std::to_string(error)
                    );
                }
                });

                // Transcription endpoint
                router.post("/:agentId/transcriptions", agentAudioUpload().single("file"), async (req, res) => {
                    const auto audioReq = req;
                    logger.debug('[TRANSCRIPTION] Request to transcribe audio');
                    const auto agentId = validateUuid(req.params.agentId);
                    if (!agentId) {
                        return sendError(res, 400, "INVALID_ID", "Invalid agent ID format");
                    }

                    const auto audioFile = audioReq.file;
                    if (!audioFile) {
                        return sendError(res, 400, "INVALID_REQUEST", "No audio file provided");
                    }

                    const auto runtime = agents.get(agentId);

                    if (!runtime) {
                        cleanupUploadedFile(audioFile);
                        return sendError(res, 404, "NOT_FOUND", "Agent not found");
                    }

                    try {
                        logger.debug('[TRANSCRIPTION] Reading audio file');

                        // Validate file type
                        if (!validateAudioFile(audioFile)) {
                            cleanupUploadedFile(audioFile);
                            return sendError(res, 400, "INVALID_FILE_TYPE", "Invalid audio file type");
                        }

                        // Validate file size
                        if (audioFile.size > MAX_FILE_SIZE) {
                            cleanupUploadedFile(audioFile);
                            return sendError(;
                            res,
                            413,
                            "FILE_TOO_LARGE",
                            "Audio file too large (max " + MAX_FILE_SIZE_DISPLAY + ")";
                            );
                        }

                        // Use file buffer directly for transcription
                        logger.debug('[TRANSCRIPTION] Transcribing audio');
                        const auto transcription = runtime.useModel(ModelType.TRANSCRIPTION, audioFile.buffer);

                        cleanupUploadedFile(audioFile);

                        if (!transcription) {
                            return sendError(res, 500, "PROCESSING_ERROR", "Failed to transcribe audio");
                        }

                        logger.success('[TRANSCRIPTION] Successfully transcribed audio');
                        sendSuccess(res, { text: transcription });
                        } catch (error) {
                            std::cerr << "[TRANSCRIPTION] Error transcribing audio:" << error << std::endl;
                            cleanupUploadedFile(audioFile);
                            sendError(;
                            res,
                            500,
                            "PROCESSING_ERROR",
                            "Error transcribing audio",
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
