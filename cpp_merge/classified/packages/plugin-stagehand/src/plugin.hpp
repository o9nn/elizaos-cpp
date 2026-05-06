#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_STAGEHAND_SRC_PLUGIN_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_STAGEHAND_SRC_PLUGIN_H
#include "core.hpp"
// External dependency removed
// External dependency removed
#include "zod.hpp"
#include "./service.js.h"
#include "./errors.js.h"
#include "./retry.js.h"
#include "./security.js.h"

extern any configSchema;
any extractUrl(string text);

extern std::shared_ptr<Action> browserNavigateAction;
extern std::shared_ptr<Action> browserClickAction;
extern std::shared_ptr<Action> browserTypeAction;
extern std::shared_ptr<Action> browserSelectAction;
extern std::shared_ptr<Action> browserExtractAction;
extern std::shared_ptr<Action> browserScreenshotAction;
extern std::shared_ptr<Provider> browserStateProvider;
std::shared_ptr<Promise<void>> testStagehandConnection(std::shared_ptr<IAgentRuntime> runtime);

extern std::shared_ptr<Plugin> stagehandPlugin;
#endif
