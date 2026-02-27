#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_PLUGIN_MANAGER_SRC_ACTIONS_SEARCHPLUGINACTION_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_PLUGIN_MANAGER_SRC_ACTIONS_SEARCHPLUGINACTION_H
#include "core.hpp"
// External dependency removed
#include "../services/pluginRegistryService.js.h"

extern std::shared_ptr<Action> searchPluginAction;
any extractSearchQuery(string text);

extern std::shared_ptr<Action> getPluginDetailsAction;
#endif
