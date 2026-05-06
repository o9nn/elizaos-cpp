#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_PLUGIN_MANAGER_SRC_COREEXTENSIONS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_PLUGIN_MANAGER_SRC_COREEXTENSIONS_H
#include "core.hpp"
// External dependency removed

void extendRuntimeWithEventUnregistration(std::shared_ptr<IAgentRuntime> runtime);

void extendRuntimeWithComponentUnregistration(std::shared_ptr<IAgentRuntime> runtime);

void applyRuntimeExtensions(std::shared_ptr<IAgentRuntime> runtime);

#endif
