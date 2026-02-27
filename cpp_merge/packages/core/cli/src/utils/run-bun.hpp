#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_RUN_BUN_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_RUN_BUN_H
#include "core.hpp"
#include "execa.hpp"
#include "./bun-installation-helper.h"

std::shared_ptr<Promise<void>> runBunCommand(array<string> args, string cwd);

#endif
