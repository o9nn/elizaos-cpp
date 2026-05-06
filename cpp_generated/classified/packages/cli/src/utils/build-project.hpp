#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_BUILD-PROJECT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_BUILD-PROJECT_H
#include "core.h"
#include "node:fs.h"
#include "node:path.h"
#include "@elizaos/core.h"
#include "./bun-exec.h"
#include "./directory-detection.h"
#include "./spinner-utils.h"
#include "yoctocolors.h"
using colors = _default;

void buildProject(string cwd = process->cwd(), boolean isPlugin = false);

#endif
