#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_DYNAMIC_ACTIONS_PLUGIN_CREATION_ACTIONS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_DYNAMIC_ACTIONS_PLUGIN_CREATION_ACTIONS_H
#include "core.hpp"
// External dependency removed
#include "../services/plugin-creation-service.h"
#include "zod.hpp"
#include "../utils/validation.h"

extern any PluginSpecificationSchema;
extern std::shared_ptr<Action> createPluginAction;
extern std::shared_ptr<Action> checkPluginCreationStatusAction;
extern std::shared_ptr<Action> cancelPluginCreationAction;
extern std::shared_ptr<Action> createPluginFromDescriptionAction;
std::shared_ptr<Promise<std::shared_ptr<PluginSpecification>>> generatePluginSpecification(string description, std::shared_ptr<IAgentRuntime> runtime);

#endif
