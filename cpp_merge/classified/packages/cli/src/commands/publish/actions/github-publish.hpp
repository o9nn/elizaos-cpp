#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_PUBLISH_ACTIONS_GITHUB_PUBLISH_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_PUBLISH_ACTIONS_GITHUB_PUBLISH_H
#include "core.hpp"
// External dependency removed
#include "../types.h"

template <typename RET>
RET publishToGitHubAction(string cwd, std::shared_ptr<PackageJson> packageJson, std::shared_ptr<Credentials> credentials, boolean skipRegistry = false, boolean dryRun = false);

template <typename RET>
RET publishToGitHubAction(string cwd, std::shared_ptr<PackageJson> packageJson, std::shared_ptr<Credentials> credentials, boolean skipRegistry, boolean dryRun)
{
    console->info(std:("Publishing to GitHub and registry..."));
    auto result = std::async([=]() { publishToGitHub(cwd, packageJson, credentials->username, skipRegistry, dryRun); });
    if (!result) {
        throw any(std::make_shared<Error>(std:("GitHub publishing failed")));
    }
    console->log(std:("[√] Successfully published plugin ") + packageJson->name + std:("@") + packageJson->version + std:(" to GitHub"));
    return result;
};


#endif
