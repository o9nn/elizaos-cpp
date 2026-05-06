#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-PLUGIN-MANAGER_SRC_ACTIONS_SEARCHPLUGINACTION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-PLUGIN-MANAGER_SRC_ACTIONS_SEARCHPLUGINACTION_H
#include "core.h"
#include "@elizaos/core.h"
#include "../services/pluginRegistryService.js.h"

extern std::shared_ptr<Action> searchPluginAction;
any extractSearchQuery(string text);

extern std::shared_ptr<Action> getPluginDetailsAction;
#endif
