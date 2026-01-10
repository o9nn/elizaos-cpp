#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-STARTER_SRC___TESTS___MODELS_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-STARTER_SRC___TESTS___MODELS_TEST_H
#include "core.h"
#include "bun:test.h"
#include "../plugin.h"
using plugin = _default;
#include "@elizaos/core.h"
#include "@elizaos/core.h"
#include "dotenv.h"
using dotenv = _default;
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
