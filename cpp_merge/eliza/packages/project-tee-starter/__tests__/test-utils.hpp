#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PROJECT_TEE_STARTER___TESTS___TEST_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PROJECT_TEE_STARTER___TESTS___TEST_UTILS_H
#include "core.hpp"
#include "bun:test.h"
// External dependency removed
#include "./utils/core-test-utils.h"
using createCoreMockRuntime = createMockRuntime;
using createCoreMockMessage = createMockMessage;
using createCoreMockState = createMockState;
#include "../src/character.h"
#include "../src/plugin.h"
// Using alias removed (invalid transpilation)

std::shared_ptr<IAgentRuntime> createMockRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides = object{});

std::shared_ptr<Memory> createMockMessage(string text, Partial<std::shared_ptr<Memory>> overrides = object{});

std::shared_ptr<State> createMockState(Partial<std::shared_ptr<State>> overrides = object{});

any setupTest(object options = object{});

any setupLoggerSpies();

#endif
