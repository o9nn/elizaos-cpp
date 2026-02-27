#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_STAGEHAND_STAGEHAND_SERVER_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_STAGEHAND_STAGEHAND_SERVER_SRC_INDEX_H
#include "core.hpp"
#include "ws.hpp"
// External dependency removed
#include "dotenv.hpp"
#include "./session-manager.js.h"
#include "./message-handler.js.h"
#include "./logger.js.h"
#include "./playwright-installer.js.h"

extern any PORT;
extern std::shared_ptr<Logger> logger;
extern std::shared_ptr<PlaywrightInstaller> playwrightInstaller;
void startServer();

#endif
