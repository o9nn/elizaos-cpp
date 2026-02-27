#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_API_AGENTS_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_API_AGENTS_INDEX_H
#include "core.hpp"
// External dependency removed
#include "express.hpp"
// Using alias removed (invalid transpilation)
#include "../../index.h"
#include "./crud.h"
#include "./lifecycle.h"
#include "./worlds.h"
#include "./panels.h"
#include "./logs.h"
#include "./runs.h"
#include "../memory/agents.h"
#include "../memory/rooms.h"

std::shared_ptr<express::Router> agentsRouter(std::shared_ptr<ElizaOS> elizaOS, std::shared_ptr<AgentServer> serverInstance);

#endif
