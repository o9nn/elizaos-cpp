#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_ELIZA-PATHS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_ELIZA-PATHS_H
#include "core.h"
#include "node:path.h"
using path = _default;

string getElizaBaseDir(string cwd = process->cwd());

string getElizaDbDir(string cwd = process->cwd());

string getElizaDataDir(string cwd = process->cwd());

string getElizaUploadsDir(string cwd = process->cwd());

string getElizaGeneratedDir(string cwd = process->cwd());

string getElizaCharactersDir(string cwd = process->cwd());

#endif
