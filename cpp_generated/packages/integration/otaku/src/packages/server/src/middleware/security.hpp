#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_MIDDLEWARE_SECURITY_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_MIDDLEWARE_SECURITY_H
#include "core.h"
#include "express.h"
using express = _default;
#include "@elizaos/core.h"

extern std::function<std::function<void(any, any, any)>()> securityMiddleware;
#endif
