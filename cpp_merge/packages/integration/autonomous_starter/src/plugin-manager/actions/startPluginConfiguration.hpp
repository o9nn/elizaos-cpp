#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-MANAGER_ACTIONS_STARTPLUGINCONFIGURATION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-MANAGER_ACTIONS_STARTPLUGINCONFIGURATION_H
#include "core.h"
#include "@elizaos/core.h"
#include "../types.h"
#include "../services/pluginConfigurationService.h"
#include "../services/pluginUserInteractionService.h"

extern std::shared_ptr<Action> startPluginConfigurationAction;
std::shared_ptr<Promise<any>> extractPluginNameFromMessage(std::shared_ptr<IAgentRuntime> runtime, string text);

#endif
