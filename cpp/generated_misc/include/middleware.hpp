#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_SERVER_SRC_API_SHARED_MIDDLEWARE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_SERVER_SRC_API_SHARED_MIDDLEWARE_H
#include "core.h"
#include "@elizaos/core.h"
#include "express.h"
using express = _default;
#include "@elizaos/core.h"
#include "./response-utils.h"
#include "./validation.h"
#include "express-rate-limit.h"
using rateLimit = _default;

extern std::function<std::function<void(any, any, any)>(std::shared_ptr<Map<any, any>>)> agentExistsMiddleware;
extern std::function<std::function<void(any, any, any)>(string)> validateUuidMiddleware;
extern std::function<std::function<void(any, any, any)>()> validateChannelIdMiddleware;
extern std::function<std::function<void(any, any, any)>()> securityMiddleware;
extern std::function<std::function<any(any, any, any)>()> validateContentTypeMiddleware;
extern std::function<any()> createApiRateLimit;
extern std::function<any()> createFileSystemRateLimit;
extern std::function<any()> createUploadRateLimit;
extern std::function<any()> createChannelValidationRateLimit;
#endif
