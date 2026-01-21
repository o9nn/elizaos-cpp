#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-BOOTSTRAP_SRC___TESTS___TEST-UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-BOOTSTRAP_SRC___TESTS___TEST-UTILS_H
#include "core.h"
#include "bun:test.h"
#include "@elizaos/core.h"

typedef std::any MockRuntime;


MockRuntime createMockRuntime(Partial<MockRuntime> overrides = object{});

Partial<std::shared_ptr<Memory>> createMockMemory(Partial<std::shared_ptr<Memory>> overrides = object{});

Partial<std::shared_ptr<State>> createMockState(Partial<std::shared_ptr<State>> overrides = object{});

std::any createMockService(Partial<Record<std::string, any>> overrides = object{});

std::any setupActionTest(object options = undefined);

#endif
