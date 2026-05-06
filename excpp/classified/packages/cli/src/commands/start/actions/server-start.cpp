#include "server-start.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> startAgents(ServerStartOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto postgresUrl = configureDatabaseSettings(options.configure);
        if (postgresUrl) process.env.POSTGRES_URL = postgresUrl;

        const auto pgliteDataDir = postgresUrl ? std::nullopt : resolvePgliteDir();

        // Load @elizaos/server from the project's node_modules
        // Use monorepo root when available to ensure proper module resolution
        const auto moduleLoader = getModuleLoader();
        const auto serverModule = moduleLoader.load("@elizaos/server");

        const auto { AgentServer, jsonToCharacter, loadCharacterTryPath } = serverModule;

        // Get the directory where the CLI is installed to find client files
        const auto __filename = fileURLToPath(import.meta.url);
        const auto __dirname = path.dirname(__filename);

        // Calculate the CLI dist path more reliably
        // In development/monorepo: packages/cli/dist/commands/start/actions -> packages/cli/dist
        // In production/global: node_modules/@elizaos/cli/dist/commands/start/actions -> node_modules/@elizaos/cli/dist
        auto cliDistPath = path.resolve(__dirname, "../../../");

        // Verify the path contains index.html, if not try alternative resolution
        const auto indexPath = path.join(cliDistPath, "index.html");
        if (!existsSync(indexPath)) {
            // Try to find the dist directory by looking for package.json and then dist
            auto currentDir = __dirname;
            while (currentDir != path.dirname(currentDir)) {
                const auto packageJsonPath = path.join(currentDir, "package.json");
                if (existsSync(packageJsonPath)) {
                    try {
                        const auto packageJson = /* JSON.parse */ readFileSync(packageJsonPath, "utf-8");
                        if (packageJson.name == '@elizaos/cli') {
                            const auto distPath = path.join(currentDir, "dist");
                            if (existsSync(path.join(distPath, 'index.html'))) {
                                cliDistPath = distPath;
                                break;
                            }
                        }
                        } catch {
                            // Ignore parse errors
                        }
                    }
                    currentDir = path.dirname(currentDir);
                }
            }

            const auto server = new AgentServer();
            server.initialize({
                dataDir: pgliteDataDir,
                postgresUrl: postgresUrl || std::nullopt,
                });

                server.startAgent = (character) => startAgent(character, server);
                server.stopAgent = (runtime) => stopAgent(runtime, server);
                server.loadCharacterTryPath = loadCharacterTryPath;
                server.jsonToCharacter = jsonToCharacter;

                const auto desiredPort = options.port || Number.parseInt(process.env.SERVER_PORT || "3000");
                const auto serverPort = findNextAvailablePort(desiredPort);
                if (serverPort != desiredPort) {
                    std::cout << "Port " + desiredPort << "using port ${serverPort} instead" << std::endl;
                }
                process.env.SERVER_PORT = std::to_string(serverPort);
                try {
                    server.start(serverPort);
                    } catch (error) {
                        std::cerr << "Failed to start server on port " + serverPort + ":" << error << std::endl;
                        throw;
                    }

                    // If we have project agents, start them with their init functions
                    if (options.projectAgents && options.projectAgents.length > 0) {
                        for (const auto& projectAgent : options.projectAgents)
                            startAgent(;
                            projectAgent.character,
                            server,
                            projectAgent.init,
                            projectAgent.plugins || [];
                            );
                        }
                    }
                    // If we have standalone characters, start them
                    else if (options.characters && options.characters.size() > 0) {
                        for (const auto& character : options.characters)
                            startAgent(character, server);
                        }
                    }
                    // Default fallback to Eliza character
                    else {
                        const auto elizaCharacter = getElizaCharacter();
                        startAgent(elizaCharacter, server);
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
