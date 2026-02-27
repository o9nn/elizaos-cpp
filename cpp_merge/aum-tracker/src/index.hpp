#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUM_TRACKER_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUM_TRACKER_SRC_INDEX_H
#include "core.hpp"
#include "hono.hpp"
#include "hono/jsx-renderer.h"
#include "hono/bun.h"
#include "hono/logger.h"
#include "hono/cors.h"
#include "./routes/api.h"
// Using alias removed (invalid transpilation)
#include "./components/ModernLayout.h"
#include "./components/ModernDashboard.h"
#include "./components/WalletDetail.h"
#include "./components/TokenDetail.h"
#include "./scripts/prefetch.h"

extern any app;
void startupPrefetch();

#endif
