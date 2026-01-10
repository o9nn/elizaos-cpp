#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/utils/package-manager.h"

std::shared_ptr<Promise<string>> getPackageManager()
{
    logger->debug(std::string("[PackageManager] Using bun as the package manager for ElizaOS CLI"));
    return std::string("bun");
};


std::shared_ptr<Promise<boolean>> isGlobalInstallation()
{
    auto envInfo = std::async([=]() { UserEnvironment::getInstanceInfo(); });
    return envInfo->packageManager->global;
};


std::shared_ptr<Promise<boolean>> isRunningViaNpx()
{
    auto envInfo = std::async([=]() { UserEnvironment::getInstanceInfo(); });
    return envInfo->packageManager->isNpx;
};


std::shared_ptr<Promise<boolean>> isRunningViaBunx()
{
    auto envInfo = std::async([=]() { UserEnvironment::getInstanceInfo(); });
    return envInfo->packageManager->isBunx;
};


array<string> getInstallCommand(boolean isGlobal)
{
    return array<string>{ std::string("add"), ((isGlobal) ? array<string>{ std::string("-g") } : array<any>()) };
};


std::shared_ptr<Promise<void>> removeFromBunLock(string packageName, string directory)
{
    auto lockFilePath = path->join(directory, std::string("bun.lock"));
    if (!existsSync(lockFilePath)) {
        logger->debug(std::string("No bun.lock file found at ") + lockFilePath + std::string(", skipping removal"));
        return std::shared_ptr<Promise<void>>();
    }
    try
    {
        std::async([=]() { execa(std::string("bun"), array<string>{ std::string("remove"), packageName }, object{
            object::pair{std::string("cwd"), directory}, 
            object::pair{std::string("stdio"), std::string("pipe")}
        }); });
        logger->debug(std::string("Successfully removed ") + packageName + std::string(" from bun.lock"));
    }
    catch (const any& error)
    {
        if (OR((error["message"]["includes"](std::string("not found"))), (error["message"]["includes"](std::string("No such package"))))) {
            logger->debug(std::string("Package ") + packageName + std::string(" not found in lockfile (expected for cleanup)"));
        } else {
            logger->warn(std::string("Failed to remove ") + packageName + std::string(" from bun.lock: ") + error["message"] + string_empty);
        }
    }
};


std::shared_ptr<Promise<object>> executeInstallation(string packageName, string versionOrTag, string directory)
{
    auto installCommand = getInstallCommand(false);
    auto finalSpecifier = (packageName->startsWith(std::string("github:"))) ? string_empty + packageName + string_empty + (versionOrTag) ? any(std::string("#") + versionOrTag + string_empty) : any(string_empty) + string_empty : (versionOrTag) ? string_empty + packageName + std::string("@") + versionOrTag + string_empty : packageName;
    try
    {
        auto args = array<string>{ installCommand, finalSpecifier };
        std::async([=]() { runBunCommand(args, directory); });
        auto installedIdentifier = (packageName->startsWith(std::string("github:"))) ? ([=]() mutable
        {
            auto spec = packageName->replace((new RegExp(std::string("^github"))), string_empty);
            auto [owner, repoWithRef] = spec->split(std::string("/"));
            auto repo = const_(repoWithRef->split(std::string("#")))[0];
            return std::string("@") + owner + std::string("/") + repo + string_empty;
        }
        )() : packageName;
        return object{
            object::pair{std::string("success"), true}, 
            object::pair{std::string("installedIdentifier"), std::string("installedIdentifier")}
        };
    }
    catch (const any& error)
    {
        if (OR((error["code"] == std::string("ENOENT")), (error["message"]["includes"](std::string("bun: command not found"))))) {
            logger->warn(std::string("Installation failed - bun command not found. ") + displayBunInstallationTipCompact() + string_empty);
        } else {
            logger->warn(std::string("Installation failed for ") + finalSpecifier + std::string(": ") + error["message"] + string_empty);
        }
        return object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("installedIdentifier"), nullptr}
        };
    }
};


string buildGitHubSpecifier(string githubSpec, string versionOrTag)
{
    if (!versionOrTag) {
        return githubSpec;
    }
    auto baseSpec = const_(githubSpec->split(std::string("#")))[0];
    return string_empty + baseSpec + std::string("#") + versionOrTag + string_empty;
};


std::shared_ptr<Promise<object>> executeInstallationWithFallback(string packageName, string versionOrTag, string directory, string githubFallback)
{
    auto result = std::async([=]() { executeInstallation(packageName, versionOrTag, directory); });
    if (OR((result["success"]), (!githubFallback))) {
        return result;
    }
    logger->debug(std::string("npm installation failed, attempting GitHub fallback: ") + githubFallback + string_empty);
    std::async([=]() { removeFromBunLock(packageName, directory); });
    auto githubSpecifier = std::string("github:") + githubFallback + string_empty + (versionOrTag) ? any(std::string("#") + versionOrTag + string_empty) : any(string_empty) + string_empty;
    return std::async([=]() { executeInstallation(githubSpecifier, string_empty, directory); });
};


