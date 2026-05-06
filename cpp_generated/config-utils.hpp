#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_START_UTILS_CONFIG-UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_START_UTILS_CONFIG-UTILS_H
#include "core.h"
#include "@/src/utils.h"
#include "@elizaos/core.h"
#include "dotenv.h"
using dotenv = _default;

std::shared_ptr<Promise<std::shared_ptr<RuntimeSettings>>> loadEnvConfig();

#endif
