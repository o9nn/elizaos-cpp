#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-PLUGIN-MANAGER_SRC_COREEXTENSIONS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-PLUGIN-MANAGER_SRC_COREEXTENSIONS_H
#include "core.h"
#include "@elizaos/core.h"

void extendRuntimeWithEventUnregistration(std::shared_ptr<IAgentRuntime> runtime);

void extendRuntimeWithComponentUnregistration(std::shared_ptr<IAgentRuntime> runtime);

void applyRuntimeExtensions(std::shared_ptr<IAgentRuntime> runtime);

#endif
