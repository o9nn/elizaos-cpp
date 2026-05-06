#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_PUBLISH_UTILS_METADATA_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_PUBLISH_UTILS_METADATA_H
#include "core.h"
#include "../types.h"

std::shared_ptr<Promise<std::shared_ptr<PackageMetadata>>> generatePackageMetadata(std::shared_ptr<PackageJson> packageJson, string cliVersion, string username);

#endif
