#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_API_SHARED_VALIDATION_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_API_SHARED_VALIDATION_H
#include "core.hpp"
// External dependency removed
// External dependency removed

extern std::function<any(any, any)> getRuntime;
extern std::function<any(string)> validateAgentId;
extern std::function<any(string)> validateRoomId;
extern std::function<any(string, string)> validateChannelId;
extern std::function<any(string)> validateMemoryId;
extern std::function<any(string)> validateWorldId;
#endif
