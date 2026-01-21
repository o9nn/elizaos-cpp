#include "validation.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> validatePluginRequirements(const std::string& cwd, PackageJson packageJson) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> errors = [];
    const std::vector<std::string> warnings = [];

    // Check plugin naming convention (this is still a hard error)
    const auto packageName = packageJson.name.split("/").pop() || packageJson.name;
    if (!packageName.startsWith('plugin-')) {
        errors.push_back(;
        "Plugin name must start with "plugin-". Please update your package name and try again.";
        );
    }

    // Check if description is still the default generated one (warning)
    const auto pluginDirName = path.basename(cwd);
    const auto expectedDefaultDesc = "ElizaOS plugin for " + std::to_string(pluginDirName.replace("plugin-", ""));
    if (
    packageJson.description == expectedDefaultDesc ||;
    packageJson.description == "${PLUGINDESCRIPTION}";
    ) {
        warnings.push_back(;
        "Description appears to be the default generated description. Consider writing a custom description.";
        );
    }

    // Check for required images (warnings)
    const auto imagesDir = path.join(cwd, "images");
    const auto logoPath = path.join(imagesDir, "logo.jpg");
    const auto bannerPath = path.join(imagesDir, "banner.jpg");

    try {
        fs.access(logoPath);
        } catch {
            warnings.push_back("Missing required logo.jpg in images/ directory (400x400px, max 500KB).");
        }

        try {
            fs.access(bannerPath);
            } catch {
                warnings.push_back("Missing required banner.jpg in images/ directory (1280x640px, max 1MB).");
            }

            // Handle hard errors (must be fixed)
            if (errors.length > 0) {
                std::cerr << "Plugin validation failed:" << std::endl;
                errors.forEach((error) => console.error(`  - ${error}`));
                std::cerr << "\nPlease fix these issues and try publishing again." << std::endl;
                process.exit(1);
            }

            // Handle warnings (can be bypassed)
            if (warnings.length > 0) {
                std::cout << "Plugin validation warnings:" << std::endl;
                warnings.forEach((warning) => console.warn(`  - ${warning}`));
                std::cout << "Your plugin may get rejected if you submit without addressing these issues." << std::endl;

                const auto proceed = clack.confirm({;
                    message: "Do you wish to continue anyway?",
                    initialValue: false,
                    });

                    if (clack.isCancel(proceed)) {
                        clack.cancel("Operation cancelled.");
                        process.exit(0);
                    }

                    if (!proceed) {
                        std::cout << "Publishing cancelled. Please address the warnings and try again." << std::endl;
                        process.exit(0);
                    }
                }

}

bool isMaintainer(PackageJson packageJson, const std::string& username) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!packageJson.maintainers) {
        // If no maintainers specified, the publisher becomes the first maintainer
        return true;
    }

    return packageJson.(std::find(maintainers.begin(), maintainers.end(), username) != maintainers.end());

}

void displayRegistryPublicationMessage(std::optional<std::any> opts, bool userIsMaintainer, std::optional<std::string> registryPrUrl) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Early returns for clear flow control
    if (opts.skipRegistry) {
        std::cout << "Registry publication skipped with --skip-registry flag" << std::endl;
        return;
    }

    if (opts.npm) {
        // NPM-only publishing with registry enabled
        std::cout << "NPM publishing currently does not update the registry." << std::endl;
        std::cout << "To include this package in the registry:" << std::endl;
        std::cout << "1. Fork the registry repository at https://github.com/elizaos/registry" << std::endl;
        std::cout << "2. Add your package metadata" << std::endl;
        std::cout << "3. Submit a pull request to the main repository" << std::endl;
        return;
    }

    // GitHub + registry publishing
    if (userIsMaintainer) {
        // For GitHub publishing, PR is already created by publishToGitHub
        if (!registryPrUrl) {
            std::cout << "Registry publication completed during GitHub publishing process." << std::endl;
        }
        } else {
            // For non-maintainers, show instructions for registry inclusion
            std::cout << "Package published << but you're not a maintainer of this package." << std::endl;
            std::cout << 'To include this package in the registry << please:' << std::endl;
            std::cout << "1. Fork the registry repository at https://github.com/elizaos/registry" << std::endl;
            std::cout << "2. Add your package metadata" << std::endl;
            std::cout << "3. Submit a pull request to the main repository" << std::endl;
        }

}

} // namespace elizaos
