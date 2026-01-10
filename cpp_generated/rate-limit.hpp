#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_MIDDLEWARE_RATE-LIMIT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_MIDDLEWARE_RATE-LIMIT_H
#include "core.h"
#include "express-rate-limit.h"
using rateLimit = _default;
#include "@elizaos/core.h"
#include "../api/shared/validation.h"

extern std::function<any()> createApiRateLimit;
extern std::function<any()> createFileSystemRateLimit;
extern std::function<any()> createUploadRateLimit;
extern std::function<any()> createChannelValidationRateLimit;
#endif
