#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_ELIZA-PATHS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_ELIZA-PATHS_H
#include "core.h"
#include "node:path.h"
using path = _default;

std::string getElizaBaseDir(std::string cwd = process->cwd());

std::string getElizaDbDir(std::string cwd = process->cwd());

std::string getElizaDataDir(std::string cwd = process->cwd());

std::string getElizaUploadsDir(std::string cwd = process->cwd());

std::string getElizaGeneratedDir(std::string cwd = process->cwd());

std::string getElizaCharactersDir(std::string cwd = process->cwd());

#endif
