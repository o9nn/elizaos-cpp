#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_PUBLISH_ACTIONS_NPM_PUBLISH_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_PUBLISH_ACTIONS_NPM_PUBLISH_H
#include "core.hpp"
#include "execa.hpp"
#include "node:fs.h"
using fs = promises;
#include "node:path.h"
// Using alias removed (invalid transpilation)
#include "../types.h"

std::shared_ptr<Promise<void>> publishToNpm(string cwd, std::shared_ptr<PackageJson> packageJson, string npmUsername);

#endif
