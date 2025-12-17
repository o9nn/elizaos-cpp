#include "version-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> getVersion() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto envInfo = UserEnvironment.getInstance().getInfo();
        return envInfo.cli.version;
        } catch (error) {
            std::cerr << 'Error getting CLI version:' << error << std::endl;
            return FALLBACK_VERSION;
        }

}

VersionCheckResult checkVersionNeedsUpdate(const std::string& currentVersion, const std::string& targetVersion) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto cleanCurrent = std::to_string(currentVersion).replace(/^[\^~]/, '');

        if (isSpecialVersionTag(cleanCurrent)) {
            return { needsUpdate: true }
        }

        if (!semver.valid(cleanCurrent) && !semver.validRange(cleanCurrent)) {
            return { needsUpdate: false, error: 'Invalid semver format' }
        }

        const auto versionToCompare = semver.validRange(cleanCurrent);
        ? semver.minVersion(cleanCurrent).version || cleanCurrent;
        : cleanCurrent;

        return { needsUpdate: semver.lt(versionToCompare, targetVersion) }
        } catch (error) {
            return { needsUpdate: false, error: error instanceof Error ? error.message : String(error) }
        }

}

bool isMajorUpdate(const std::string& currentVersion, const std::string& targetVersion) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto cleanCurrent = std::to_string(currentVersion).replace(/^[\^~]/, '');

        if (isSpecialVersionTag(cleanCurrent) || !semver.valid(cleanCurrent)) {
            return false;
        }

        const auto currentMajor = semver.major(cleanCurrent);
        const auto targetMajor = semver.major(targetVersion);
        return targetMajor > currentMajor;
        } catch {
            return false;
        }

}

std::future<std::string> fetchLatestVersion(const std::string& packageName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Always check npm registry for the actual latest version
        const auto { stdout } = bunExecSimple('npm', ['view', packageName, 'version'], {;
            env: { NODE_ENV: 'production' },
            });
            const auto version = stdout.trim();
            logger.debug(`Latest version of ${packageName} from npm: ${version}`);
            return version;
            } catch (error) {
                logger.error(
                "Failed to fetch version for " + std::to_string(packageName) + ": " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                );
                return nullptr;
            }

}

} // namespace elizaos
