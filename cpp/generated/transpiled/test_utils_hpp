#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-BOOTSTRAP_SRC___TESTS___TEST-UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-BOOTSTRAP_SRC___TESTS___TEST-UTILS_H
#include "core.h"
#include "bun:test.h"
#include "@elizaos/core.h"

typedef any MockRuntime;


MockRuntime createMockRuntime(Partial<MockRuntime> overrides = object{});

Partial<std::shared_ptr<Memory>> createMockMemory(Partial<std::shared_ptr<Memory>> overrides = object{});

Partial<std::shared_ptr<State>> createMockState(Partial<std::shared_ptr<State>> overrides = object{});

any createMockService(Partial<Record<string, any>> overrides = object{});

any setupActionTest(object options = undefined);

#endif
