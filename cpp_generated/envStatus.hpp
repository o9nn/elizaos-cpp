#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-ENV_PROVIDERS_ENVSTATUS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-ENV_PROVIDERS_ENVSTATUS_H
#include "core.h"
#include "@elizaos/core.h"
#include "../types.h"

string formatEnvVarStatus(std::shared_ptr<EnvVarConfig> config, boolean showValues = false);

string generateEnvStatusMessage(std::shared_ptr<EnvVarMetadata> envVars, boolean showValues = false);

extern std::shared_ptr<Provider> envStatusProvider;
#endif
