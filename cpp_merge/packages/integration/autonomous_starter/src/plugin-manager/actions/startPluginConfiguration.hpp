#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_MANAGER_ACTIONS_STARTPLUGINCONFIGURATION_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_MANAGER_ACTIONS_STARTPLUGINCONFIGURATION_H
#include "core.hpp"
// External dependency removed
#include "../types.h"
#include "../services/pluginConfigurationService.h"
#include "../services/pluginUserInteractionService.h"

extern std::shared_ptr<Action> startPluginConfigurationAction;
std::shared_ptr<Promise<any>> extractPluginNameFromMessage(std::shared_ptr<IAgentRuntime> runtime, string text);

#endif
