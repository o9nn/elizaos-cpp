#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-WORLD_SRC_LIB_CACHE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-WORLD_SRC_LIB_CACHE_H
#include "core.h"
#include "@/types.h"
#include "fs.h"
using fs = _default;
#include "path.h"
using path = _default;

extern std::any CACHE_FILE;
extern double CACHE_DURATION;
std::any getCachedData();

void setCachedData(array<std::shared_ptr<TokenHolding>> holdings);

boolean shouldRefreshCache();

#endif
