#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_SERVER_SRC_API_SHARED_RESPONSE-UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_SERVER_SRC_API_SHARED_RESPONSE-UTILS_H
#include "core.h"
#include "express.h"
using express = _default;

extern std::function<void(std::any, double, std::string, std::string, std::string)> sendError;
extern std::function<void(std::any, std::any, double)> sendSuccess;
#endif
