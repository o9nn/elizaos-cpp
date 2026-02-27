#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_NEXTJS_STARTER_SRC_LIB_UUID_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_NEXTJS_STARTER_SRC_LIB_UUID_UTILS_H
#include "core.hpp"
#include "js-sha1.h"

extern std::function<string(std::shared_ptr<Uint8Array>)> uint8ArrayToHex;
string generateUUIDFromString(string inputString);

string generateQueryRoomId(string seed, string query);

#endif
