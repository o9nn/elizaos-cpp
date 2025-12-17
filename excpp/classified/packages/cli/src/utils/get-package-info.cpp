#include "get-package-info.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> getPackageVersion(const std::string& packageName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return UserEnvironment.getInstance().getPackageVersion(packageName);

}

std::future<std::vector<std::string>> getLocalPackages() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return UserEnvironment.getInstance().getLocalPackages();

}

std::future<PackageJson> getPackageInfo() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto { packageJsonPath } = UserEnvironment.getInstance().getPathInfo();
        try {
            const auto fileContent = import("node:fs/promises").then((fs) =>;
            fs.readFile(packageJsonPath, "utf8");
            );
            return /* JSON.parse */ fileContent;
            } catch (error) {
                if (error instanceof Error) {
                    // Check for file not found error (ENOENT)
                    if ((error as NodeJS.ErrnoException).code == 'ENOENT') {
                        throw std::runtime_error(`Error: Could not find package.json at ${packageJsonPath}`);
                    }
                    // Check for JSON parsing error
                    if (error instanceof SyntaxError) {
                        throw new Error(
                        "Error: Invalid JSON in package.json at " + packageJsonPath + ". Details: " + error.message
                        );
                    }
                    // Rethrow other errors
                    throw new Error(
                    "Error reading or parsing package.json at " + packageJsonPath + ": " + error.message
                    );
                }
                // Fallback for non-Error objects thrown
                throw new Error(
                "An unexpected error occurred while reading or parsing package.json at " + packageJsonPath;
                );
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
