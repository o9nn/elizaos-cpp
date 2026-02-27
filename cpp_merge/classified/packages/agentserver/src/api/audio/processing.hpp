#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_API_AUDIO_PROCESSING_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_API_AUDIO_PROCESSING_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"
#include "express.h"
using express = _default;
#include "node:fs.h"
using fs = _default;
#include "node:os.h"
using os = _default;
#include "node:path.h"
using path = _default;
#include "../shared/file-utils.h"
#include "../shared/response-utils.h"
#include "../shared/uploads/index.h"
#include "../shared/middleware.h"
#include "../shared/constants.h"

class AudioRequest;

class AudioRequest, public std::enable_shared_from_this<AudioRequest> {
public:
    using std::enable_shared_from_this<AudioRequest>::shared_from_this;
    std::shared_ptr<Express::Multer::File> file;

    object params;
};

string _validateSecureFilePath(string filePath);

std::shared_ptr<express::Router> createAudioProcessingRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents);

#endif
