#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_START_UTILS_DEPENDENCY_RESOLVER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_START_UTILS_DEPENDENCY_RESOLVER_H
#include "core.hpp"
// External dependency removed

array<std::shared_ptr<Plugin>> resolvePluginDependencies(std::shared_ptr<Map<string, std::shared_ptr<Plugin>>> availablePlugins, boolean isTestMode = false);

#endif
