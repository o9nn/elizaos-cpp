#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_TEST_UTILS_PLUGIN-UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_TEST_UTILS_PLUGIN-UTILS_H
#include "core.h"
#include "@/src/project.h"
#include "@/src/utils/directory-detection.h"
#include "@elizaos/core.h"
#include "node:fs.h"
#include "node:path.h"
using path = _default;

std::shared_ptr<Promise<array<std::shared_ptr<Plugin>>>> loadPluginDependencies(std::shared_ptr<DirectoryInfo> projectInfo);

#endif
