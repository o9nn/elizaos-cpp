#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-PLUGIN-MANAGER_SRC___TESTS___E2E_TEST-UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-PLUGIN-MANAGER_SRC___TESTS___E2E_TEST-UTILS_H
#include "core.h"
#include "@elizaos/core.h"
#include "node:assert.h"
using assert = strict;
#include "uuid.h"
using uuid = v4;

std::shared_ptr<Promise<object>> setupScenario(std::shared_ptr<IAgentRuntime> runtime);

std::shared_ptr<Promise<std::shared_ptr<Content>>> sendMessageAndWaitForResponse(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Room> room, std::shared_ptr<Entity> user, string text);

#endif
