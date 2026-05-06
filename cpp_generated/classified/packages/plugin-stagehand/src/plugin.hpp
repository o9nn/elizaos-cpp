#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-STAGEHAND_SRC_PLUGIN_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-STAGEHAND_SRC_PLUGIN_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"
#include "zod.h"
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
