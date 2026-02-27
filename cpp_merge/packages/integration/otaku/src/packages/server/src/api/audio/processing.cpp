#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/server/src/api/audio/processing.h"

std::shared_ptr<express::Router> createAudioProcessingRouter(std::shared_ptr<ElizaOS> elizaOS)
{
    auto router = express->Router();
    router->use(createUploadRateLimit());
    router->use(createFileSystemRateLimit());
    router->post(std::string("/:agentId/audio-messages"), agentAudioUpload()->single(std::string("file")), [=](auto req, auto res) mutable
    {
        auto audioReq = as<std::shared_ptr<AudioRequest>>(req);
        logger->debug(std::string("[AUDIO MESSAGE] Processing audio message"));
        auto agentId = validateUuid(req["params"]["agentId"]);
        if (!agentId) {
            return sendError(res, 400, std::string("INVALID_ID"), std::string("Invalid agent ID format"));
        }
        auto audioFile = audioReq->file;
        if (!audioFile) {
            return sendError(res, 400, std::string("INVALID_REQUEST"), std::string("No audio file provided"));
        }
        auto runtime = elizaOS->getAgent(agentId);
        if (!runtime) {
            cleanupUploadedFile(audioFile);
            return sendError(res, 404, std::string("NOT_FOUND"), std::string("Agent not found"));
        }
        try
        {
            if (!validateAudioFile(audioFile)) {
                cleanupUploadedFile(audioFile);
                return sendError(res, 400, std::string("INVALID_FILE_TYPE"), std::string("Invalid audio file type"));
            }
            if (audioFile->size > MAX_FILE_SIZE) {
                cleanupUploadedFile(audioFile);
                return sendError(res, 413, std::string("FILE_TOO_LARGE"), std::string("Audio file too large (max ") + MAX_FILE_SIZE_DISPLAY + std::string(")"));
            }
            auto transcription = std::async([=]() { runtime->useModel(ModelType->TRANSCRIPTION, audioFile->buffer); });
            logger->info(std::string("[AUDIO MESSAGE] Transcription for agent ") + agentId + std::string(": ") + transcription + string_empty);
            cleanupUploadedFile(audioFile);
            sendSuccess(res, object{
                object::pair{std::string("transcription"), std::string("transcription")}, 
                object::pair{std::string("message"), std::string("Audio transcribed, further processing TBD.")}
            });
        }
        catch (const any& error)
        {
            logger->error(std::string("[AUDIO MESSAGE] Error processing audio:"), (is<Error>(error)) ? error->message : String(error));
            cleanupUploadedFile(audioFile);
            sendError(res, 500, std::string("PROCESSING_ERROR"), std::string("Error processing audio message"), (is<Error>(error)) ? error->message : String(error));
        }
    }
    );
    router->post(std::string("/:agentId/transcriptions"), agentAudioUpload()->single(std::string("file")), [=](auto req, auto res) mutable
    {
        auto audioReq = as<std::shared_ptr<AudioRequest>>(req);
        logger->debug(std::string("[TRANSCRIPTION] Request to transcribe audio"));
        auto agentId = validateUuid(req["params"]["agentId"]);
        if (!agentId) {
            return sendError(res, 400, std::string("INVALID_ID"), std::string("Invalid agent ID format"));
        }
        auto audioFile = audioReq->file;
        if (!audioFile) {
            return sendError(res, 400, std::string("INVALID_REQUEST"), std::string("No audio file provided"));
        }
        auto runtime = elizaOS->getAgent(agentId);
        if (!runtime) {
            cleanupUploadedFile(audioFile);
            return sendError(res, 404, std::string("NOT_FOUND"), std::string("Agent not found"));
        }
        try
        {
            logger->debug(std::string("[TRANSCRIPTION] Reading audio file"));
            if (!validateAudioFile(audioFile)) {
                cleanupUploadedFile(audioFile);
                return sendError(res, 400, std::string("INVALID_FILE_TYPE"), std::string("Invalid audio file type"));
            }
            if (audioFile->size > MAX_FILE_SIZE) {
                cleanupUploadedFile(audioFile);
                return sendError(res, 413, std::string("FILE_TOO_LARGE"), std::string("Audio file too large (max ") + MAX_FILE_SIZE_DISPLAY + std::string(")"));
            }
            logger->debug(std::string("[TRANSCRIPTION] Transcribing audio"));
            auto transcription = std::async([=]() { runtime->useModel(ModelType->TRANSCRIPTION, audioFile->buffer); });
            cleanupUploadedFile(audioFile);
            if (!transcription) {
                return sendError(res, 500, std::string("PROCESSING_ERROR"), std::string("Failed to transcribe audio"));
            }
            logger->success(std::string("[TRANSCRIPTION] Successfully transcribed audio"));
            sendSuccess(res, object{
                object::pair{std::string("text"), transcription}
            });
        }
        catch (const any& error)
        {
            logger->error(std::string("[TRANSCRIPTION] Error transcribing audio:"), (is<Error>(error)) ? error->message : String(error));
            cleanupUploadedFile(audioFile);
            sendError(res, 500, std::string("PROCESSING_ERROR"), std::string("Error transcribing audio"), (is<Error>(error)) ? error->message : String(error));
        }
    }
    );
    return router;
};


