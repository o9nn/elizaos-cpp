#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_VERCEL-API_APP_API_PLUGINS_REGISTRY_ROUTE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_VERCEL-API_APP_API_PLUGINS_REGISTRY_ROUTE_H
#include "core.h"
#include "next/server.h"
#include "@/lib/parse-registry.h"
#include "@/lib/types.h"

extern any cachedData;
extern double cacheTimestamp;
extern double CACHE_DURATION;
any GET();

any OPTIONS();

#endif
