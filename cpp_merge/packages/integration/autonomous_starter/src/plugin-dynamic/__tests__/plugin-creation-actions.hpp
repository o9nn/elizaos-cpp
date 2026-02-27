#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_DYNAMIC___TESTS___PLUGIN_CREATION_ACTIONS_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_DYNAMIC___TESTS___PLUGIN_CREATION_ACTIONS_TEST_H
#include "core.hpp"
#include "vitest.hpp"
#include "../actions/plugin-creation-actions.h"
// External dependency removed
#include "../services/plugin-creation-service.h"

extern std::function<any(string)> createMockMemory;
extern std::function<any()> createMockRuntime;
#endif
