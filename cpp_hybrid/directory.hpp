#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_PLUGINS_UTILS_DIRECTORY_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_PLUGINS_UTILS_DIRECTORY_H
#include "core.h"
#include "@/src/utils/directory-detection.h"
#include "@elizaos/core.h"
#include "node:fs.h"
#include "node:path.h"
using path = _default;
#include "../types.h"

extern std::function<std::shared_ptr<Record<string, string>>(string)> getDependenciesFromDirectory;
#endif
