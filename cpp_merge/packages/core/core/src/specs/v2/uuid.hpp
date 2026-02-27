#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V2_UUID_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V2_UUID_H
#include "core.hpp"
#include "zod.hpp"
#include "../../utils.h"
using coreStringToUuid = stringToUuid;
using coreValidateUuid = validateUuid;
#include "./types.h"

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
