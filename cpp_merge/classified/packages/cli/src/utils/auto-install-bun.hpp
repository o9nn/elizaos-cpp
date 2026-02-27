#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_AUTO_INSTALL_BUN_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_AUTO_INSTALL_BUN_H
#include "core.hpp"
// External dependency removed
#include "./bun-exec.h"
// Node.js module removed - use C++ filesystem
// Node.js module removed - use C++ filesystem

extern double INSTALLATION_VERIFICATION_DELAY_MS;
void updatePathForBun();

std::shared_ptr<Promise<boolean>> isBunInstalled();

std::shared_ptr<Promise<boolean>> autoInstallBun();

boolean shouldAutoInstall();

#endif
