#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_START_UTILS_PLUGIN_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_START_UTILS_PLUGIN_UTILS_H
#include "core.hpp"
// External dependency removed
// External dependency removed
// External dependency removed
#include "../types.h"

any isValidPluginShape(any obj);

std::shared_ptr<Promise<any>> loadAndPreparePlugin(string pluginName);

std::shared_ptr<PluginValidation> validatePlugin(any plugin);

#endif
