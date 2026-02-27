#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_TEST_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_TEST_INDEX_H
#include "core.h"
#include "@/src/utils.h"
#include "@/src/utils/port-validation.h"
#include "@elizaos/core.h"
#include "commander.h"
#include "./actions/run-all-tests.h"
#include "./actions/component-tests.h"
#include "./actions/e2e-tests.h"
#include "./types.h"
#include "./utils/project-utils.h"

extern any test;
any registerCommand(std::shared_ptr<Command> cli);

#endif
