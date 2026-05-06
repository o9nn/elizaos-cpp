#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_ENV_PROMPT_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_ENV_PROMPT_H
#include "core.hpp"
// External dependency removed
// External dependency removed

std::shared_ptr<Promise<string>> getEnvFilePath();

std::shared_ptr<Promise<Record<string, string>>> readEnvFile();

std::shared_ptr<Promise<void>> writeEnvFile(Record<string, string> envVars);

#endif
