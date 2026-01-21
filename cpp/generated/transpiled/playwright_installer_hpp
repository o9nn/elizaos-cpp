#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-STAGEHAND_STAGEHAND-SERVER_SRC_PLAYWRIGHT-INSTALLER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-STAGEHAND_STAGEHAND-SERVER_SRC_PLAYWRIGHT-INSTALLER_H
#include "core.h"
#include "child_process.h"
#include "fs.h"
#include "path.h"
#include "./logger.js.h"

class PlaywrightInstaller;

class PlaywrightInstaller : public object, public std::enable_shared_from_this<PlaywrightInstaller> {
public:
    using std::enable_shared_from_this<PlaywrightInstaller>::shared_from_this;
    std::shared_ptr<Logger> logger;

    boolean isInstalling = false;

    any installPromise = nullptr;

    PlaywrightInstaller(std::shared_ptr<Logger> logger);
    virtual boolean isPlaywrightInstalled();
    virtual std::shared_ptr<Promise<void>> installPlaywright();
    virtual std::shared_ptr<Promise<void>> ensurePlaywrightInstalled();
    virtual boolean isReady();
};

#endif
