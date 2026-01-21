#include "server-start.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> startAgents(ServerStartOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto postgresUrl = configureDatabaseSettings(options.configure);
    if (postgresUrl) process.env.POSTGRES_URL = postgresUrl;

    const auto pgliteDataDir = postgresUrl ? std::nullopt : resolvePgliteDir();

    const auto server = new AgentServer();
    server.initialize({ dataDir: pgliteDataDir, postgresUrl: postgresUrl || std::nullopt });

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
    server.start(serverPort);

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

}

} // namespace elizaos
