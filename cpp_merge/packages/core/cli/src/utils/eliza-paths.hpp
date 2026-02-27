#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_ELIZA_PATHS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_ELIZA_PATHS_H
#include "core.hpp"
#include "node:path.h"
// Using alias removed (invalid transpilation)

string getElizaBaseDir(string cwd = process->cwd());

string getElizaDbDir(string cwd = process->cwd());

string getElizaDataDir(string cwd = process->cwd());

string getElizaUploadsDir(string cwd = process->cwd());

string getElizaGeneratedDir(string cwd = process->cwd());

string getElizaCharactersDir(string cwd = process->cwd());

#endif
