#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/update/utils/environment-utils.h"

std::shared_ptr<Promise<std::shared_ptr<CliEnvironment>>> getCliEnvironment()
{
    auto [isGlobal, isNpx, isBunx, isNpmInstalled, packageManager] = std::async([=]() { Promise->all(std::tuple<any, any, any, any, any>{ isGlobalInstallation(), isRunningViaNpx(), isRunningViaBunx(), isCliInstalledViaNpm(), getPackageManager() }); });
    return object{
        object::pair{std::string("isGlobal"), std::string("isGlobal")}, 
        object::pair{std::string("isNpx"), std::string("isNpx")}, 
        object::pair{std::string("isBunx"), std::string("isBunx")}, 
        object::pair{std::string("isNpmInstalled"), std::string("isNpmInstalled")}, 
        object::pair{std::string("packageManager"), std::string("packageManager")}
    };
};


