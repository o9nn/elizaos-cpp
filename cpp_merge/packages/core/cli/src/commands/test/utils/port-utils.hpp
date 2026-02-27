#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_TEST_UTILS_PORT_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_TEST_UTILS_PORT_UTILS_H
#include "core.hpp"
#include "node:net.h"

std::shared_ptr<Promise<boolean>> checkPortAvailable(double port);

#endif
