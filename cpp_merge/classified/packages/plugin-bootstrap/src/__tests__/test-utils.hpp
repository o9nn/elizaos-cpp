#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_BOOTSTRAP_SRC___TESTS___TEST_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_BOOTSTRAP_SRC___TESTS___TEST_UTILS_H
#include "core.hpp"
#include "bun:test.h"
// External dependency removed

typedef any MockRuntime;


MockRuntime createMockRuntime(Partial<MockRuntime> overrides = object{});

Partial<std::shared_ptr<Memory>> createMockMemory(Partial<std::shared_ptr<Memory>> overrides = object{});

Partial<std::shared_ptr<State>> createMockState(Partial<std::shared_ptr<State>> overrides = object{});

any createMockService(Partial<Record<string, any>> overrides = object{});

any setupActionTest(object options = std::nullopt);

#endif
