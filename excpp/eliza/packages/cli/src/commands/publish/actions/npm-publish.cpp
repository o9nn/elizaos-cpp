#include "npm-publish.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> publishToNpm(const std::string& cwd, PackageJson packageJson, const std::string& npmUsername) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "Publishing user: " + std::to_string(npmUsername) << std::endl;

    // Update npmPackage field if it's a placeholder or not set
    if (!packageJson.npmPackage || packageJson.npmPackage == '${NPM_PACKAGE}') {
        packageJson.npmPackage = packageJson.name;
        std::cout << "Set npmPackage to: " + std::to_string(packageJson.npmPackage) << std::endl;

        // Save updated package.json
        const auto packageJsonPath = path.join(cwd, 'package.json');
        fs.writeFile(packageJsonPath, JSON.stringify(packageJson, nullptr, 2), 'utf-8');
    }

    // Build the package
    std::cout << 'Building package...' << std::endl;
    execa('npm', ['run', 'build'], { cwd, stdio: 'inherit' });

    // Publish to npm with --ignore-scripts to prevent recursion
    std::cout << 'Publishing to npm...' << std::endl;
    execa('npm', ['publish', '--ignore-scripts'], { cwd, stdio: 'inherit' });

    std::cout << "[√] Successfully published " + std::to_string(packageJson.name) + "@" + std::to_string(packageJson.version) + " to npm" << std::endl;

}

} // namespace elizaos
