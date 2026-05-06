#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-PLUGIN-MANAGER_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-PLUGIN-MANAGER_SRC_INDEX_H
#include "core.h"
#include "@elizaos/core.h"
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
#include "@elizaos/core.h"

extern std::shared_ptr<Plugin> pluginManagerPlugin;
#endif
