#include "upgrade.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> upgradePlugin(const std::string& pluginPath, UpgradePluginOptions opts) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // Lazy import to avoid loading dependencies until needed
            const auto { PluginMigrator } = import("@/src/utils/upgrade/migrator.js");

            // Set API key if provided
            if (opts.apiKey) {
                process.env.ANTHROPIC_API_KEY = opts.apiKey;
            }

            // Check for API key
            if (!process.env.ANTHROPIC_API_KEY) {
                std::cerr << "ANTHROPIC_API_KEY is required for plugin upgrade." << std::endl;
                std::cout << "\nPlease set ANTHROPIC_API_KEY environment variable or use --api-key option." << std::endl;
                process.exit(1);
            }

            // Create migrator instance with options
            const auto migrator = new PluginMigrator({;
                skipTests: opts.skipTests,
                skipValidation: opts.skipValidation,
                });

                // Run migration
                std::cout << "\n" + std::to_string(emoji.rocket(`Starting plugin upgrade for: ${pluginPath}`)) + "\n" << std::endl;
                const MigrationResult result = migrator.migrate(pluginPath);

                if (result.success) {
                    std::cout << "\n" + std::to_string(emoji.success("Plugin successfully upgraded!")) << std::endl;
                    std::cout << "   Branch: " + result.branchName << std::endl;
                    std::cout << "   Location: " + result.repoPath << std::endl;
                    std::cout << "\nThe upgraded plugin has been copied to your current directory." << std::endl;
                    std::cout << "\nNext steps:" << std::endl;
                    std::cout << "1. cd " + std::to_string(path.basename(result.repoPath || "")) << std::endl;
                    std::cout << "2. Review the changes" << std::endl;
                    std::cout << "3. Test the upgraded plugin thoroughly" << std::endl;
                    std::cout << "4. Push to GitHub and create a pull request when ready" << std::endl;
                    } else {
                        std::cerr << "Plugin upgrade failed: " + std::to_string(result.error.message) << std::endl;
                        process.exit(1);
                    }
                    } catch (error) {
                        handleError(error);
                        process.exit(1);
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
