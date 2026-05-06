#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-STAGEHAND_STAGEHAND-SERVER_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-STAGEHAND_STAGEHAND-SERVER_SRC_INDEX_H
#include "core.h"
#include "ws.h"
#include "@browserbasehq/stagehand.h"
#include "dotenv.h"
#include "./session-manager.js.h"
#include "./message-handler.js.h"
#include "./logger.js.h"
#include "./playwright-installer.js.h"

extern any PORT;
extern std::shared_ptr<Logger> logger;
extern std::shared_ptr<PlaywrightInstaller> playwrightInstaller;
void startServer();

#endif
