#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V1_SRC___TESTS___ACTION_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V1_SRC___TESTS___ACTION_TEST_H
#include "core.hpp"
#include "vitest.hpp"
#include "../action.h"
#include "../types.h"

class ActionV2;

extern std::function<any(double)> createTestUUID;
class ActionV2 : public object, public std::enable_shared_from_this<ActionV2> {
public:
    using std::enable_shared_from_this<ActionV2>::shared_from_this;
    string name;

    string description;

    array<string> similes;

    array<array<object>> examples;

    any handler;

    any validate;
};

#endif
