#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_API_AGENTS_KNOWLEDGE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_API_AGENTS_KNOWLEDGE_H
#include "core.hpp"
// External dependency removed
// External dependency removed
#include "express.hpp"
// Using alias removed (invalid transpilation)
#include "../../server.h"
#include "../shared/response-utils.h"

std::shared_ptr<express::Router> createAgentKnowledgeRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> _serverInstance);

#endif
