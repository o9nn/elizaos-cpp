#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_TEST_ACTIONS_RUN_ALL_TESTS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_TEST_ACTIONS_RUN_ALL_TESTS_H
#include "core.hpp"
// External dependency removed
#include "../types.h"
#include "../utils/project-utils.h"
#include "./component-tests.h"
#include "./e2e-tests.h"

std::shared_ptr<Promise<void>> runAllTests(any testPath, std::shared_ptr<TestCommandOptions> options);

#endif
