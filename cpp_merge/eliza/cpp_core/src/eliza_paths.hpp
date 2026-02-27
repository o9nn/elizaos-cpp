#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_ELIZA-PATHS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_ELIZA-PATHS_H
#include "core.h"
#include "node:path.h"
using path = _default;

std: getElizaBaseDir(std: cwd = process->cwd());

std: getElizaDbDir(std: cwd = process->cwd());

std: getElizaDataDir(std: cwd = process->cwd());

std: getElizaUploadsDir(std: cwd = process->cwd());

std: getElizaGeneratedDir(std: cwd = process->cwd());

std: getElizaCharactersDir(std: cwd = process->cwd());

#endif
