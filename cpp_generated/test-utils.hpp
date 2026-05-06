#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-TEE-STARTER___TESTS___TEST-UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-TEE-STARTER___TESTS___TEST-UTILS_H
#include "core.h"
#include "bun:test.h"
#include "@elizaos/core.h"
#include "./utils/core-test-utils.h"
using createCoreMockRuntime = createMockRuntime;
using createCoreMockMessage = createMockMessage;
using createCoreMockState = createMockState;
#include "../src/character.h"
#include "../src/plugin.h"
using plugin = _default;

std::shared_ptr<IAgentRuntime> createMockRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides = object{});

std::shared_ptr<Memory> createMockMessage(string text, Partial<std::shared_ptr<Memory>> overrides = object{});

std::shared_ptr<State> createMockState(Partial<std::shared_ptr<State>> overrides = object{});

any setupTest(object options = object{});

any setupLoggerSpies();

#endif
