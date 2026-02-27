#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_TEST_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_TEST_INDEX_H
#include "core.hpp"
// External dependency removed
// External dependency removed
// External dependency removed
#include "commander.hpp"
#include "./actions/run-all-tests.h"
#include "./actions/component-tests.h"
#include "./actions/e2e-tests.h"
#include "./types.h"
#include "./utils/project-utils.h"

extern any test;
any registerCommand(std::shared_ptr<Command> cli);

#endif
