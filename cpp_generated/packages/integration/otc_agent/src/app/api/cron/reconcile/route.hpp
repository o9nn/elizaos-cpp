#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_APP_API_CRON_RECONCILE_ROUTE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_APP_API_CRON_RECONCILE_ROUTE_H
#include "core.h"
#include "next/server.h"
#include "@/services/reconciliation.h"
#include "@/lib/agent-runtime.h"

extern string CRON_SECRET;
any GET(std::shared_ptr<NextRequest> request);

any POST(std::shared_ptr<NextRequest> request);

#endif
