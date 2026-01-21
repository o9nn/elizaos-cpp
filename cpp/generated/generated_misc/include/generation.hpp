#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-ENV_GENERATION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-ENV_GENERATION_H
#include "core.h"
#include "@elizaos/core.h"
#include "./types.h"

extern object generationTemplates;
extern object generationDependencies;
boolean canGenerateEnvVar(std::string varName, std::string type, std::string description = undefined);

std::any generateScript(std::string varName, std::string type, std::string pluginName, std::string description = undefined);

std::string getGenerationDescription(std::string varName, std::string type);

#endif
