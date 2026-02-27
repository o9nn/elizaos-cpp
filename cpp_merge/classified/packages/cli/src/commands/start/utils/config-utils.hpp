#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_START_UTILS_CONFIG_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_START_UTILS_CONFIG_UTILS_H
#include "core.hpp"
// External dependency removed
// External dependency removed
#include "dotenv.hpp"
// Using alias removed (invalid transpilation)
#include "../../env/utils/file-operations.h"

std::shared_ptr<Promise<std::shared_ptr<RuntimeSettings>>> loadEnvConfig();

boolean hasCharacterSecrets(any character);

void ensureCharacterSettings(any character);

std::shared_ptr<Promise<any>> loadLocalEnvSecrets();

std::shared_ptr<Promise<boolean>> setDefaultSecretsFromEnv(any character);

#endif
