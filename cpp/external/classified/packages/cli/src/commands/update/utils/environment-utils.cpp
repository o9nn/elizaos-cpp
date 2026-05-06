#include "environment-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<CliEnvironment> getCliEnvironment() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [isGlobal, isNpx, isBunx, isNpmInstalled, packageManager] = Promise.all([;
    isGlobalInstallation(),
    isRunningViaNpx(),
    isRunningViaBunx(),
    isCliInstalledViaNpm(),
    getPackageManager(),
    ]);

    return {
        isGlobal,
        isNpx,
        isBunx,
        isNpmInstalled,
        packageManager,
        };

}

} // namespace elizaos
