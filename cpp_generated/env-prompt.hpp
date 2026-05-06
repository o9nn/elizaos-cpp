#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_ENV-PROMPT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_ENV-PROMPT_H
#include "core.h"
#include "@/src/services/env-file.service.h"
#include "@elizaos/core.h"

std::shared_ptr<Promise<string>> getEnvFilePath();

std::shared_ptr<Promise<Record<string, string>>> readEnvFile();

std::shared_ptr<Promise<void>> writeEnvFile(Record<string, string> envVars);

#endif
