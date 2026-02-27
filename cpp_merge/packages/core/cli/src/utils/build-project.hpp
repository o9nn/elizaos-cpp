#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_BUILD_PROJECT_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_BUILD_PROJECT_H
#include "core.hpp"
// Node.js module removed - use C++ filesystem
// Node.js module removed - use C++ filesystem
// External dependency removed
#include "execa.hpp"
#include "./directory-detection.h"
#include "./run-bun.h"

void buildProject(string cwd = process->cwd(), boolean isPlugin = false);

#endif
