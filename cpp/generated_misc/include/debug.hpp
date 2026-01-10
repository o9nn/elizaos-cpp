#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_SERVER_SRC_API_RUNTIME_DEBUG_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_SERVER_SRC_API_RUNTIME_DEBUG_H
#include "core.h"
#include "express.h"
using express = _default;
#include "../../index.h"

std::shared_ptr<express::Router> createDebugRouter(std::shared_ptr<AgentServer> serverInstance);

#endif
