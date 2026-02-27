#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_ROBOT_TESTS_ACTION_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_ROBOT_TESTS_ACTION_TEST_H
#include "core.hpp"
#include "vitest.hpp"
#include "../action.h"
#include "../types.h"
#include "../service.js.h"
// External dependency removed

extern std::shared_ptr<RobotService> mockRobotService;
extern any mockRuntime;
extern std::function<any(string)> createMockMessage;
extern std::function<any(std::shared_ptr<Record<string, any>>)> createMockState;
#endif
