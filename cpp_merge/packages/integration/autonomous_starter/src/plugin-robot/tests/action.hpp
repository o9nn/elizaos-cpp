#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-ROBOT_TESTS_ACTION_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-ROBOT_TESTS_ACTION_TEST_H
#include "core.h"
#include "vitest.h"
#include "../action.h"
#include "../types.h"
#include "../service.js.h"
#include "@elizaos/core.h"

extern std::shared_ptr<RobotService> mockRobotService;
extern any mockRuntime;
extern std::function<any(string)> createMockMessage;
extern std::function<any(std::shared_ptr<Record<string, any>>)> createMockState;
#endif
