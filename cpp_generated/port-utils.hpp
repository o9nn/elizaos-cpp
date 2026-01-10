#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_TEST_UTILS_PORT-UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_TEST_UTILS_PORT-UTILS_H
#include "core.h"
#include "node:net.h"

std::shared_ptr<Promise<boolean>> checkPortAvailable(double port);

#endif
