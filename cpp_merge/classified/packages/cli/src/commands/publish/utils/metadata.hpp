#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_PUBLISH_UTILS_METADATA_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_PUBLISH_UTILS_METADATA_H
#include "core.hpp"
#include "../types.h"

std::shared_ptr<Promise<std::shared_ptr<PackageMetadata>>> generatePackageMetadata(std::shared_ptr<PackageJson> packageJson, string cliVersion, string username);

#endif
