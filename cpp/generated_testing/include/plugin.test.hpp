#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-STARTER_SRC___TESTS___PLUGIN_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-STARTER_SRC___TESTS___PLUGIN_TEST_H
#include "core.h"
#include "bun:test.h"
#include "../plugin.h"
using plugin = _default;
#include "@elizaos/core.h"
#include "../plugin.h"
#include "dotenv.h"
using dotenv = _default;

void documentTestResult(string testName, any result, any error = nullptr);

any createRealRuntime();

#endif
