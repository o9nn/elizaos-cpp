#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUM-TRACKER_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUM-TRACKER_SRC_INDEX_H
#include "core.h"
#include "hono.h"
#include "hono/jsx-renderer.h"
#include "hono/bun.h"
#include "hono/logger.h"
#include "hono/cors.h"
#include "./routes/api.h"
using api = _default;
#include "./components/ModernLayout.h"
#include "./components/ModernDashboard.h"
#include "./components/WalletDetail.h"
#include "./components/TokenDetail.h"
#include "./scripts/prefetch.h"

extern any app;
void startupPrefetch();

#endif
