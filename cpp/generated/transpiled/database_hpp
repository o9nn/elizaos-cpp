#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V1_SRC_DATABASE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V1_SRC_DATABASE_H
#include "core.h"
#include "./types.h"
using IDatabaseAdapterFromTypes = IDatabaseAdapter;
#include "@elizaos/core-plugin-v2.h"
using IDatabaseAdapterV2 = IDatabaseAdapter;

typedef IDatabaseAdapterFromTypes IDatabaseAdapter;


std::shared_ptr<Account> fromV2Entity(std::shared_ptr<Entity> entity);

std::shared_ptr<Entity> toV2Entity(std::shared_ptr<Account> account);

IDatabaseAdapter fromV2DatabaseAdapter(std::shared_ptr<IDatabaseAdapterV2> adapterV2);

std::shared_ptr<IDatabaseAdapterV2> toV2DatabaseAdapter(IDatabaseAdapter adapterV1);

#endif
