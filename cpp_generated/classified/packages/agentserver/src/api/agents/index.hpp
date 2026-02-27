#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_API_AGENTS_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_API_AGENTS_INDEX_H
#include "core.h"
#include "@elizaos/core.h"
#include "express.h"
using express = _default;
#include "../../server.h"
#include "./crud.h"
#include "./lifecycle.h"
#include "./worlds.h"
#include "./panels.h"
#include "./logs.h"
#include "./capabilities.h"
#include "./goals.h"
#include "./todos.h"
#include "./settings.h"
#include "./knowledge.h"
#include "./plugins.h"
#include "../memory/agents.h"
#include "../memory/rooms.h"
#include "./portability.h"

std::shared_ptr<express::Router> agentsRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> serverInstance);

#endif
