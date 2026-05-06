#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_ENV_UTILS_FILE_OPERATIONS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_ENV_UTILS_FILE_OPERATIONS_H
#include "core.hpp"
// External dependency removed
// Node.js module removed - use C++ filesystem
// Node.js module removed - use C++ filesystem
// Using alias removed (invalid transpilation)
#include "../types.h"

std::shared_ptr<Promise<string>> getGlobalEnvPath();

std::shared_ptr<Promise<any>> getLocalEnvPath();

std::shared_ptr<Promise<EnvVars>> parseEnvFile(string filePath);

std::shared_ptr<Promise<void>> writeEnvFile(string filePath, EnvVars envVars);

std::shared_ptr<Promise<boolean>> resetEnvFile(string filePath);

#endif
