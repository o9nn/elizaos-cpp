#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_HANDLE-ERROR_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_HANDLE-ERROR_H
#include "core.h"
#include "@elizaos/core.h"
#include "../commands/agent.h"
#include "commander.h"
#include "yoctocolors.h"
using colors = _default;

void handleError(any error);

void checkServer(std::shared_ptr<OptionValues> opts);

#endif
