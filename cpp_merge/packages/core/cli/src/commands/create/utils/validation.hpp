#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_CREATE_UTILS_VALIDATION_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_CREATE_UTILS_VALIDATION_H
#include "core.hpp"
#include "node:fs.h"
#include "node:fs/promises.h"
// Using alias removed (invalid transpilation)
#include "zod.hpp"
#include "../types.h"
#include "../types.h"

extern any ProjectNameSchema;
extern any PluginNameSchema;
std::shared_ptr<CreateOptions> validateCreateOptions(any options);

object validateProjectName(string name);

object processPluginName(string name);

std::shared_ptr<Promise<object>> validateTargetDirectory(string targetDir);

#endif
