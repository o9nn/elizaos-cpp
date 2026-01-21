#include "npm-publish.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> publishToNpm(const std::string& cwd, PackageJson packageJson, const std::string& npmUsername) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "Publishing user: " + npmUsername << std::endl;

    // Update npmPackage field if it's a placeholder or not set
    if (!packageJson.npmPackage || packageJson.npmPackage == '${NPM_PACKAGE}') {
        packageJson.npmPackage = packageJson.name;
        std::cout << "Set npmPackage to: " + packageJson.npmPackage << std::endl;

        // Save updated package.json
        const auto packageJsonPath = path.join(cwd, "package.json");
        fs.writeFile(packageJsonPath, /* JSON.stringify */ std::string(packageJson, nullptr, 2), "utf-8");
    }

    // Build the package
    std::cout << "Building package..." << std::endl;
    bunExecInherit("npm", ["run", "build"], { cwd });

    // Publish to npm with --ignore-scripts to prevent recursion
    std::cout << "Publishing to npm..." << std::endl;
    bunExecInherit("npm", ["publish", "--ignore-scripts"], { cwd });

    std::cout << "[√] Successfully published " + packageJson.name + "@" + packageJson.version + " to npm" << std::endl;

}

} // namespace elizaos
