#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_START_UTILS_CONFIG-UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_START_UTILS_CONFIG-UTILS_H
#include "core.h"
#include "@/src/utils.h"
#include "@elizaos/core.h"
#include "dotenv.h"
using dotenv = _default;
#include "../../env/utils/file-operations.h"

std::shared_ptr<Promise<std::shared_ptr<RuntimeSettings>>> loadEnvConfig();

boolean hasCharacterSecrets(any character);

void ensureCharacterSettings(any character);

std::shared_ptr<Promise<any>> loadLocalEnvSecrets();

std::shared_ptr<Promise<boolean>> setDefaultSecretsFromEnv(any character);

#endif
