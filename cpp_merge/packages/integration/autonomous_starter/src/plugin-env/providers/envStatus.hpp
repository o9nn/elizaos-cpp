#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_ENV_PROVIDERS_ENVSTATUS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_ENV_PROVIDERS_ENVSTATUS_H
#include "core.hpp"
// External dependency removed
#include "../types.h"

string formatEnvVarStatus(std::shared_ptr<EnvVarConfig> config, boolean showValues = false);

string generateEnvStatusMessage(std::shared_ptr<EnvVarMetadata> envVars, boolean showValues = false);

extern std::shared_ptr<Provider> envStatusProvider;
#endif
