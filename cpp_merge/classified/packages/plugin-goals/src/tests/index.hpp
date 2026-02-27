#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_GOALS_SRC_TESTS_INDEX_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_GOALS_SRC_TESTS_INDEX_TEST_H
#include "core.hpp"
#include "bun:test.h"
#include "../index.h"
// Using alias removed (invalid transpilation)
// External dependency removed

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
