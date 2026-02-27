#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V1_SRC_DATABASE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V1_SRC_DATABASE_H
#include "core.hpp"
#include "./types.h"
using IDatabaseAdapterFromTypes = IDatabaseAdapter;
// External dependency removed
using IDatabaseAdapterV2 = IDatabaseAdapter;

typedef IDatabaseAdapterFromTypes IDatabaseAdapter;


std::shared_ptr<Account> fromV2Entity(std::shared_ptr<Entity> entity);

std::shared_ptr<Entity> toV2Entity(std::shared_ptr<Account> account);

IDatabaseAdapter fromV2DatabaseAdapter(std::shared_ptr<IDatabaseAdapterV2> adapterV2);

std::shared_ptr<IDatabaseAdapterV2> toV2DatabaseAdapter(IDatabaseAdapter adapterV1);

#endif
