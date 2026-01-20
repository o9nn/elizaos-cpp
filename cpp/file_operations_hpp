#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_ENV_UTILS_FILE-OPERATIONS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_ENV_UTILS_FILE-OPERATIONS_H
#include "core.h"
#include "@/src/services/env-file.service.h"
#include "node:fs.h"
#include "node:path.h"
using path = _default;
#include "../types.h"

std::shared_ptr<Promise<string>> getGlobalEnvPath();

std::shared_ptr<Promise<any>> getLocalEnvPath();

std::shared_ptr<Promise<EnvVars>> parseEnvFile(string filePath);

std::shared_ptr<Promise<void>> writeEnvFile(string filePath, EnvVars envVars);

std::shared_ptr<Promise<boolean>> resetEnvFile(string filePath);

#endif
