#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V2_UUID_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V2_UUID_H
#include "core.h"
#include "zod.h"
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
