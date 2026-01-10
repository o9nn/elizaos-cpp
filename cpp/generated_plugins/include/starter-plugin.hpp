#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-STARTER_SRC___TESTS___E2E_STARTER-PLUGIN_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-STARTER_SRC___TESTS___E2E_STARTER-PLUGIN_H
#include "core.h"
#include "@elizaos/core.h"

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
