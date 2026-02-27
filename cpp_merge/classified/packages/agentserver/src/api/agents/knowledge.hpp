#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_API_AGENTS_KNOWLEDGE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_API_AGENTS_KNOWLEDGE_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"
#include "express.h"
using express = _default;
#include "../../server.h"
#include "../shared/response-utils.h"

std::shared_ptr<express::Router> createAgentKnowledgeRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> _serverInstance);

#endif
