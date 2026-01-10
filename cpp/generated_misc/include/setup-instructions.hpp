#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_MONOREPO_UTILS_SETUP-INSTRUCTIONS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_MONOREPO_UTILS_SETUP-INSTRUCTIONS_H
#include "core.h"
#include "node:path.h"
using path = _default;
#include "@/src/utils/emoji-handler.h"
#include "../types.h"

std::shared_ptr<PlatformInstructions> getBunInstallInstructions();

void displayBasicSteps(string cdPath);

void displayPrerequisites();

void displayBunInstructions();

void displayNextSteps(string targetDir);

#endif
