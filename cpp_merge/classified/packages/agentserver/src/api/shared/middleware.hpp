#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_API_SHARED_MIDDLEWARE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_API_SHARED_MIDDLEWARE_H
#include "core.hpp"
// External dependency removed
#include "express.hpp"
// Using alias removed (invalid transpilation)
// External dependency removed
#include "./response-utils.h"
#include "./validation.h"
#include "express-rate-limit.h"
// Using alias removed (invalid transpilation)

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
