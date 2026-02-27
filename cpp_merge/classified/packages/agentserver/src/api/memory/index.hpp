#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_API_MEMORY_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC_API_MEMORY_INDEX_H
#include "core.hpp"
// External dependency removed
#include "express.hpp"
// Using alias removed (invalid transpilation)
#include "../../server.h"
#include "./agents.h"
#include "./groups.h"
#include "./rooms.h"

std::shared_ptr<express::Router> memoryRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> serverInstance);

#endif
