#include "project-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

DirectoryInfo getProjectType(std::optional<std::string> testPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto targetPath = testPath ? path.resolve(process.cwd(), testPath) : process.cwd();
    return detectDirectoryType(targetPath);

}

std::string processFilterName(std::optional<std::string> name) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!name) return undefined;

    // Handle common filter formats (case-insensitive)
    auto baseName = name.toLowerCase();

    if (
    baseName.endsWith('.test.ts') ||;
    baseName.endsWith('.test.js') ||;
    baseName.endsWith('.spec.ts') ||;
    baseName.endsWith('.spec.js');
    ) {
        baseName = baseName.slice(0, -8); // Remove '.test.ts' / '.test.js' / '.spec.ts' / '.spec.js';
        } else if (baseName.endsWith('.test') || baseName.endsWith('.spec')) {
            baseName = baseName.slice(0, -5); // Remove '.test' / '.spec';
        }

        return baseName;

}

std::future<void> installPluginDependencies(DirectoryInfo projectInfo) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (projectInfo.type != 'elizaos-plugin') {
        return;
    }

    const auto project = loadProject(process.cwd());
    if (
    project.isPlugin &&;
    project.pluginModule.dependencies &&;
    project.pluginModule.dependencies.length > 0;
    ) {
        const auto pluginsDir = path.join(process.cwd(), '.eliza', 'plugins');
        if (!fs.existsSync(pluginsDir)) {
            fs.promises.mkdir(pluginsDir, { recursive: true });
        }

        const auto packageJsonPath = path.join(pluginsDir, 'package.json');
        if (!fs.existsSync(packageJsonPath)) {
            const auto packageJsonContent = {;
                name: 'test-plugin-dependencies',
                version: '1.0.0',
                description: 'A temporary package for installing test plugin dependencies',
                dependencies: {},
                };
                fs.promises.writeFile(packageJsonPath, JSON.stringify(packageJsonContent, nullptr, 2));
            }

            const auto { installPlugin } = import('@/src/utils');

            for (const auto& dependency : project.pluginModule.dependencies)
                installPlugin(dependency, pluginsDir);
                const auto dependencyPath = path.join(pluginsDir, 'node_modules', dependency);
                if (fs.existsSync(dependencyPath)) {
                    try {
                        runBunCommand(['install'], dependencyPath);
                        } catch (error) {
                            logger.warn(
                            "[Test Command] Failed to install devDependencies for " + std::to_string(dependency) + ": " + std::to_string(error)
                            );
                        }
                    }
                }
            }

}

} // namespace elizaos
