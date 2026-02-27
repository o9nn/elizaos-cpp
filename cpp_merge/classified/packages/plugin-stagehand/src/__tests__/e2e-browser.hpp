#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_STAGEHAND_SRC___TESTS___E2E_BROWSER_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_STAGEHAND_SRC___TESTS___E2E_BROWSER_TEST_H
#include "core.hpp"
#include "bun:test.h"
// External dependency removed
#include "../service.js.h"
#include "../plugin.js.h"
#include "../process-manager.js.h"
#include "../websocket-client.js.h"

extern std::function<any()> createMockRuntime;
extern std::function<any(string)> createMemory;
extern std::shared_ptr<StagehandService> service;
extern std::shared_ptr<IAgentRuntime> runtime;
#endif
