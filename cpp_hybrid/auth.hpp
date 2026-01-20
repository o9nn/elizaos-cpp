#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_CLIENT_SRC_UTILS_AUTH_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_CLIENT_SRC_UTILS_AUTH_H
#include "core.h"

extern std::function<string(string)> sanitizeToken;
extern std::function<string()> getAuthToken;
extern std::function<any(string)> parseJwt;
extern std::function<boolean(string)> isTokenExpired;
#endif
