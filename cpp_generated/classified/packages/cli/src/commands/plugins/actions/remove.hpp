#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_PLUGINS_ACTIONS_REMOVE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_PLUGINS_ACTIONS_REMOVE_H
#include "core.h"
#include "@/src/utils/directory-detection.h"
#include "@/src/utils.h"
#include "@elizaos/core.h"
#include "../../../utils/bun-exec.js.h"
#include "node:fs.h"
#include "node:path.h"
using path = _default;
#include "../utils/naming.h"
#include "../utils/directory.h"

std::shared_ptr<Promise<void>> removePlugin(string plugin);

#endif
