#include "project-utils.hpp"
#include <string>
#include <future>
#include <filesystem>
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

DirectoryInfo getProjectType(std::optional<std::string> testPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto targetPath = testPath ? path.resolve(std::filesystem::current_path().string(), testPath) : std::filesystem::current_path().string();
    return detectDirectoryType(targetPath);

}

std::string processFilterName(std::optional<std::string> name) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!name) return std::nullopt;

    // Handle common filter formats (case-insensitive)
    auto baseName = name.toLowerCase();

    if (
    baseName.rfind(".test.ts") ||;
    baseName.rfind(".test.js") ||;
    baseName.rfind(".spec.ts") ||;
    baseName.rfind(".spec.js");
    ) {
        baseName = baseName.slice(0, -8); // Remove ".test.ts" / ".test.js" / ".spec.ts" / ".spec.js";
        } else if (baseName.rfind(".test") || baseName.rfind(".spec")) {
            baseName = baseName.slice(0, -5); // Remove ".test" / ".spec";
        }

        return baseName;

}

std::future<void> installPluginDependencies(DirectoryInfo projectInfo) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (projectInfo.type != 'elizaos-plugin') {
        return;
    }

    const auto project = loadProject(std::filesystem::current_path().string());
    if (
    project.isPlugin &&;
    project.pluginModule.dependencies &&;
    project.pluginModule.dependencies.size() > 0;
    ) {
        const auto pluginsDir = path.join(std::filesystem::current_path().string(), ".eliza", "plugins");
        if (!fs.existsSync(pluginsDir)) {
            fs.promises.mkdir(pluginsDir, Config{recursive = true});
        }

        const auto packageJsonPath = path.join(pluginsDir, "package.json");
        if (!fs.existsSync(packageJsonPath)) {
            const auto packageJsonContent = {;
                name: "test-plugin-dependencies",
                version: "1.0.0",
                description: "A temporary package for installing test plugin dependencies",
                dependencies: {},
                };
                fs.promises.writeFile(packageJsonPath, /* JSON.stringify */ std::string(packageJsonContent, nullptr, 2));
            }

            const auto { installPlugin } = import("@/src/utils");

            for (const auto& dependency : project.pluginModule.dependencies)
                installPlugin(dependency, pluginsDir);
                const auto dependencyPath = path.join(pluginsDir, "node_modules", dependency);
                if (fs.existsSync(dependencyPath)) {
                    try {
                        runBunCommand(["install"], dependencyPath);
                        } catch (error) {
                            logger.warn(
                            "[Test Command] Failed to install devDependencies for " + dependency + ": " + error
                            );
                        }
                    }
                }
            }

}

} // namespace elizaos
