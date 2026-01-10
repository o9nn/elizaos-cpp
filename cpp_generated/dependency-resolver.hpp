#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_START_UTILS_DEPENDENCY-RESOLVER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_START_UTILS_DEPENDENCY-RESOLVER_H
#include "core.h"
#include "@elizaos/core.h"

array<std::shared_ptr<Plugin>> resolvePluginDependencies(std::shared_ptr<Map<string, std::shared_ptr<Plugin>>> availablePlugins, boolean isTestMode = false);

#endif
