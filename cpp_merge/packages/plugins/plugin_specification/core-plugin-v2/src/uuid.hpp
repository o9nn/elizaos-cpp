#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V2_SRC_UUID_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V2_SRC_UUID_H
#include "core.hpp"
#include "zod.hpp"
#include "./types.h"
// External dependency removed
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
