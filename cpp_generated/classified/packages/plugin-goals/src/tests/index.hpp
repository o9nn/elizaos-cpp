#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-GOALS_SRC_TESTS_INDEX_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-GOALS_SRC_TESTS_INDEX_TEST_H
#include "core.h"
#include "bun:test.h"
#include "../index.h"
using GoalsPlugin = _default;
#include "@elizaos/core.h"

class TestSuite;

class TestSuite : public object, public std::enable_shared_from_this<TestSuite> {
public:
    using std::enable_shared_from_this<TestSuite>::shared_from_this;
    string name;

    any config;

    TestSuite(string name_, any config_);
    virtual void addTest(any test);
    virtual void run();
};

extern std::function<object(object)> createUnitTest;
#endif
