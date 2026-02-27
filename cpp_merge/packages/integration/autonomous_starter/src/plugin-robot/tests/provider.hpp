#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_ROBOT_TESTS_PROVIDER_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_ROBOT_TESTS_PROVIDER_TEST_H
#include "core.hpp"
#include "vitest.hpp"
#include "../provider.h"
#include "../service.h"
// External dependency removed
#include "../types.h"

extern object mockRobotService;
extern any mockRuntime;
extern std::function<any(string)> createMockMessage;
extern std::function<any(std::shared_ptr<Record<string, any>>)> createMockState;
extern std::function<std::shared_ptr<ScreenContext>(std::shared_ptr<Partial<std::shared_ptr<ScreenContext>>>)> createMockScreenContext;
#endif
