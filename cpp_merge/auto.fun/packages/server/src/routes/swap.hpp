#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_ROUTES_SWAP_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_ROUTES_SWAP_H
#include "core.h"
#include "drizzle-orm.h"
#include "hono.h"
#include "hono/cache.h"
using honoCacheMiddleware = cache;
#include "zod.h"
#include "../chart.h"
#include "../db.h"
#include "../redis.h"
#include "../util.h"
#include "@codex-data/sdk.h"
#include "moment.h"
using moment = _default;
#include "@codex-data/sdk/dist/resources/graphql.h"

extern any codex;
extern any router;
extern any ChartParamsSchema;
#endif
