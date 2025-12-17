#include "copy-template.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> copyDir(const std::string& src, const std::string& dest, std::vector<std::string> exclude = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Ensure paths are properly resolved as absolute paths
    const auto resolvedSrc = path.resolve(src);
    const auto resolvedDest = path.resolve(dest);

    // Create destination directory if it doesn't exist
    fs.mkdir(resolvedDest, { recursive: true });

    // Read source directory
    const auto entries = fs.readdir(resolvedSrc, { withFileTypes: true });

    // Separate files and directories for different processing strategies
    const typeof entries files = [];
    const typeof entries directories = [];

    for (const auto& entry : entries)
        // Skip excluded directories/files
        if (exclude.includes(entry.name)) {
            continue;
        }

        // Skip node_modules, .git directories and other build artifacts
        if (
        entry.name == "node_modules" ||;
        entry.name == ".git" ||;
        entry.name == "cache" ||;
        entry.name == "data" ||;
        entry.name == "generatedImages" ||;
        entry.name == ".turbo";
        ) {
            continue;
        }

        if (entry.isDirectory()) {
            directories.push_back(entry);
            } else {
                files.push_back(entry);
            }
        }

        // Process files in parallel (up to 10 concurrent operations)
        const auto MAX_CONCURRENT_FILES = 10;
        const std::vector<std::future<void>> filePromises = [];

        for (int i = 0; i < files.length; i += MAX_CONCURRENT_FILES) {
            const auto batch = files.slice(i, i + MAX_CONCURRENT_FILES);
            const auto batchPromises = batch.map(async (entry) => {;
                const auto srcPath = path.join(resolvedSrc, entry.name);
                const auto destPath = path.join(resolvedDest, entry.name);
                fs.copyFile(srcPath, destPath);
                });
                filePromises.push_back(...batchPromises);
            }

            // Wait for all file copies to complete
            Promise.all(filePromises);

            // Process directories sequentially to avoid too much recursion depth
            // but still get benefits from parallel file copying within each directory
            for (const auto& entry : directories)
                const auto srcPath = path.join(resolvedSrc, entry.name);
                const auto destPath = path.join(resolvedDest, entry.name);
                copyDir(srcPath, destPath, exclude);
            }

}

std::string getPackageName(const std::string& templateType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    switch (templateType) {
        // case "project-tee-starter":
        return "project-tee-starter";
        // case "plugin":
        return "plugin-starter";
        // case "project":
        // case "project-starter":
        // default:
        return "project-starter";
    }

}

