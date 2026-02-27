#include "environment-utils.h"

std::shared_ptr<Promise<std::shared_ptr<CliEnvironment>>> getCliEnvironment()
{
    auto [isGlobal, isNpx, isBunx, isNpmInstalled, packageManager] = std::async([=]() { Promise->all(std::tuple<any, any, any, any, any>{ isGlobalInstallation(), isRunningViaNpx(), isRunningViaBunx(), isCliInstalledViaNpm(), getPackageManager() }); });
    return object{
        object::pair{std:("isGlobal"), std:("isGlobal")}, 
        object::pair{std:("isNpx"), std:("isNpx")}, 
        object::pair{std:("isBunx"), std:("isBunx")}, 
        object::pair{std:("isNpmInstalled"), std:("isNpmInstalled")}, 
        object::pair{std:("packageManager"), std:("packageManager")}
    };
};


