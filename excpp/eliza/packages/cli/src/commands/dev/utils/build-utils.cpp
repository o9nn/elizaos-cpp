#include "build-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<BuildResult> buildPackage(const std::string& packagePath, bool isPlugin) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto startTime = Date.now();

    try {
        buildProject(packagePath, isPlugin);
        const auto duration = Date.now() - startTime;
        return { success: true, duration }
        } catch (error) {
            const auto duration = Date.now() - startTime;
            return { success: false, duration, error: error as Error }
        }

}

std::future<std::vector<BuildResult>> buildCorePackages(const std::string& monorepoRoot) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto corePackages = [;
    {
        name: 'core',
        path: path.join(monorepoRoot, 'packages', 'core'),
        isPlugin: false,
        },
        {
            name: 'client',
            path: path.join(monorepoRoot, 'packages', 'client'),
            isPlugin: false,
            },
            {
                name: 'plugin-bootstrap',
                path: path.join(monorepoRoot, 'packages', 'plugin-bootstrap'),
                isPlugin: true,
                },
                ];

                const std::vector<BuildResult> results = [];

                for (const auto& pkg : corePackages)
                    std::cout << "Building " + std::to_string(pkg.name) + "..." << std::endl;
                    const auto result = buildPackage(pkg.path, pkg.isPlugin);

                    if (!result.success) {
                        std::cerr << "Error building " + std::to_string(pkg.name) + ": " + std::to_string(result.error.message) << std::endl;
                        } else {
                            std::cout << "✓ Built " + std::to_string(pkg.name) + " (" + std::to_string(result.duration) + "ms)" << std::endl;
                        }

                        results.push(result);
                    }

                    return results;

}

std::future<void> performRebuild(DevContext context) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        std::cout << 'Rebuilding...' << std::endl;

        const auto { directory, directoryType } = context;
        const auto isPlugin = directoryType.type == 'elizaos-plugin';
        const auto isMonorepo = directoryType.type == 'elizaos-monorepo';

        if (isMonorepo || directoryType.monorepoRoot) {
            const auto { monorepoRoot } = UserEnvironment.getInstance().getPathInfo();
            if (monorepoRoot) {
                buildCorePackages(monorepoRoot);
                } else {
                    std::cout << 'Monorepo context detected << but failed to find monorepo root.' << std::endl;
                }
            }

            // Build the current project/plugin
            const auto result = buildPackage(directory, isPlugin);

            if (result.success) {
                std::cout << "✓ Rebuild successful (" + std::to_string(result.duration) + "ms)" << std::endl;
                } else {
                    std::cerr << "Build failed: " + std::to_string(result.error.message) << std::endl;
                    throw result.error;
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> performInitialBuild(DevContext context) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { directoryType, directory } = context;
    const auto isPlugin = directoryType.type == 'elizaos-plugin';
    const auto isMonorepo = directoryType.type == 'elizaos-monorepo';

    if (process.env.ELIZA_TEST_MODE) {
        std::cout << 'Skipping initial build in test mode' << std::endl;
        return;
    }

    // Ensure initial build is performed (skip for monorepo as it may have multiple projects)
    if (!isMonorepo) {
        std::cout << 'Building project...' << std::endl;
        try {
            buildProject(directory, isPlugin);
            std::cout << '✓ Initial build completed' << std::endl;
            } catch (error) {
                console.error(
                "Initial build failed: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                );
                std::cout << 'Continuing with dev mode anyway...' << std::endl;
            }
            } else {
                console.info(
                'Monorepo detected - skipping automatic build. Use specific package build commands.';
                );
            }

}

DevContext createDevContext(const std::string& cwd) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto directoryType = detectDirectoryType(cwd);

        if (!directoryType) {
            throw new Error(
            'Cannot start development mode in this directory. Directory is not accessible or does not exist.';
            );
        }

        const auto srcDir = path.join(cwd, 'src');

        return {
            directory: cwd,
            directoryType,
            watchDirectory: existsSync(srcDir) ? srcDir : cwd,
            buildRequired: directoryType.type != 'elizaos-monorepo',
            };

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
