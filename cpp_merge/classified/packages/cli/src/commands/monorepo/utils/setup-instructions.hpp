#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_MONOREPO_UTILS_SETUP_INSTRUCTIONS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_MONOREPO_UTILS_SETUP_INSTRUCTIONS_H
#include "core.hpp"
// Node.js module removed - use C++ filesystem
// Using alias removed (invalid transpilation)
// External dependency removed
#include "../types.h"

std::shared_ptr<PlatformInstructions> getBunInstallInstructions();

void displayBasicSteps(string cdPath);

void displayPrerequisites();

void displayBunInstructions();

void displayNextSteps(string targetDir);

#endif
