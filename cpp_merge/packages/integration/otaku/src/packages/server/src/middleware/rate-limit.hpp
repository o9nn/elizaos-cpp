#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_MIDDLEWARE_RATE_LIMIT_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_MIDDLEWARE_RATE_LIMIT_H
#include "core.hpp"
#include "express-rate-limit.h"
// Using alias removed (invalid transpilation)
// External dependency removed
#include "../api/shared/validation.h"

extern std::function<any()> createApiRateLimit;
extern std::function<any()> createFileSystemRateLimit;
extern std::function<any()> createUploadRateLimit;
extern std::function<any()> createChannelValidationRateLimit;
#endif
