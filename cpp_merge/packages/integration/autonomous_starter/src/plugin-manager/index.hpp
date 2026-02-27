#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-MANAGER_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-MANAGER_INDEX_H
#include "core.h"
#include "@elizaos/core.h"
#include "./services/pluginManagerService.h"
#include "./services/pluginConfigurationService.h"
#include "./services/pluginUserInteractionService.h"
#include "./actions/loadPlugin.h"
#include "./actions/unloadPlugin.h"
#include "./actions/startPluginConfiguration.h"
#include "./actions/installPluginFromRegistry.js.h"
#include "./providers/pluginStateProvider.h"
#include "./providers/pluginConfigurationStatus.h"
#include "./providers/registryPluginsProvider.h"
#include "./evaluators/pluginConfigurationEvaluator.h"
#include "./types.h"

extern std::shared_ptr<Plugin> pluginManagerPlugin;
#endif
