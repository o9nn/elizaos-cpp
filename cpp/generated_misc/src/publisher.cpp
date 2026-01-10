#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/utils/publisher.h"

std::shared_ptr<Promise<boolean>> testPublishToNpm(string cwd)
{
    try
    {
        std::async([=]() { execa(std::string("npm"), array<string>{ std::string("whoami") }); });
        logger->info(std::string("[✓] Logged in to npm"));
        logger->info(std::string("Testing build..."));
        std::async([=]() { execa(std::string("npm"), array<string>{ std::string("run"), std::string("build"), std::string("--dry-run") }, object{
            object::pair{std::string("cwd"), std::string("cwd")}
        }); });
        logger->info(std::string("[✓] Build test successful"));
        std::async([=]() { execa(std::string("npm"), array<string>{ std::string("access"), std::string("ls-packages") }, object{
            object::pair{std::string("cwd"), std::string("cwd")}
        }); });
        logger->info(std::string("[✓] Have publish permissions"));
        return true;
    }
    catch (const any& error)
    {
        logger->error(std::string("Test failed:"), error);
        if (is<Error>(error)) {
            logger->error(std::string("Error message: ") + error->message + string_empty);
            logger->error(std::string("Error stack: ") + error->stack + string_empty);
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
            logger->error(std::string("Failed to get GitHub credentials"));
            return false;
        }
        auto token = credentials["token"];
        logger->info(std::string("[✓] GitHub credentials found"));
        auto response = std::async([=]() { fetch(std::string("https://api.github.com/user"), object{
            object::pair{std::string("headers"), object{
                object::pair{std::string("Authorization"), std::string("token ") + token + string_empty}
            }}
        }); });
        if (!response->ok) {
            logger->error(std::string("Invalid GitHub token or insufficient permissions"));
            return false;
        }
        logger->info(std::string("[✓] GitHub token is valid"));
        if (packageJson->packageType == std::string("project")) {
            logger->info(std::string("[✓] Project validation complete - GitHub token is valid"));
            return true;
        }
        auto settings = std::async([=]() { getRegistrySettings(); });
        auto [registryOwner, registryRepo] = settings->defaultRegistry->split(std::string("/"));
        logger->info(std::string("Testing with registry: ") + registryOwner + std::string("/") + registryRepo + string_empty);
        auto hasFork = std::async([=]() { forkExists(token, registryRepo, username); });
        logger->info((hasFork) ? std::string("[✓] Fork exists") : std::string("[✓] Can create fork"));
        if (!hasFork) {
            logger->info(std::string("Creating fork..."));
            auto forkCreated = std::async([=]() { forkRepository(token, registryOwner, registryRepo); });
            if (!forkCreated) {
                logger->error(std::string("Failed to create fork"));
                return false;
            }
            logger->info(std::string("[✓] Fork created"));
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 3000);
            }
            ); });
        }
        auto branchName = std::string("test-") + packageJson->name->replace((new RegExp(std::string("^@[^/]+\"))), string_empty) + std::string("-") + packageJson->version + string_empty;
        auto hasBranch = std::async([=]() { branchExists(token, username, registryRepo, branchName); });
        logger->info((hasBranch) ? std::string("[✓] Test branch exists") : std::string("[✓] Can create branch"));
        if (!hasBranch) {
            logger->info(std::string("Creating branch..."));
            auto branchCreated = std::async([=]() { createBranch(token, username, registryRepo, branchName, std::string("main")); });
            if (!branchCreated) {
                logger->error(std::string("Failed to create branch"));
                return false;
            }
            logger->info(std::string("[✓] Branch created"));
        }
        auto simpleName = packageJson->name->replace((new RegExp(std::string("^@[^/]+\"))), string_empty)->replace((new RegExp(std::string("[^a-zA-Z0-9-]"))), std::string("-"));
        auto testPath = std::string("test-files/") + simpleName + std::string("-test.json");
        logger->info(std::string("Attempting to create test file: ") + testPath + std::string(" in branch: ") + branchName + string_empty);
        auto dirCreated = std::async([=]() { ensureDirectory(token, string_empty + username + std::string("/") + registryRepo + string_empty, std::string("test-files"), branchName); });
        if (!dirCreated) {
            logger->warn(std::string("Failed to create test directory, but continuing with file creation"));
        }
        auto canUpdate = std::async([=]() { updateFile(token, username, registryRepo, testPath, JSON->stringify(object{
            object::pair{std::string("test"), true}, 
            object::pair{std::string("timestamp"), ((std::make_shared<Date>()))->toISOString()}
        }), std::string("Test file update"), branchName); });
        if (!canUpdate) {
            logger->error(std::string("Cannot update files in repository"));
            return false;
        }
        logger->info(std::string("[✓] Can create and update files"));
        return true;
    }
    catch (const any& error)
    {
        logger->error(std::string("Test failed:"), error);
        return false;
    }
};


std::shared_ptr<Promise<boolean>> publishToNpm(string cwd)
{
    try
    {
        std::async([=]() { execa(std::string("npm"), array<string>{ std::string("whoami") }); });
        logger->info(std::string("Building package..."));
        std::async([=]() { execa(std::string("npm"), array<string>{ std::string("run"), std::string("build") }, object{
            object::pair{std::string("cwd"), std::string("cwd")}, 
            object::pair{std::string("stdio"), std::string("inherit")}
        }); });
        logger->info(std::string("Publishing to npm..."));
        std::async([=]() { execa(std::string("npm"), array<string>{ std::string("publish") }, object{
            object::pair{std::string("cwd"), std::string("cwd")}, 
            object::pair{std::string("stdio"), std::string("inherit")}
        }); });
        return true;
    }
    catch (const any& error)
    {
        logger->error(std::string("Failed to publish to npm:"), error);
        return false;
    }
};


