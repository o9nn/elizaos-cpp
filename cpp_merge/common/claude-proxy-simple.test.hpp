#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_INFERENCE_SRC___TESTS___CLAUDE_PROXY_SIMPLE_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_INFERENCE_SRC___TESTS___CLAUDE_PROXY_SIMPLE_TEST_H
#include "core.hpp"
#include "bun:test.h"
// External dependency removed
#include "../claude-proxy.h"

class MockRuntime;

class MockRuntime : public object, public std::enable_shared_from_this<MockRuntime> {
public:
    using std::enable_shared_from_this<MockRuntime>::shared_from_this;
    std::function<any(string)> getSetting;

    std::function<std::shared_ptr<Promise<string>>(string, any)> useModel;
};

#endif
