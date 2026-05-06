#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_APP_API_RPC_BASE_ROUTE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_APP_API_RPC_BASE_ROUTE_H
#include "core.h"
#include "next/server.h"

extern string ALCHEMY_API_KEY;
extern string ALCHEMY_BASE_URL;
any POST(std::shared_ptr<NextRequest> request);

#endif
