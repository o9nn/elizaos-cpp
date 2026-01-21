#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_CLIENT_SRC_UTILS_AUTH_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_CLIENT_SRC_UTILS_AUTH_H
#include "core.h"

extern std::function<std::string(std::string)> sanitizeToken;
extern std::function<std::string()> getAuthToken;
extern std::function<std::any(std::string)> parseJwt;
extern std::function<boolean(std::string)> isTokenExpired;
#endif
