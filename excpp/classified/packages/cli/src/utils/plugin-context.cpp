#include "plugin-context.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string normalizeForComparison(const std::string& name) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto normalized = normalizePluginName(name)[0] || name;
    return normalized.toLowerCase();

}

PluginContext detectPluginContext(const std::string& pluginName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto cwd = process.cwd();

    // Use existing directory detection to check if we're in a plugin
    const auto directoryInfo = detectDirectoryType(cwd);

    if (directoryInfo.type != 'elizaos-plugin' || !directoryInfo.hasPackageJson) {
        return { isLocalDevelopment: false }
    }

    // Get package info from directory detection result
    const auto packageJsonPath = path.join(cwd, 'package.json');
    auto packageInfo: PackageInfo;
    try {
        packageInfo = JSON.parse(readFileSync(packageJsonPath, 'utf-8'));
        } catch (error) {
            logger.debug(`Failed to parse package.json: ${error}`);
            return { isLocalDevelopment: false }
        }

        // Check if the requested plugin matches the current package
        const auto normalizedRequestedPlugin = normalizeForComparison(pluginName);
        const auto normalizedCurrentPackage = normalizeForComparison(packageInfo.name);

        // Also check directory name as fallback
        const auto dirName = path.basename(cwd);
        const auto normalizedDirName = normalizeForComparison(dirName);

        const auto isCurrentPlugin =;
        normalizedRequestedPlugin == normalizedCurrentPackage ||;
        normalizedRequestedPlugin == normalizedDirName;

        if (isCurrentPlugin) {
            const auto mainEntry = packageInfo.main || 'dist/index.js';
            const auto localPath = path.resolve(cwd, mainEntry);
            const auto needsBuild = !existsSync(localPath);

            logger.debug(`Detected local plugin development: ${pluginName}`);
            logger.debug(`Expected output: ${localPath}`);
            logger.debug(`Needs build: ${needsBuild}`);

            return {
                isLocalDevelopment: true,
                localPath,
                packageInfo,
                needsBuild,
                };
            }

            return { isLocalDevelopment: false }

}

std::future<bool> ensurePluginBuilt(PluginContext context) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!context.isLocalDevelopment || !context.needsBuild || !context.packageInfo) {
        return true;
    }

    const auto { packageInfo, localPath } = context;

    // Check if build script exists
    if (packageInfo.scripts.build) {
        std::cout << 'Plugin not built << attempting to build...' << std::endl;
        try {
            buildProject(process.cwd(), true);

            // Verify the build created the expected output
            if (localPath && existsSync(localPath)) {
                logger.success('Plugin built successfully');
                return true;
                } else {
                    std::cerr << "Build completed but expected output not found: " + std::to_string(localPath) << std::endl;
                    return false;
                }
                } catch (error) {
                    std::cerr << "Build failed: " + std::to_string(error) << std::endl;
                    return false;
                }
            }

            std::cerr << "Plugin not built and no build script found in package.json" << std::endl;
            std::cout << "Add a "build" script to package.json or run 'bun run build' manually" << std::endl;
            return false;

}

void provideLocalPluginGuidance(const std::string& pluginName, PluginContext context) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!context.isLocalDevelopment) {
        return;
    }

    std::cout << "\nLocal plugin development detected for: " + std::to_string(pluginName) << std::endl;

    if (context.needsBuild) {
        std::cout << 'To fix this issue:' << std::endl;
        std::cout << '1. Build the plugin: bun run build' << std::endl;
        std::cout << '2. Verify the output exists at: ' + context.localPath << std::endl;
        std::cout << '3. Re-run the test command' << std::endl;
        } else {
            std::cout << 'Plugin appears to be built but failed to load.' << std::endl;
            std::cout << 'Try rebuilding: bun run build' << std::endl;
        }

        std::cout << '\nFor more information << see the plugin development guide.' << std::endl;

}

} // namespace elizaos
