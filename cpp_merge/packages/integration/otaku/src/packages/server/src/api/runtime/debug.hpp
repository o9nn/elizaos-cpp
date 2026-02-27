#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_API_RUNTIME_DEBUG_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_API_RUNTIME_DEBUG_H
#include "core.hpp"
#include "express.hpp"
// Using alias removed (invalid transpilation)
#include "../../index.h"
#include "../../middleware.h"

std::shared_ptr<express::Router> createDebugRouter(std::shared_ptr<AgentServer> serverInstance);

#endif
