#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V1_SRC___TESTS___STATE_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V1_SRC___TESTS___STATE_TEST_H
#include "core.hpp"
#include "vitest.hpp"
#include "../types.h"
#include "../state.h"
#include "../state.h"

class StateV2;

class StateV2 : public object, public std::enable_shared_from_this<StateV2> {
public:
    using std::enable_shared_from_this<StateV2>::shared_from_this;
    object values;

    object data;

    string text;
};

extern std::function<any(double)> createTestUUID;
extern array<object> emptyMemoryData;
#endif
