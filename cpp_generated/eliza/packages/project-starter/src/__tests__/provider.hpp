#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-STARTER_SRC___TESTS___PROVIDER_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-STARTER_SRC___TESTS___PROVIDER_TEST_H
#include "core.h"
#include "bun:test.h"
#include "../plugin.h"
using plugin = _default;
#include "@elizaos/core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;
#include "dotenv.h"
using dotenv = _default;

void documentTestResult(string testName, any result, any error = nullptr);

std::shared_ptr<IAgentRuntime> createRealRuntime();

std::shared_ptr<Memory> createRealMemory();

#endif
