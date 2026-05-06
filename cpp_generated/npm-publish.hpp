#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_PUBLISH_ACTIONS_NPM-PUBLISH_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_PUBLISH_ACTIONS_NPM-PUBLISH_H
#include "core.h"
#include "execa.h"
#include "node:fs.h"
using fs = promises;
#include "node:path.h"
using path = _default;
#include "../types.h"

std::shared_ptr<Promise<void>> publishToNpm(string cwd, std::shared_ptr<PackageJson> packageJson, string npmUsername);

#endif
