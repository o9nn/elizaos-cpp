#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_MIDDLEWARE_VALIDATION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_MIDDLEWARE_VALIDATION_H
#include "core.h"
#include "@elizaos/core.h"
#include "express.h"
using express = _default;
#include "@elizaos/core.h"
#include "../api/shared/response-utils.h"
#include "../api/shared/validation.h"
#include "./jwt.h"

extern std::function<std::function<void(any, any, any)>(any)> agentExistsMiddleware;
extern std::function<std::function<void(any, any, any)>(string)> validateUuidMiddleware;
extern std::function<std::function<void(any, any, any)>()> validateChannelIdMiddleware;
extern std::function<std::function<std::shared_ptr<Promise<void>>(std::shared_ptr<AuthenticatedRequest>, any, any)>(std::function<std::shared_ptr<Promise<array<any>>>(any)>)> requireChannelParticipant;
extern std::function<std::function<any(std::shared_ptr<AuthenticatedRequest>, any, any)>()> requireAuthenticated;
extern std::function<std::function<any(any, any, any)>()> validateContentTypeMiddleware;
#endif
