#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-ROBOT_TESTS_PROVIDER_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-ROBOT_TESTS_PROVIDER_TEST_H
#include "core.h"
#include "vitest.h"
#include "../provider.h"
#include "../service.h"
#include "@elizaos/core.h"
#include "../types.h"

extern object mockRobotService;
extern any mockRuntime;
extern std::function<any(string)> createMockMessage;
extern std::function<any(std::shared_ptr<Record<string, any>>)> createMockState;
extern std::function<std::shared_ptr<ScreenContext>(std::shared_ptr<Partial<std::shared_ptr<ScreenContext>>>)> createMockScreenContext;
#endif
