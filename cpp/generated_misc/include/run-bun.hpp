#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_RUN-BUN_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_RUN-BUN_H
#include "core.h"
#include "execa.h"
#include "./bun-installation-helper.h"

std::shared_ptr<Promise<void>> runBunCommand(array<string> args, string cwd);

#endif
