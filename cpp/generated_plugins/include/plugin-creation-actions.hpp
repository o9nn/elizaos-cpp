#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-DYNAMIC_ACTIONS_PLUGIN-CREATION-ACTIONS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-DYNAMIC_ACTIONS_PLUGIN-CREATION-ACTIONS_H
#include "core.h"
#include "@elizaos/core.h"
#include "../services/plugin-creation-service.h"
#include "zod.h"
#include "../utils/validation.h"

extern any PluginSpecificationSchema;
extern std::shared_ptr<Action> createPluginAction;
extern std::shared_ptr<Action> checkPluginCreationStatusAction;
extern std::shared_ptr<Action> cancelPluginCreationAction;
extern std::shared_ptr<Action> createPluginFromDescriptionAction;
std::shared_ptr<Promise<std::shared_ptr<PluginSpecification>>> generatePluginSpecification(string description, std::shared_ptr<IAgentRuntime> runtime);

#endif
