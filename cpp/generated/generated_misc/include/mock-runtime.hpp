#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CORE_SRC_TEST-UTILS_MOCK-RUNTIME_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CORE_SRC_TEST-UTILS_MOCK-RUNTIME_H
#include "core.h"
#include "../types.h"

Partial<std::shared_ptr<IAgentRuntime>> createMockRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides = object{});

std::shared_ptr<Memory> createMockMemory(string text, Partial<std::shared_ptr<Memory>> overrides = object{});

std::shared_ptr<State> createMockState(Partial<std::shared_ptr<State>> overrides = object{});

#endif
