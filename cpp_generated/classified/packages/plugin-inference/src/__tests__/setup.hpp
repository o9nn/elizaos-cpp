#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-INFERENCE_SRC___TESTS___SETUP_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-INFERENCE_SRC___TESTS___SETUP_H
#include "core.h"
#include "vitest.h"
#include "@elizaos/core.h"

class MockRuntime;

class MockRuntime : public object, public std::enable_shared_from_this<MockRuntime> {
public:
    using std::enable_shared_from_this<MockRuntime>::shared_from_this;
    std::function<any(string)> getSetting;
};

#endif
