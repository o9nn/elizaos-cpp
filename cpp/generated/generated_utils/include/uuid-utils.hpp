#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-NEXTJS-STARTER_SRC_LIB_UUID-UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-NEXTJS-STARTER_SRC_LIB_UUID-UTILS_H
#include "core.h"
#include "js-sha1.h"

extern std::function<std::string(std::shared_ptr<Uint8Array>)> uint8ArrayToHex;
std::string generateUUIDFromString(std::string inputString);

std::string generateQueryRoomId(std::string seed, std::string query);

#endif
