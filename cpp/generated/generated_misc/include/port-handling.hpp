#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_PORT-HANDLING_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_PORT-HANDLING_H
#include "core.h"
#include "node:net.h"
using net = _default;

std::shared_ptr<Promise<boolean>> isPortFree(double port);

std::shared_ptr<Promise<double>> findNextAvailablePort(double startPort);

#endif
