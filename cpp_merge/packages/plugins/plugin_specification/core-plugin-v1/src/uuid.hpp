#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V1_SRC_UUID_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V1_SRC_UUID_H
#include "core.hpp"
#include "./types.h"
using UUIDv1 = UUID;
// External dependency removed

typedef UUIDv1 UUID;


UUID asUUID(string id);

UUID generateUuidFromString(string input);

#endif
