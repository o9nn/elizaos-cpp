#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V1_UUID_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V1_UUID_H
#include "core.h"
#include "../v2.h"
#include "./types.h"
using UUIDv1 = UUID;

typedef UUIDv1 UUID;


UUID asUUID(string id);

UUID generateUuidFromString(string input);

#endif
