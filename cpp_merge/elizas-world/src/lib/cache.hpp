#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_WORLD_SRC_LIB_CACHE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAS_WORLD_SRC_LIB_CACHE_H
#include "core.hpp"
// External dependency removed
#include "fs.hpp"
// Using alias removed (invalid transpilation)
#include "path.hpp"
// Using alias removed (invalid transpilation)

extern any CACHE_FILE;
extern double CACHE_DURATION;
any getCachedData();

void setCachedData(array<std::shared_ptr<TokenHolding>> holdings);

boolean shouldRefreshCache();

#endif
