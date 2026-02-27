#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_SERVER_SRC_API_RUNTIME_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_SERVER_SRC_API_RUNTIME_INDEX_H
#include "core.h"
#include "@elizaos/core.h"
#include "express.h"
using express = _default;
#include "../../index.h"
#include "./health.h"
#include "./logging.h"
#include "./debug.h"

std::shared_ptr<express::Router> runtimeRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> serverInstance);

#endif
