#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_START_UTILS_PLUGIN-UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_START_UTILS_PLUGIN-UTILS_H
#include "core.h"
#include "@/src/utils.h"
#include "@/src/utils/plugin-context.h"
#include "@elizaos/core.h"
#include "../types.h"

any isValidPluginShape(any obj);

std::shared_ptr<Promise<any>> loadAndPreparePlugin(string pluginName);

std::shared_ptr<PluginValidation> validatePlugin(any plugin);

#endif
