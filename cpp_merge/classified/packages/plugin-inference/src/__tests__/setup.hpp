#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_INFERENCE_SRC___TESTS___SETUP_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_INFERENCE_SRC___TESTS___SETUP_H
#include "core.hpp"
#include "vitest.hpp"
// External dependency removed

class MockRuntime;

class MockRuntime : public object, public std::enable_shared_from_this<MockRuntime> {
public:
    using std::enable_shared_from_this<MockRuntime>::shared_from_this;
    std::function<any(string)> getSetting;
};

#endif
