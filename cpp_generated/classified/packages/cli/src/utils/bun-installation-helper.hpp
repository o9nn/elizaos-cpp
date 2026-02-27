#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_BUN-INSTALLATION-HELPER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_BUN-INSTALLATION-HELPER_H
#include "core.h"
#include "@elizaos/core.h"
#include "./emoji-handler.h"
#include "./bun-exec.h"

class BunInstallationResult;

class BunInstallationResult : public object, public std::enable_shared_from_this<BunInstallationResult> {
public:
    using std::enable_shared_from_this<BunInstallationResult>::shared_from_this;
    boolean installed;

    string message;

    string error;
};

std::shared_ptr<Promise<std::shared_ptr<BunInstallationResult>>> checkBunInstallation();

void displayBunInstallInstructions();

string displayBunInstallationTipCompact();

#endif
