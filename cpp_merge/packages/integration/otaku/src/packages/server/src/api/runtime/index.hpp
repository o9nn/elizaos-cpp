#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_API_RUNTIME_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_API_RUNTIME_INDEX_H
#include "core.hpp"
// External dependency removed
#include "express.hpp"
// Using alias removed (invalid transpilation)
#include "../../index.h"
#include "./health.h"
#include "./logging.h"
#include "./debug.h"

std::shared_ptr<express::Router> runtimeRouter(std::shared_ptr<ElizaOS> elizaOS, std::shared_ptr<AgentServer> serverInstance);

#endif
