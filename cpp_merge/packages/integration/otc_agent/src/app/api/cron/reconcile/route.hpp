#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SRC_APP_API_CRON_RECONCILE_ROUTE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SRC_APP_API_CRON_RECONCILE_ROUTE_H
#include "core.hpp"
#include "next/server.h"
// External dependency removed
// External dependency removed

extern string CRON_SECRET;
any GET(std::shared_ptr<NextRequest> request);

any POST(std::shared_ptr<NextRequest> request);

#endif
