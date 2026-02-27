#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CORE_SRC_TEST_UTILS_MOCK_RUNTIME_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CORE_SRC_TEST_UTILS_MOCK_RUNTIME_H
#include "core.hpp"
#include "../types.h"

Partial<std::shared_ptr<IAgentRuntime>> createMockRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides = object{});

std::shared_ptr<Memory> createMockMemory(string text, Partial<std::shared_ptr<Memory>> overrides = object{});

std::shared_ptr<State> createMockState(Partial<std::shared_ptr<State>> overrides = object{});

#endif
