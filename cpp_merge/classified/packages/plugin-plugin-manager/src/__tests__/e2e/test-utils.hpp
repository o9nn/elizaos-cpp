#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_PLUGIN_MANAGER_SRC___TESTS___E2E_TEST_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_PLUGIN_MANAGER_SRC___TESTS___E2E_TEST_UTILS_H
#include "core.hpp"
// External dependency removed
// Node.js module removed - use C++ filesystem
using assert = strict;
#include "uuid.hpp"
using uuid = v4;

std::shared_ptr<Promise<object>> setupScenario(std::shared_ptr<IAgentRuntime> runtime);

std::shared_ptr<Promise<std::shared_ptr<Content>>> sendMessageAndWaitForResponse(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Room> room, std::shared_ptr<Entity> user, string text);

#endif
