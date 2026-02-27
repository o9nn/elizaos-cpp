#include "publisher.hpp"

std::shared_ptr<Promise<boolean>> testPublishToNpm(string cwd)
{
    try
    {
        std::async([=]() { execa(std:("npm"), array<string>{ std:("whoami") }); });
        logger->info(std:("[✓] Logged in to npm"));
        logger->info(std:("Testing build..."));
        std::async([=]() { execa(std:("npm"), array<string>{ std:("run"), std:("build"), std:("--dry-run") }, object{
            object::pair{std:("cwd"), std:("cwd")}
        }); });
        logger->info(std:("[✓] Build test successful"));
        std::async([=]() { execa(std:("npm"), array<string>{ std:("access"), std:("ls-packages") }, object{
            object::pair{std:("cwd"), std:("cwd")}
        }); });
        logger->info(std:("[✓] Have publish permissions"));
        return true;
    }
    catch (const any& error)
    {
        logger->error(std:("Test failed:"), error);
        if (is<Error>(error)) {
            logger->error(std:("Error message: ") + error->message + string_empty);
            logger->error(std:("Error stack: ") + error->stack + string_empty);
        }
        return false;
    }
};


std::shared_ptr<Promise<boolean>> testPublishToGitHub(std::shared_ptr<PackageJson> packageJson, string username)
{
    try
    {
        auto credentials = std::async([=]() { getGitHubCredentials(); });
        if (!credentials) {
            logger->error(std:("Failed to get GitHub credentials"));
            return false;
        }
        auto token = credentials["token"];
        logger->info(std:("[✓] GitHub credentials found"));
        auto response = std::async([=]() { fetch(std:("https://api.github.com/user"), object{
            object::pair{std:("headers"), object{
                object::pair{std:("Authorization"), std:("token ") + token + string_empty}
            }}
        }); });
        if (!response->ok) {
            logger->error(std:("Invalid GitHub token or insufficient permissions"));
            return false;
        }
        logger->info(std:("[✓] GitHub token is valid"));
        if (packageJson->packageType == std:("project")) {
            logger->info(std:("[✓] Project validation complete - GitHub token is valid"));
            return true;
        }
        auto settings = std::async([=]() { getRegistrySettings(); });
        auto [registryOwner, registryRepo] = settings->defaultRegistry->split(std:("/"));
        logger->info(std:("Testing with registry: ") + registryOwner + std:("/") + registryRepo + string_empty);
        auto hasFork = std::async([=]() { forkExists(token, registryRepo, username); });
        logger->info((hasFork) ? std:("[✓] Fork exists") : std:("[✓] Can create fork"));
        if (!hasFork) {
            logger->info(std:("Creating fork..."));
            auto forkCreated = std::async([=]() { forkRepository(token, registryOwner, registryRepo); });
            if (!forkCreated) {
                logger->error(std:("Failed to create fork"));
                return false;
            }
            logger->info(std:("[✓] Fork created"));
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 3000);
            }
            ); });
        }
        auto branchName = std:("test-") + packageJson->name->replace((new RegExp(std:("^@[^/]+\"))), string_empty) + std:("-") + packageJson->version + string_empty;
        auto hasBranch = std::async([=]() { branchExists(token, username, registryRepo, branchName); });
        logger->info((hasBranch) ? std:("[✓] Test branch exists") : std:("[✓] Can create branch"));
        if (!hasBranch) {
            logger->info(std:("Creating branch..."));
            auto branchCreated = std::async([=]() { createBranch(token, username, registryRepo, branchName, std:("main")); });
            if (!branchCreated) {
                logger->error(std:("Failed to create branch"));
                return false;
            }
            logger->info(std:("[✓] Branch created"));
        }
        auto simpleName = packageJson->name->replace((new RegExp(std:("^@[^/]+\"))), string_empty)->replace((new RegExp(std:("[^a-zA-Z0-9-]"))), std:("-"));
        auto testPath = std:("test-files/") + simpleName + std:("-test.json");
        logger->info(std:("Attempting to create test file: ") + testPath + std:(" in branch: ") + branchName + string_empty);
        auto dirCreated = std::async([=]() { ensureDirectory(token, string_empty + username + std:("/") + registryRepo + string_empty, std:("test-files"), branchName); });
        if (!dirCreated) {
            logger->warn(std:("Failed to create test directory, but continuing with file creation"));
        }
        auto canUpdate = std::async([=]() { updateFile(token, username, registryRepo, testPath, JSON->stringify(object{
            object::pair{std:("test"), true}, 
            object::pair{std:("timestamp"), ((std::make_shared<Date>()))->toISOString()}
        }), std:("Test file update"), branchName); });
        if (!canUpdate) {
            logger->error(std:("Cannot update files in repository"));
            return false;
        }
        logger->info(std:("[✓] Can create and update files"));
        return true;
    }
    catch (const any& error)
    {
        logger->error(std:("Test failed:"), error);
        return false;
    }
};


std::shared_ptr<Promise<boolean>> publishToNpm(string cwd)
{
    try
    {
        std::async([=]() { execa(std:("npm"), array<string>{ std:("whoami") }); });
        logger->info(std:("Building package..."));
        std::async([=]() { execa(std:("npm"), array<string>{ std:("run"), std:("build") }, object{
            object::pair{std:("cwd"), std:("cwd")}, 
            object::pair{std:("stdio"), std:("inherit")}
        }); });
        logger->info(std:("Publishing to npm..."));
        std::async([=]() { execa(std:("npm"), array<string>{ std:("publish") }, object{
            object::pair{std:("cwd"), std:("cwd")}, 
            object::pair{std:("stdio"), std:("inherit")}
        }); });
        return true;
    }
    catch (const any& error)
    {
        logger->error(std:("Failed to publish to npm:"), error);
        return false;
    }
};


