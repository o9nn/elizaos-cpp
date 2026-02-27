#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PROJECT_TEE_STARTER___TESTS___PROVIDER_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PROJECT_TEE_STARTER___TESTS___PROVIDER_TEST_H
#include "core.hpp"
#include "bun:test.h"
#include "../src/plugin.h"
// Using alias removed (invalid transpilation)
// External dependency removed
// External dependency removed
#include "uuid.hpp"
using uuidv4 = v4;
#include "dotenv.hpp"
// Using alias removed (invalid transpilation)
#include "../src/plugin.h"
// Using alias removed (invalid transpilation)

void documentTestResult(string testName, any result, any error = nullptr);

std::shared_ptr<IAgentRuntime> createRealRuntime();

std::shared_ptr<Memory> createRealMemory();

#endif
