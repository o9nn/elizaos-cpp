#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_BUN_INSTALLATION_HELPER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_BUN_INSTALLATION_HELPER_H
#include "core.hpp"
// External dependency removed
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
