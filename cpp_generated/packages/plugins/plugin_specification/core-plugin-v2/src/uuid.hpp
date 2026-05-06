#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V2_SRC_UUID_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V2_SRC_UUID_H
#include "core.h"
#include "zod.h"
#include "./types.h"
#include "@elizaos/core.h"
using coreValidateUuid = validateUuid;
using coreStringToUuid = stringToUuid;

extern any uuidSchema;
any validateUuid(any value);

template <typename P0>
std::shared_ptr<UUID> stringToUuid(P0 target);

template <typename P0>
std::shared_ptr<UUID> stringToUuid(P0 target)
{
    return coreStringToUuid(target);
};


#endif
