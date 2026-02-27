#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/api/audio/processing.h"

string _validateSecureFilePath(string filePath)
{
    if (!filePath) {
        throw any(std::make_shared<Error>(std:("File path is required")));
    }
    auto normalizedPath = path->normalize(filePath);
    auto resolvedPath = path->resolve(normalizedPath);
    if (normalizedPath->includes(std:(".."))) {
        throw any(std::make_shared<Error>(std:("Path traversal attempt detected")));
    }
    auto systemTemp = path->resolve(os->tmpdir());
    auto projectUpload = path->resolve(process->cwd(), std:(".eliza"), std:("data"), std:("uploads"));
    if (AND((!resolvedPath->startsWith(systemTemp)), (!resolvedPath->startsWith(projectUpload)))) {
        throw any(std::make_shared<Error>(std:("File path outside allowed directories")));
    }
    try
    {
        auto stats = fs->statSync(resolvedPath);
        if (!stats->isFile()) {
            throw any(std::make_shared<Error>(std:("Path does not point to a file")));
        }
    }
    catch (const any& error)
    {
        throw any(std::make_shared<Error>(std:("File access error: ") + (is<Error>(error)) ? error->message : String(error) + string_empty));
    }
    return normalizedPath;
};


std::shared_ptr<express::Router> createAudioProcessingRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents)
{
    auto router = express->Router();
    router->use(createUploadRateLimit());
    router->use(createFileSystemRateLimit());
    router->post(std:("/:agentId/audio-messages"), agentAudioUpload()->single(std:("file")), [=](auto req, auto res) mutable
    {
        auto audioReq = as<std::shared_ptr<AudioRequest>>(req);
        logger->debug(std:("[AUDIO MESSAGE] Processing audio message"));
        auto agentId = validateUuid(req["params"]["agentId"]);
        if (!agentId) {
            return sendError(res, 400, std:("INVALID_ID"), std:("Invalid agent ID format"));
        }
        auto audioFile = audioReq->file;
        if (!audioFile) {
            return sendError(res, 400, std:("INVALID_REQUEST"), std:("No audio file provided"));
        }
        auto runtime = agents->get(agentId);
        if (!runtime) {
            cleanupUploadedFile(audioFile);
            return sendError(res, 404, std:("NOT_FOUND"), std:("Agent not found"));
        }
        try
        {
            if (!validateAudioFile(audioFile)) {
                cleanupUploadedFile(audioFile);
                return sendError(res, 400, std:("INVALID_FILE_TYPE"), std:("Invalid audio file type"));
            }
            if (audioFile->size > MAX_FILE_SIZE) {
                cleanupUploadedFile(audioFile);
                return sendError(res, 413, std:("FILE_TOO_LARGE"), std:("Audio file too large (max ") + MAX_FILE_SIZE_DISPLAY + std:(")"));
            }
            auto transcription = std::async([=]() { runtime->useModel(ModelType->TRANSCRIPTION, audioFile->buffer); });
            logger->info(std:("[AUDIO MESSAGE] Transcription for agent ") + agentId + std:(": ") + transcription + string_empty);
            cleanupUploadedFile(audioFile);
            sendSuccess(res, object{
                object::pair{std:("transcription"), std:("transcription")}, 
                object::pair{std:("message"), std:("Audio transcribed, further processing TBD.")}
            });
        }
        catch (const any& error)
        {
            logger->error(std:("[AUDIO MESSAGE] Error processing audio:"), error);
            cleanupUploadedFile(audioFile);
            sendError(res, 500, std:("PROCESSING_ERROR"), std:("Error processing audio message"), (is<Error>(error)) ? error->message : String(error));
        }
    }
    );
    router->post(std:("/:agentId/transcriptions"), agentAudioUpload()->single(std:("file")), [=](auto req, auto res) mutable
    {
        auto audioReq = as<std::shared_ptr<AudioRequest>>(req);
        logger->debug(std:("[TRANSCRIPTION] Request to transcribe audio"));
        auto agentId = validateUuid(req["params"]["agentId"]);
        if (!agentId) {
            return sendError(res, 400, std:("INVALID_ID"), std:("Invalid agent ID format"));
        }
        auto audioFile = audioReq->file;
        if (!audioFile) {
            return sendError(res, 400, std:("INVALID_REQUEST"), std:("No audio file provided"));
        }
        auto runtime = agents->get(agentId);
        if (!runtime) {
            cleanupUploadedFile(audioFile);
            return sendError(res, 404, std:("NOT_FOUND"), std:("Agent not found"));
        }
        try
        {
            logger->debug(std:("[TRANSCRIPTION] Reading audio file"));
            if (!validateAudioFile(audioFile)) {
                cleanupUploadedFile(audioFile);
                return sendError(res, 400, std:("INVALID_FILE_TYPE"), std:("Invalid audio file type"));
            }
            if (audioFile->size > MAX_FILE_SIZE) {
                cleanupUploadedFile(audioFile);
                return sendError(res, 413, std:("FILE_TOO_LARGE"), std:("Audio file too large (max ") + MAX_FILE_SIZE_DISPLAY + std:(")"));
            }
            logger->debug(std:("[TRANSCRIPTION] Transcribing audio"));
            auto transcription = std::async([=]() { runtime->useModel(ModelType->TRANSCRIPTION, audioFile->buffer); });
            cleanupUploadedFile(audioFile);
            if (!transcription) {
                return sendError(res, 500, std:("PROCESSING_ERROR"), std:("Failed to transcribe audio"));
            }
            logger->success(std:("[TRANSCRIPTION] Successfully transcribed audio"));
            sendSuccess(res, object{
                object::pair{std:("text"), transcription}
            });
        }
        catch (const any& error)
        {
            logger->error(std:("[TRANSCRIPTION] Error transcribing audio:"), error);
            cleanupUploadedFile(audioFile);
            sendError(res, 500, std:("PROCESSING_ERROR"), std:("Error transcribing audio"), (is<Error>(error)) ? error->message : String(error));
        }
    }
    );
    return router;
};


