#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_SHELL_SRC_TESTS_TEST_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_SHELL_SRC_TESTS_TEST_UTILS_H
#include "core.hpp"
#include "bun:test.h"
// External dependency removed

std::shared_ptr<IAgentRuntime> createMockRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides = object{});

std::shared_ptr<Memory> createMockMemory(Partial<std::shared_ptr<Memory>> overrides = object{});

std::shared_ptr<State> createMockState(Partial<std::shared_ptr<State>> overrides = object{});

#endif
