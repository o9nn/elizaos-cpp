#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PROJECT_STARTER_SRC___TESTS___MODELS_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PROJECT_STARTER_SRC___TESTS___MODELS_TEST_H
#include "core.hpp"
#include "bun:test.h"
#include "../plugin.h"
// Using alias removed (invalid transpilation)
// External dependency removed
// External dependency removed
#include "dotenv.hpp"
// Using alias removed (invalid transpilation)
#include "./utils/core-test-utils.h"

class TestGenerateParams;

class TestGenerateParams : public object, public std::enable_shared_from_this<TestGenerateParams> {
public:
    using std::enable_shared_from_this<TestGenerateParams>::shared_from_this;
    string prompt;

    array<string> stopSequences;

    double maxTokens;

    double temperature;

    double frequencyPenalty;

    double presencePenalty;
};

extern std::function<std::shared_ptr<Promise<object>>(any, std::function<std::shared_ptr<Promise<string>>(any, std::shared_ptr<TestGenerateParams>)>)> runCoreModelTests;
#endif
