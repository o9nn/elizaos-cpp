#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_ELIZAOS_SERVICES_SRC___TESTS___E2E_STARTER_PLUGIN_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_ELIZAOS_SERVICES_SRC___TESTS___E2E_STARTER_PLUGIN_H
#include "core.hpp"
// External dependency removed

typedef  UUID;

class TestSuite;
class Memory;
class State;

class TestSuite : public object, public std::enable_shared_from_this<TestSuite> {
public:
    using std::enable_shared_from_this<TestSuite>::shared_from_this;
    string name;

    string description;

    array<object> tests;
};

class Memory : public object, public std::enable_shared_from_this<Memory> {
public:
    using std::enable_shared_from_this<Memory>::shared_from_this;
    UUID entityId;

    UUID roomId;

    object content;
};

class State : public object, public std::enable_shared_from_this<State> {
public:
    using std::enable_shared_from_this<State>::shared_from_this;
    Record<string, any> values;

    Record<string, any> data;

    string text;
};

extern std::shared_ptr<TestSuite> StarterPluginTestSuite;
#endif
