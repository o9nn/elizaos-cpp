#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_PLUGIN_MANAGER_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_PLUGIN_MANAGER_SRC_INDEX_H
#include "core.hpp"
// External dependency removed
#include "./services/pluginManagerService.h"
#include "./services/pluginConfigurationService.h"
#include "./actions/loadPlugin.h"
#include "./actions/unloadPlugin.h"
#include "./actions/installPluginFromRegistry.h"
#include "./actions/searchPluginAction.h"
#include "./actions/clonePluginAction.h"
#include "./actions/publishPluginAction.h"
#include "./providers/pluginStateProvider.h"
#include "./providers/pluginConfigurationStatus.h"
#include "./providers/registryPluginsProvider.h"
#include "./providers/pluginKnowledgeProvider.h"
#include "./types.h"
// External dependency removed

extern std::shared_ptr<Plugin> pluginManagerPlugin;
#endif
