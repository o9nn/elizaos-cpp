#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_PACKAGE_MANAGER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_PACKAGE_MANAGER_H
#include "core.hpp"
// External dependency removed
#include "./user-environment.h"
#include "./bun-installation-helper.h"
#include "./run-bun.h"
// Node.js module removed - use C++ filesystem
// Node.js module removed - use C++ filesystem
// Using alias removed (invalid transpilation)
#include "execa.hpp"

std::shared_ptr<Promise<string>> getPackageManager();

std::shared_ptr<Promise<boolean>> isGlobalInstallation();

std::shared_ptr<Promise<boolean>> isRunningViaNpx();

std::shared_ptr<Promise<boolean>> isRunningViaBunx();

array<string> getInstallCommand(boolean isGlobal);

std::shared_ptr<Promise<void>> removeFromBunLock(string packageName, string directory);

std::shared_ptr<Promise<object>> executeInstallation(string packageName, string versionOrTag = string_empty, string directory = process->cwd());

string buildGitHubSpecifier(string githubSpec, string versionOrTag = std::nullopt);

std::shared_ptr<Promise<object>> executeInstallationWithFallback(string packageName, string versionOrTag = string_empty, string directory = process->cwd(), string githubFallback = std::nullopt);

#endif
