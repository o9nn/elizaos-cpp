#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_TESTING_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_TESTING_H
#include "core.h"
#include "./runtime.h"

class TestCase;
class TestSuite;

class TestCase : public object, public std::enable_shared_from_this<TestCase> {
public:
    using std::enable_shared_from_this<TestCase>::shared_from_this;
    string name;

    std::function<any(std::shared_ptr<IAgentRuntime>)> fn;
};

class TestSuite : public object, public std::enable_shared_from_this<TestSuite> {
public:
    using std::enable_shared_from_this<TestSuite>::shared_from_this;
    string name;

    array<std::shared_ptr<TestCase>> tests;
};

#endif
