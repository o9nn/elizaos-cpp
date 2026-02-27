#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-DUMMY-SERVICES_SRC_E2E_TEST-UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-DUMMY-SERVICES_SRC_E2E_TEST-UTILS_H
#include "core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuid = v4;
#include "node:assert.h"
using assert = strict;

std::shared_ptr<Promise<object>> setupScenario(std::shared_ptr<IAgentRuntime> runtime);

std::shared_ptr<Promise<std::shared_ptr<Content>>> sendMessageAndWaitForResponse(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Room> room, std::shared_ptr<Entity> user, string text);

#endif
