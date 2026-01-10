#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-STAGEHAND_SRC___TESTS___E2E-BROWSER_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-STAGEHAND_SRC___TESTS___E2E-BROWSER_TEST_H
#include "core.h"
#include "bun:test.h"
#include "@elizaos/core.h"
#include "../service.js.h"
#include "../plugin.js.h"
#include "../process-manager.js.h"
#include "../websocket-client.js.h"

extern std::function<any()> createMockRuntime;
extern std::function<any(string)> createMemory;
extern std::shared_ptr<StagehandService> service;
extern std::shared_ptr<IAgentRuntime> runtime;
#endif
