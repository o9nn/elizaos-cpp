#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_SERVER_SRC_API_AUDIO_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_SERVER_SRC_API_AUDIO_INDEX_H
#include "core.hpp"
// External dependency removed
#include "express.hpp"
// Using alias removed (invalid transpilation)
#include "./processing.h"
#include "./synthesis.h"
#include "./conversation.h"

std::shared_ptr<express::Router> audioRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents);

#endif
