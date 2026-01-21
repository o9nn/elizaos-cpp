#include "dev-server.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> startDevMode(DevOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto cwd = process.cwd();
    const auto context = createDevContext(cwd);
    const auto serverManager = getServerManager();

    const auto { directoryType } = context;
    const auto isProject = directoryType.type == "elizaos-project";
    const auto isPlugin = directoryType.type == "elizaos-plugin";
    const auto isMonorepo = directoryType.type == "elizaos-monorepo";

    // Log project type
    if (isProject) {
        std::cout << "Identified ElizaOS project package" << std::endl;
        } else if (isPlugin) {
            std::cout << "Identified ElizaOS plugin package" << std::endl;
            } else if (isMonorepo) {
                std::cout << "Identified ElizaOS monorepo" << std::endl;
                } else {
                    console.warn(
                    "Not in a recognized ElizaOS project, plugin, or monorepo directory. Current directory is: " + directoryType.type + ". Running in standalone mode."
                    );
                }

                // Prepare CLI arguments for the start command
                const std::vector<std::string> cliArgs = [];

                // Pass through port option
                if (options.port) {
                    cliArgs.push_back("--port", options.std::to_string(port));
                }

                // Pass through configure option
                if (options.configure) {
                    cliArgs.push_back("--configure");
                }

                // Handle characters - pass through to start command
                if (options.character) {
                    if (Array.isArray(options.character)) {
                        cliArgs.push_back("--character", ...options.character);
                        } else {
                            cliArgs.push_back("--character", options.character);
                        }
                    }

                    // Function to rebuild and restart the server
                    const auto rebuildAndRestart = async () => {;
                        try {
                            // Ensure the server is stopped first
                            serverManager.stop();

                            // Perform rebuild
                            performRebuild(context);

                            std::cout << '✓ Rebuild successful << restarting...' << std::endl;

                            // Start the server with the args
                            serverManager.start(cliArgs);
                            } catch (error) {
                                console.error(
                                "Error during rebuild and restart: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                                );
                                // Try to restart the server even if build fails
                                if (!serverManager.process) {
                                    std::cout << "Attempting to restart server regardless of build failure..." << std::endl;
                                    serverManager.start(cliArgs);
                                }
                            }
                            };

                            // Perform initial build if required
                            if (isProject || isPlugin || isMonorepo) {
                                const auto modeDescription = isMonorepo ? "monorepo" : isProject ? "project" : "plugin";
                                std::cout << "Running in " + modeDescription + " mode" << std::endl;

                                performInitialBuild(context);
                            }

                            // Start the server initially
                            serverManager.start(cliArgs);

                            // Set up file watching if we're in a project, plugin, or monorepo directory
                            if (isProject || isPlugin || isMonorepo) {
                                // Pass the rebuildAndRestart function as the onChange callback
                                watchDirectory(context.watchDirectory, rebuildAndRestart);

                                std::cout << "Dev mode is active! The server will restart when files change." << std::endl;
                                std::cout << "Press Ctrl+C to exit" << std::endl;
                                } else {
                                    // In standalone mode, just keep the server running without watching files
                                    std::cout << "Server is running in standalone dev mode." << std::endl;
                                    std::cout << "Press Ctrl+C to exit" << std::endl;
                                }

}

} // namespace elizaos
