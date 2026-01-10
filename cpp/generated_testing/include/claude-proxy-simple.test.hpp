#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-INFERENCE_SRC___TESTS___CLAUDE-PROXY-SIMPLE_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-INFERENCE_SRC___TESTS___CLAUDE-PROXY-SIMPLE_TEST_H
#include "core.h"
#include "bun:test.h"
#include "@elizaos/core.h"
#include "../claude-proxy.h"

class MockRuntime;

class MockRuntime : public object, public std::enable_shared_from_this<MockRuntime> {
public:
    using std::enable_shared_from_this<MockRuntime>::shared_from_this;
    std::function<any(string)> getSetting;

    std::function<std::shared_ptr<Promise<string>>(string, any)> useModel;
};

#endif
