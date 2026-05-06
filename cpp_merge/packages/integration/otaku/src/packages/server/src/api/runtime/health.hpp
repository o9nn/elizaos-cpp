#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_API_RUNTIME_HEALTH_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_API_RUNTIME_HEALTH_H
#include "core.hpp"
// External dependency removed
// External dependency removed
#include "express.hpp"
// Using alias removed (invalid transpilation)
#include "../../middleware.h"
#include "../../index.h"

std::shared_ptr<express::Router> createHealthRouter(std::shared_ptr<ElizaOS> elizaOS, std::shared_ptr<AgentServer> serverInstance);

#endif
