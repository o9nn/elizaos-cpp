#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_PACKAGE-MANAGER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_PACKAGE-MANAGER_H
#include "core.h"
#include "@elizaos/core.h"
#include "./user-environment.h"
#include "./bun-installation-helper.h"
#include "./run-bun.h"
#include "node:fs.h"
#include "node:path.h"
using path = _default;
#include "execa.h"

std::shared_ptr<Promise<string>> getPackageManager();

std::shared_ptr<Promise<boolean>> isGlobalInstallation();

std::shared_ptr<Promise<boolean>> isRunningViaNpx();

std::shared_ptr<Promise<boolean>> isRunningViaBunx();

array<string> getInstallCommand(boolean isGlobal);

std::shared_ptr<Promise<void>> removeFromBunLock(string packageName, string directory);

std::shared_ptr<Promise<object>> executeInstallation(string packageName, string versionOrTag = string_empty, string directory = process->cwd());

string buildGitHubSpecifier(string githubSpec, string versionOrTag = undefined);

std::shared_ptr<Promise<object>> executeInstallationWithFallback(string packageName, string versionOrTag = string_empty, string directory = process->cwd(), string githubFallback = undefined);

#endif
