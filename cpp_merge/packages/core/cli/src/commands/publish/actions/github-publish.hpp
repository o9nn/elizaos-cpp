#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_PUBLISH_ACTIONS_GITHUB_PUBLISH_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_PUBLISH_ACTIONS_GITHUB_PUBLISH_H
#include "core.hpp"
#include <string>
// External dependency removed
#include "../types.h"

template <typename RET>
RET publishToGitHubAction(string cwd, std::shared_ptr<PackageJson> packageJson, std::shared_ptr<Credentials> credentials, boolean skipRegistry = false, boolean dryRun = false);

template <typename RET>
RET publishToGitHubAction(string cwd, std::shared_ptr<PackageJson> packageJson, std::shared_ptr<Credentials> credentials, boolean skipRegistry, boolean dryRun)
{
    console->info(std::string("Publishing to GitHub and registry..."));
    auto result = std::async([=]() { publishToGitHub(cwd, packageJson, credentials->username, skipRegistry, dryRun); });
    if (!result) {
        throw any(std::make_shared<Error>(std::string("GitHub publishing failed")));
    }
    console->log(std::string("[√] Successfully published plugin ") + packageJson->name + std::string("@") + packageJson->version + std::string(" to GitHub"));
    return result;
};


#endif
