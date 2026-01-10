#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_PUBLISHER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_PUBLISHER_H
#include "core.h"
#include "execa.h"
#include "@elizaos/core.h"
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
        logger->error(std::string("Failed to get GitHub credentials"));
        return false;
    }
    auto token = credentials["token"];
    if (!packageJson->packageType) {
        logger->error(std::string("Package type is required. Set "packageType" to either "plugin" or "project" in package.json"));
        return false;
    }
    if (AND((packageJson->packageType != std::string("plugin")), (packageJson->packageType != std::string("project")))) {
        logger->error(std::string("Invalid package type: ") + packageJson->packageType + std::string(". Must be either "plugin" or "project""));
        return false;
    }
    if (isTest) {
        logger->info(std::string("Running in test mode - no actual changes will be made"));
    }
    if (skipRegistry) {
        logger->info(std::string("Registry updates will be skipped as requested with --skip-registry flag"));
    }
    if (!isTest) {
        auto repoName = packageJson->name->replace((new RegExp(std::string("^@[^/]+\"))), string_empty);
        auto description = OR((packageJson->description), (std::string("ElizaOS ") + packageJson->packageType + string_empty));
        any topic;
        if (packageJson->packageType == std::string("plugin")) {
            topic = std::string("elizaos-plugins");
        } else if (packageJson->packageType == std::string("project")) {
            topic = std::string("elizaos-projects");
        } else {
            topic = std::string("elizaos-plugins");
        }
        logger->info(std::string("Checking/creating GitHub repository: ") + username + std::string("/") + repoName + string_empty);
        auto repoResult = std::async([=]() { createGitHubRepository(token, repoName, description, false, array<any>{ topic }); });
        if (!repoResult["success"]) {
            logger->error(std::string("Failed to create GitHub repository: ") + repoResult["message"] + string_empty);
            return false;
        }
        logger->info(std::string("Using repository: ") + repoResult["repoUrl"] + string_empty);
        auto repoUrl = std::string("https://") + token + std::string("@github.com/") + username + std::string("/") + repoName + std::string(".git");
        logger->info(std::string("Pushing code to GitHub..."));
        auto pushSuccess = std::async([=]() { pushToGitHub(cwd, repoUrl); });
        if (!pushSuccess) {
            logger->error(std::string("Failed to push code to GitHub repository."));
            return false;
        }
        logger->success(std::string("Successfully pushed code to GitHub repository"));
        if (OR((packageJson->packageType == std::string("project")), (skipRegistry))) {
            auto reason = (packageJson->packageType == std::string("project")) ? std::string("Projects do not need registry updates") : std::string("Registry updates skipped as requested with --skip-registry flag");
            logger->info(string_empty + packageJson->name + std::string(" published to GitHub successfully. ") + reason + string_empty);
            return object{
                object::pair{std::string("success"), true}, 
                object::pair{std::string("prUrl"), repoResult["repoUrl"]}
            };
        }
    }
    if (OR((packageJson->packageType == std::string("project")), (skipRegistry))) {
        if (isTest) {
            logger->info(std::string("Test successful - project would be published to GitHub only"));
        }
        return true;
    }
    auto settings = std::async([=]() { getRegistrySettings(); });
    auto [registryOwner, registryRepo] = settings->defaultRegistry->split(std::string("/"));
    auto hasFork = std::async([=]() { forkExists(token, registryRepo, username); });
    string forkFullName;
    if (AND((!hasFork), (!isTest))) {
        logger->info(std::string("Creating fork of ") + settings->defaultRegistry + std::string("..."));
        auto fork = std::async([=]() { forkRepository(token, registryOwner, registryRepo); });
        if (!fork) {
            logger->error(std::string("Failed to fork registry repository."));
            return false;
        }
        forkFullName = fork;
        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
        {
            return setTimeout(resolve, 2000);
        }
        ); });
    } else {
        forkFullName = string_empty + username + std::string("/") + registryRepo + string_empty;
        logger->info(std::string("Using existing fork: ") + forkFullName + string_empty);
    }
    auto entityType = packageJson->packageType;
    auto packageNameWithoutScope = packageJson->name->replace((new RegExp(std::string("^@[^/]+\"))), string_empty);
    string branchName;
    if (AND((entityType == std::string("plugin")), (packageNameWithoutScope->startsWith(std::string("plugin-"))))) {
        branchName = string_empty + packageNameWithoutScope + std::string("-") + packageJson->version + string_empty;
        logger->info(std::string("Using package name directly to avoid duplicate plugin prefix: ") + branchName + string_empty);
    } else {
        branchName = string_empty + entityType + std::string("-") + packageNameWithoutScope + std::string("-") + packageJson->version + string_empty;
    }
    auto hasBranch = std::async([=]() { branchExists(token, username, registryRepo, branchName); });
    if (AND((!hasBranch), (!isTest))) {
        logger->info(std::string("Creating branch ") + branchName + std::string("..."));
        auto created = std::async([=]() { createBranch(token, username, registryRepo, branchName); });
        if (!created) {
            logger->error(std::string("Failed to create branch."));
            return false;
        }
    }
    auto packageName = packageJson->name->replace((new RegExp(std::string("^@[^/]+\"))), string_empty);
    auto registryPackageName = packageJson->name;
    if (!isTest) {
        try
        {
            auto indexContent = std::async([=]() { getFileContent(token, username, registryRepo, std::string("index.json")); });
            if (indexContent) {
                auto githubRepo = std::string("github:") + username + std::string("/") + packageName + string_empty;
                auto index = JSON->parse(indexContent);
                if (const_(index)[registryPackageName]) {
                    logger->warn(std::string("Package ") + registryPackageName + std::string(" already exists in registry"));
                    return false;
                }
                logger->info(std::string("Adding registry entry: ") + registryPackageName + std::string(" -> ") + githubRepo + string_empty);
                auto lines = indexContent->split(std::string("\
"));
                auto newEntry = std::string("    "") + registryPackageName + std::string("": "") + githubRepo + std::string("",");
                auto insertIndex = -1;
                for (auto i = 0; i < lines->get_length(); i++)
                {
                    auto line = const_(lines)[i]->trim();
                    if (OR((!line), (line == std::string("{")))) continue;
                    if (line == std::string("}")) {
                        insertIndex = i;
                        break;
                    }
                    auto match = line->match((new RegExp(std::string("^\s*"(@[^"]+)"))));
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
                        if (const_(lines)[i]->trim() == std::string("}")) {
                            insertIndex = i;
                            break;
                        }
                    }
                }
                if (insertIndex == -1) {
                    logger->error(std::string("Could not find insertion point in index.json"));
                    return false;
                }
                lines->splice(insertIndex, 0, newEntry);
                auto updatedContent = lines->join(std::string("\
"));
                auto indexUpdated = std::async([=]() { updateFile(token, username, registryRepo, std::string("index.json"), updatedContent, std::string("Add ") + registryPackageName + std::string(" to registry"), branchName); });
                if (!indexUpdated) {
                    logger->error(std::string("Failed to update registry index."));
                    return false;
                }
            } else {
                logger->error(std::string("Could not fetch index.json from registry"));
                return false;
            }
        }
        catch (const any& error)
        {
            logger->error(std::string("Failed to update index.json: ") + (is<Error>(error)) ? error->message : String(error) + string_empty);
            return false;
        }
        auto prUrl = std::async([=]() { createPullRequest(token, registryOwner, registryRepo, std::string("Add ") + registryPackageName + std::string(" to registry"), std::string("This PR adds ") + registryPackageName + std::string(" to the registry.\
\
- Package name: ") + registryPackageName + std::string("\
- GitHub repository: github:") + username + std::string("/") + packageName + std::string("\
- Version: ") + packageJson->version + std::string("\
- Description: ") + (OR((packageJson->description), (std::string("No description provided")))) + std::string("\
\
Submitted by: @") + username + string_empty, string_empty + username + std::string(":") + branchName + string_empty, std::string("main")); });
        if (!prUrl) {
            logger->error(std::string("Failed to create pull request."));
            return false;
        }
        logger->success(std::string("Pull request created: ") + prUrl + string_empty);
        return object{
            object::pair{std::string("success"), true}, 
            object::pair{std::string("prUrl"), prUrl}
        };
    } else {
        logger->info(std::string("Test successful - all checks passed"));
        logger->info(std::string("Would create:"));
        logger->info(std::string("- Branch: ") + branchName + string_empty);
        logger->info(std::string("- Registry entry: ") + registryPackageName + std::string(" -> github:") + username + std::string("/") + packageName + string_empty);
        logger->info(std::string("- Pull request: Add ") + registryPackageName + std::string(" to registry"));
    }
    return true;
};


#endif