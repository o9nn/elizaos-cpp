#include "package-manager.h"

std::shared_ptr<Promise<string>> getPackageManager()
{
    logger->debug(std:("[PackageManager] Using bun as the package manager for ElizaOS CLI"));
    return std:("bun");
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
    return array<string>{ std:("add"), ((isGlobal) ? array<string>{ std:("-g") } : array<any>()) };
};


std::shared_ptr<Promise<void>> removeFromBunLock(string packageName, string directory)
{
    auto lockFilePath = path->join(directory, std:("bun.lock"));
    if (!existsSync(lockFilePath)) {
        logger->debug(std:("No bun.lock file found at ") + lockFilePath + std:(", skipping removal"));
        return std::shared_ptr<Promise<void>>();
    }
    try
    {
        auto result = std::async([=]() { bunExec(std:("bun"), array<string>{ std:("remove"), packageName }, object{
            object::pair{std:("cwd"), directory}
        }); });
        if (result->success) {
            logger->debug(std:("Successfully removed ") + packageName + std:(" from bun.lock"));
        } else {
            if (OR((result->stderr->includes(std:("not found"))), (result->stderr->includes(std:("No such package"))))) {
                logger->debug(std:("Package ") + packageName + std:(" not found in lockfile (expected for cleanup)"));
            } else {
                logger->warn(std:("Failed to remove ") + packageName + std:(" from bun.lock: ") + (OR((result->stderr), (std:("Unknown error")))) + string_empty);
            }
        }
    }
    catch (const any& error)
    {
        logger->warn(std:("Unexpected error removing ") + packageName + std:(" from bun.lock: ") + error["message"] + string_empty);
    }
};


std::shared_ptr<Promise<object>> executeInstallation(string packageName, string versionOrTag, string directory)
{
    auto installCommand = getInstallCommand(false);
    auto finalSpecifier = (packageName->startsWith(std:("github:"))) ? string_empty + packageName + string_empty + (versionOrTag) ? any(std:("#") + versionOrTag + string_empty) (string_empty) + string_empty : (versionOrTag) ? string_empty + packageName + std:("@") + versionOrTag + string_empty : packageName;
    try
    {
        auto args = array<string>{ installCommand, finalSpecifier };
        std::async([=]() { runBunCommand(args, directory, true); });
        auto installedIdentifier = (packageName->startsWith(std:("github:"))) ? ([=]() mutable
        {
            auto spec = packageName->replace((new RegExp(std:("^github"))), string_empty);
            auto [owner, repoWithRef] = spec->split(std:("/"));
            auto repo = const_(repoWithRef->split(std:("#")))[0];
            return std:("@") + owner + std:("/") + repo + string_empty;
        }
        )() : packageName;
        return object{
            object::pair{std:("success"), true}, 
            object::pair{std:("installedIdentifier"), std:("installedIdentifier")}
        };
    }
    catch (const any& error)
    {
        if (OR((error["code"] == std:("ENOENT")), (error["message"]["includes"](std:("bun: command not found"))))) {
            logger->warn(std:("Installation failed - bun command not found. ") + displayBunInstallationTipCompact() + string_empty);
        } else {
            logger->warn(std:("Installation failed for ") + finalSpecifier + std:(": ") + error["message"] + string_empty);
        }
        return object{
            object::pair{std:("success"), false}, 
            object::pair{std:("installedIdentifier"), nullptr}
        };
    }
};


string buildGitHubSpecifier(string githubSpec, string versionOrTag)
{
    if (!versionOrTag) {
        return githubSpec;
    }
    auto baseSpec = const_(githubSpec->split(std:("#")))[0];
    return string_empty + baseSpec + std:("#") + versionOrTag + string_empty;
};


std::shared_ptr<Promise<object>> executeInstallationWithFallback(string packageName, string versionOrTag, string directory, string githubFallback)
{
    auto result = std::async([=]() { executeInstallation(packageName, versionOrTag, directory); });
    if (OR((result["success"]), (!githubFallback))) {
        return result;
    }
    logger->debug(std:("npm installation failed, attempting GitHub fallback: ") + githubFallback + string_empty);
    std::async([=]() { removeFromBunLock(packageName, directory); });
    auto githubSpecifier = std:("github:") + githubFallback + string_empty + (versionOrTag) ? any(std:("#") + versionOrTag + string_empty) (string_empty) + string_empty;
    return std::async([=]() { executeInstallation(githubSpecifier, string_empty, directory); });
};


