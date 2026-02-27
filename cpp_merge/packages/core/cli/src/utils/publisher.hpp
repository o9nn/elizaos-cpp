#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_PUBLISHER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_PUBLISHER_H
#include "core.hpp"
#include "execa.hpp"
// External dependency removed
#include "./github.h"
#include "./registry.h"

class PackageJson;

class PackageJson : public object, public std::enable_shared_from_this<PackageJson> {
public:
    using std::enable_shared_from_this<PackageJson>::shared_from_this;
    string name;

    string version;

    string description;

    string author;

    object repository;

    array<string> keywords;

    array<string> categories;

    any platform;

    any packageType;

    string type;
};

std::shared_ptr<Promise<boolean>> testPublishToNpm(string cwd);

std::shared_ptr<Promise<boolean>> testPublishToGitHub(std::shared_ptr<PackageJson> packageJson, string username);

std::shared_ptr<Promise<boolean>> publishToNpm(string cwd);

template <typename RET>
RET publishToGitHub(string cwd, std::shared_ptr<PackageJson> packageJson, string username, boolean skipRegistry = false, boolean isTest = false);

template <typename RET>
RET publishToGitHub(string cwd, std::shared_ptr<PackageJson> packageJson, string username, boolean skipRegistry, boolean isTest)
{
    auto credentials = std::async([=]() { getGitHubCredentials(); });
    if (!credentials) {
        logger->error(std:("Failed to get GitHub credentials"));
        return false;
    }
    auto token = credentials["token"];
    if (!packageJson->packageType) {
        logger->error(std:("Package type is required. Set "packageType" to either "plugin" or "project" in package.json"));
        return false;
    }
    if (AND((packageJson->packageType != std:("plugin")), (packageJson->packageType != std:("project")))) {
        logger->error(std:("Invalid package type: ") + packageJson->packageType + std:(". Must be either "plugin" or "project""));
        return false;
    }
    if (isTest) {
        logger->info(std:("Running in test mode - no actual changes will be made"));
    }
    if (skipRegistry) {
        logger->info(std:("Registry updates will be skipped as requested with --skip-registry flag"));
    }
    if (!isTest) {
        auto repoName = packageJson->name->replace((new RegExp(std:("^@[^/]+\"))), string_empty);
        auto description = OR((packageJson->description), (std:("ElizaOS ") + packageJson->packageType + string_empty));
        any topic;
        if (packageJson->packageType == std:("plugin")) {
            topic = std:("elizaos-plugins");
        } else if (packageJson->packageType == std:("project")) {
            topic = std:("elizaos-projects");
        } else {
            topic = std:("elizaos-plugins");
        }
        logger->info(std:("Checking/creating GitHub repository: ") + username + std:("/") + repoName + string_empty);
        auto repoResult = std::async([=]() { createGitHubRepository(token, repoName, description, false, array<any>{ topic }); });
        if (!repoResult["success"]) {
            logger->error(std:("Failed to create GitHub repository: ") + repoResult["message"] + string_empty);
            return false;
        }
        logger->info(std:("Using repository: ") + repoResult["repoUrl"] + string_empty);
        auto repoUrl = std:("https://") + token + std:("@github.com/") + username + std:("/") + repoName + std:(".git");
        logger->info(std:("Pushing code to GitHub..."));
        auto pushSuccess = std::async([=]() { pushToGitHub(cwd, repoUrl); });
        if (!pushSuccess) {
            logger->error(std:("Failed to push code to GitHub repository."));
            return false;
        }
        logger->success(std:("Successfully pushed code to GitHub repository"));
        if (OR((packageJson->packageType == std:("project")), (skipRegistry))) {
            auto reason = (packageJson->packageType == std:("project")) ? std:("Projects do not need registry updates") : std:("Registry updates skipped as requested with --skip-registry flag");
            logger->info(string_empty + packageJson->name + std:(" published to GitHub successfully. ") + reason + string_empty);
            return object{
                object::pair{std:("success"), true}, 
                object::pair{std:("prUrl"), repoResult["repoUrl"]}
            };
        }
    }
    if (OR((packageJson->packageType == std:("project")), (skipRegistry))) {
        if (isTest) {
            logger->info(std:("Test successful - project would be published to GitHub only"));
        }
        return true;
    }
    auto settings = std::async([=]() { getRegistrySettings(); });
    auto [registryOwner, registryRepo] = settings->defaultRegistry->split(std:("/"));
    auto hasFork = std::async([=]() { forkExists(token, registryRepo, username); });
    string forkFullName;
    if (AND((!hasFork), (!isTest))) {
        logger->info(std:("Creating fork of ") + settings->defaultRegistry + std:("..."));
        auto fork = std::async([=]() { forkRepository(token, registryOwner, registryRepo); });
        if (!fork) {
            logger->error(std:("Failed to fork registry repository."));
            return false;
        }
        forkFullName = fork;
        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
        {
            return setTimeout(resolve, 2000);
        }
        ); });
    } else {
        forkFullName = string_empty + username + std:("/") + registryRepo + string_empty;
        logger->info(std:("Using existing fork: ") + forkFullName + string_empty);
    }
    auto entityType = packageJson->packageType;
    auto packageNameWithoutScope = packageJson->name->replace((new RegExp(std:("^@[^/]+\"))), string_empty);
    string branchName;
    if (AND((entityType == std:("plugin")), (packageNameWithoutScope->startsWith(std:("plugin-"))))) {
        branchName = string_empty + packageNameWithoutScope + std:("-") + packageJson->version + string_empty;
        logger->info(std:("Using package name directly to avoid duplicate plugin prefix: ") + branchName + string_empty);
    } else {
        branchName = string_empty + entityType + std:("-") + packageNameWithoutScope + std:("-") + packageJson->version + string_empty;
    }
    auto hasBranch = std::async([=]() { branchExists(token, username, registryRepo, branchName); });
    if (AND((!hasBranch), (!isTest))) {
        logger->info(std:("Creating branch ") + branchName + std:("..."));
        auto created = std::async([=]() { createBranch(token, username, registryRepo, branchName); });
        if (!created) {
            logger->error(std:("Failed to create branch."));
            return false;
        }
    }
    auto packageName = packageJson->name->replace((new RegExp(std:("^@[^/]+\"))), string_empty);
    auto registryPackageName = packageJson->name;
    if (!isTest) {
        try
        {
            auto indexContent = std::async([=]() { getFileContent(token, username, registryRepo, std:("index.json")); });
            if (indexContent) {
                auto githubRepo = std:("github:") + username + std:("/") + packageName + string_empty;
                auto index = JSON->parse(indexContent);
                if (const_(index)[registryPackageName]) {
                    logger->warn(std:("Package ") + registryPackageName + std:(" already exists in registry"));
                    return false;
                }
                logger->info(std:("Adding registry entry: ") + registryPackageName + std:(" -> ") + githubRepo + string_empty);
                auto lines = indexContent->split(std:("\
"));
                auto newEntry = std:("    "") + registryPackageName + std:("": "") + githubRepo + std:("",");
                auto insertIndex = -1;
                for (auto i = 0; i < lines->get_length(); i++)
                {
                    auto line = const_(lines)[i]->trim();
                    if (OR((!line), (line == std:("{")))) continue;
                    if (line == std:("}")) {
                        insertIndex = i;
                        break;
                    }
                    auto match = line->match((new RegExp(std:("^\s*"(@[^"]+)"))));
                    if (match) {
                        auto existingPackage = (*const_(match))[1];
                        if (registryPackageName < existingPackage) {
                            insertIndex = i;
                            break;
                        }
                    }
                }
                if (insertIndex == -1) {
                    for (auto i = lines->get_length() - 1; i >= 0; i--)
                    {
                        if (const_(lines)[i]->trim() == std:("}")) {
                            insertIndex = i;
                            break;
                        }
                    }
                }
                if (insertIndex == -1) {
                    logger->error(std:("Could not find insertion point in index.json"));
                    return false;
                }
                lines->splice(insertIndex, 0, newEntry);
                auto updatedContent = lines->join(std:("\
"));
                auto indexUpdated = std::async([=]() { updateFile(token, username, registryRepo, std:("index.json"), updatedContent, std:("Add ") + registryPackageName + std:(" to registry"), branchName); });
                if (!indexUpdated) {
                    logger->error(std:("Failed to update registry index."));
                    return false;
                }
            } else {
                logger->error(std:("Could not fetch index.json from registry"));
                return false;
            }
        }
        catch (const any& error)
        {
            logger->error(std:("Failed to update index.json: ") + (is<Error>(error)) ? error->message : String(error) + string_empty);
            return false;
        }
        auto prUrl = std::async([=]() { createPullRequest(token, registryOwner, registryRepo, std:("Add ") + registryPackageName + std:(" to registry"), std:("This PR adds ") + registryPackageName + std:(" to the registry.\
\
- Package name: ") + registryPackageName + std:("\
- GitHub repository: github:") + username + std:("/") + packageName + std:("\
- Version: ") + packageJson->version + std:("\
- Description: ") + (OR((packageJson->description), (std:("No description provided")))) + std:("\
\
Submitted by: @") + username + string_empty, string_empty + username + std:(":") + branchName + string_empty, std:("main")); });
        if (!prUrl) {
            logger->error(std:("Failed to create pull request."));
            return false;
        }
        logger->success(std:("Pull request created: ") + prUrl + string_empty);
        return object{
            object::pair{std:("success"), true}, 
            object::pair{std:("prUrl"), prUrl}
        };
    } else {
        logger->info(std:("Test successful - all checks passed"));
        logger->info(std:("Would create:"));
        logger->info(std:("- Branch: ") + branchName + string_empty);
        logger->info(std:("- Registry entry: ") + registryPackageName + std:(" -> github:") + username + std:("/") + packageName + string_empty);
        logger->info(std:("- Pull request: Add ") + registryPackageName + std:(" to registry"));
    }
    return true;
};


#endif