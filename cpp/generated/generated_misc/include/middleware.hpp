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

extern std::function<std::function<void(std::any, std::any, std::any)>(std::shared_ptr<Map<std::any, any>>)> agentExistsMiddleware;
extern std::function<std::function<void(std::any, std::any, std::any)>(std::string)> validateUuidMiddleware;
extern std::function<std::function<void(std::any, std::any, std::any)>()> validateChannelIdMiddleware;
extern std::function<std::function<void(std::any, std::any, std::any)>()> securityMiddleware;
extern std::function<std::function<std::any(std::any, std::any, std::any)>()> validateContentTypeMiddleware;
extern std::function<std::any()> createApiRateLimit;
extern std::function<std::any()> createFileSystemRateLimit;
extern std::function<std::any()> createUploadRateLimit;
extern std::function<std::any()> createChannelValidationRateLimit;
#endif
