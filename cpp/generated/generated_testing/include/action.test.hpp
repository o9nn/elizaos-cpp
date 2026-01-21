#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V1_SRC___TESTS___ACTION_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V1_SRC___TESTS___ACTION_TEST_H
#include "core.h"
#include "vitest.h"
#include "../action.h"
#include "../types.h"

class ActionV2;

extern std::function<std::any(double)> createTestUUID;
class ActionV2 : public object, public std::enable_shared_from_this<ActionV2> {
public:
    using std::enable_shared_from_this<ActionV2>::shared_from_this;
    std::string name;

    std::string description;

    array<string> similes;

    array<array<object>> examples;

    std::any handler;

    std::any validate;
};

#endif
