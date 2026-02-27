#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_ELIZAOS_SERVICES_SRC___TESTS___TEST_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_ELIZAOS_SERVICES_SRC___TESTS___TEST_UTILS_H
#include "core.hpp"
#include "bun:test.h"
// External dependency removed

class MockRuntime;

std::shared_ptr<MockRuntime> createMockRuntime(Partial<std::shared_ptr<MockRuntime>> overrides = object{});

Partial<std::shared_ptr<Memory>> createMockMemory(Partial<std::shared_ptr<Memory>> overrides = object{});

Partial<std::shared_ptr<State>> createMockState(Partial<std::shared_ptr<State>> overrides = object{});

any setupTest(object overrides = object{});

class MockRuntime : public object, public std::enable_shared_from_this<MockRuntime> {
public:
    using std::enable_shared_from_this<MockRuntime>::shared_from_this;
    std::shared_ptr<UUID> agentId;

    object character;

    std::shared_ptr<Map<string, std::shared_ptr<Service>>> services;

    ReturnType<mock> getService;

    ReturnType<mock> registerService;

    ReturnType<mock> getSetting;

    ReturnType<mock> useModel;

    ReturnType<mock> init;
};

any setupLoggerSpies();

#endif
