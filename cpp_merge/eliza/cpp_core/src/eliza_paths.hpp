#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_ELIZA_PATHS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_ELIZA_PATHS_H
#include "core.hpp"
#include <string>
// Node.js module removed - use C++ filesystem
// Using alias removed (invalid transpilation)

std::string getElizaBaseDir(std::string cwd = process->cwd());

std::string getElizaDbDir(std::string cwd = process->cwd());

std::string getElizaDataDir(std::string cwd = process->cwd());

std::string getElizaUploadsDir(std::string cwd = process->cwd());

std::string getElizaGeneratedDir(std::string cwd = process->cwd());

std::string getElizaCharactersDir(std::string cwd = process->cwd());

#endif
