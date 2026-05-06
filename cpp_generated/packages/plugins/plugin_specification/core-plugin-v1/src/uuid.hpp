#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V1_SRC_UUID_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V1_SRC_UUID_H
#include "core.h"
#include "./types.h"
using UUIDv1 = UUID;
#include "@elizaos/core-plugin-v2.h"

typedef UUIDv1 UUID;


UUID asUUID(string id);

UUID generateUuidFromString(string input);

#endif