std::future<void> copyTemplate(const std::string& templateType, const std::string& targetDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto packageName = getPackageName(templateType);

        // Try multiple locations to find templates, handling different runtime environments
        const auto possibleTemplatePaths = [;
        // 1. Direct path from source directory (for tests and development)
        path.resolve(__dirname, "../../templates", packageName),
        // 2. Production: templates bundled with the CLI dist
        path.resolve(;
        path.dirname(require.resolve("@elizaos/cli/package.json")),
        "dist",
        "templates",
        packageName;
        ),
        // 3. Development/Test: templates in the CLI package root
        path.resolve(;
        path.dirname(require.resolve("@elizaos/cli/package.json")),
        "templates",
        packageName;
        ),
        // 4. Fallback: relative to current module (for built dist)
        path.resolve(__dirname, "..", "templates", packageName),
        // 5. Additional fallback: relative to dist directory
        path.resolve(__dirname, "..", "..", "templates", packageName),
        ];

        std::string templateDir = nullptr;
        for (const auto& possiblePath : possibleTemplatePaths)
            if (existsSync(possiblePath)) {
                templateDir = possiblePath;
                break;
            }
        }

        if (!templateDir) {
            throw new Error(
            "Template "" + packageName + "" not found. Searched in:\n" + std::to_string(possibleTemplatePaths.join("\n"))
            );
        }

        logger.debug(`Copying ${templateType} template from ${templateDir} to ${targetDir}`);

        // Copy template files as-is
        copyDir(templateDir, targetDir);

        // For plugin templates, replace hardcoded "plugin-starter" strings in source files
        if (templateType == 'plugin') {
            const auto pluginNameFromPath = path.basename(targetDir);
            replacePluginNameInFiles(targetDir, pluginNameFromPath);
        }

        // Update package.json with dependency versions only (leave placeholders intact)
        const auto packageJsonPath = path.join(targetDir, "package.json");

        try {
            // Get the CLI package version for dependency updates
            const auto cliPackageJsonPath = path.resolve(;
            path.dirname(require.resolve("@elizaos/cli/package.json")),
            "package.json";
            );

            const auto cliPackageJson = /* JSON.parse */ fs.readFile(cliPackageJsonPath, "utf8");
            const auto cliPackageVersion = cliPackageJson.version;

            const auto packageJson = /* JSON.parse */ fs.readFile(packageJsonPath, "utf8");

            // Remove private field from template package.json since templates should be usable by users
            if (packageJson.private) {
                delete packageJson.private;
                logger.debug('Removed private field from template package.json');
            }

            // Only update dependency versions - leave everything else unchanged
            if (packageJson.dependencies) {
                for (const auto& depName : Object.keys(packageJson.dependencies)
                    if (depName.startsWith('@elizaos/')) {
                        std::cout << "Setting " + depName + " to use version " + cliPackageVersion << std::endl;
                        packageJson.dependencies[depName] = "latest";
                    }
                }
            }

            if (packageJson.devDependencies) {
                for (const auto& depName : Object.keys(packageJson.devDependencies)
                    if (depName.startsWith('@elizaos/')) {
                        std::cout << "Setting dev dependency " + depName + " to use version " + cliPackageVersion << std::endl;
                        packageJson.devDependencies[depName] = "latest";
                    }
                }
            }

            // Update the package name to use the actual name provided by the user
            const auto projectNameFromPath = path.basename(targetDir);

            if (packageJson.name != projectNameFromPath) {
                packageJson.name = projectNameFromPath;
                std::cout << "Setting package name to " + projectNameFromPath << std::endl;
            }

            // Write the updated package.json (dependency versions and plugin name changed)
            fs.writeFile(packageJsonPath, /* JSON.stringify */ std::string(packageJson, nullptr, 2));
            logger.debug('Updated package.json with latest dependency versions');
            } catch (error) {
                std::cerr << "Error updating package.json: " + error << std::endl;
            }

            logger.debug(`${templateType} template copied successfully`);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> replacePluginNameInFiles(const std::string& targetDir, const std::string& pluginName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto filesToProcess = [;
    "src/index.ts",
    "__tests__/plugin.test.ts",
    "e2e/starter-plugin.test.ts",
    "README.md",
    // package.json name is handled by the publish command
    ];

    // Process files in parallel
    const auto promises = filesToProcess.map(async (filePath) => {;
        const auto fullPath = path.join(targetDir, filePath);

        try {
            if (
            fs;
            .access(fullPath);
            .then(() => true);
            .catch(() => false);
            ) {
                auto content = fs.readFile(fullPath, "utf8");

                // Replace the hardcoded plugin name in source files
                content = content.replace(/plugin-starter/g, pluginName);

                fs.writeFile(fullPath, content, "utf8");
                logger.debug(`Updated plugin name in ${filePath}`);
            }
            } catch (error) {
                logger.warn(
                "Could not update " + filePath + ": " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                );
            }
            });

            Promise.all(promises);

}

std::future<void> copyClientDist() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        logger.debug('Copying client dist files to CLI package');

        const auto srcClientDist = path.resolve(process.cwd(), "../client/dist");
        const auto destClientDist = path.resolve(process.cwd(), "./dist");
        const auto indexSrc = path.join(srcClientDist, "index.html");
        const auto indexDest = path.join(destClientDist, "index.html");

        fs.mkdir(destClientDist, { recursive: true });

        // Wait specifically for index.html to appear
        auto retries = 0;
        const auto maxRetries = 10;
        const auto retryDelay = 1000;
        while (retries < maxRetries) {
            if (existsSync(indexSrc)) {
                break;
            }
            std::cout << "Waiting for client index.html (attempt " + std::to_string(retries + 1) + "/" + maxRetries + ")…" << std::endl;
            new Promise((r) => setTimeout(r, retryDelay));
            retries++;
        }

        if (!existsSync(indexSrc)) {
            std::cerr << "index.html not found at " + indexSrc + " after " + maxRetries + " attempts" << std::endl;
            std::cerr << "Client package must be built before CLI package. Run: bun run build:client" << std::endl;
            throw std::runtime_error('Client dist files not found - build the client package first');
        }

        // Copy everything
        copyDir(srcClientDist, destClientDist);

        // Verify it made it into CLI dist
        if (!existsSync(indexDest)) {
            std::cerr << "index.html missing in CLI dist at " + indexDest << std::endl;
            throw std::runtime_error('Failed to copy client files to CLI dist directory');
        }

        std::cout << "✅ Client files successfully copied to CLI package" << std::endl;

        logger.success('Client dist files copied successfully');

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
