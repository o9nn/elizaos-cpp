#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_TEST_ACTIONS_RUN-ALL-TESTS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_TEST_ACTIONS_RUN-ALL-TESTS_H
#include "core.h"
#include "@elizaos/core.h"
#include "../types.h"
#include "../utils/project-utils.h"
#include "./component-tests.h"
#include "./e2e-tests.h"

std::shared_ptr<Promise<void>> runAllTests(any testPath, std::shared_ptr<TestCommandOptions> options);

#endif
