#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_SERVER_SRC_API_AUDIO_PROCESSING_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_SERVER_SRC_API_AUDIO_PROCESSING_H
#include "core.hpp"
// External dependency removed
// External dependency removed
#include "express.hpp"
// Using alias removed (invalid transpilation)
// Node.js module removed - use C++ filesystem
// Using alias removed (invalid transpilation)
// Node.js module removed - use C++ filesystem
// Using alias removed (invalid transpilation)
// Node.js module removed - use C++ filesystem
// Using alias removed (invalid transpilation)
#include "../shared/file-utils.js.h"
#include "../shared/response-utils.js.h"
#include "../shared/uploads/index.js.h"
#include "../shared/middleware.js.h"
#include "../shared/constants.js.h"

class AudioRequest;

class AudioRequest, public std::enable_shared_from_this<AudioRequest> {
public:
    using std::enable_shared_from_this<AudioRequest>::shared_from_this;
    std::shared_ptr<Express::Multer::File> file;

    object params;
};

string validateSecureFilePath(string filePath);

std::shared_ptr<express::Router> createAudioProcessingRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents);

#endif
