#include "server-start.hpp"
#include <future>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> startAgents(ServerStartOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto postgresUrl = configureDatabaseSettings(options.configure);
    if (postgresUrl) std::getenv("POSTGRES_URL") = postgresUrl;

    const auto pgliteDataDir = postgresUrl ? std::nullopt : resolvePgliteDir();

    const auto server = std::make_unique<AgentServer>();
    server.initialize({ dataDir: pgliteDataDir, postgresUrl: postgresUrl || std::nullopt });

    server.startAgent = [&](character) { return startAgent(character, server); };
    server.stopAgent = [&](runtime) { return stopAgent(runtime, server); };
    server.loadCharacterTryPath = loadCharacterTryPath;
    server.jsonToCharacter = jsonToCharacter;

    const auto desiredPort = options.port || Number.parseInt(std::getenv("SERVER_PORT") || "3000");
    const auto serverPort = findNextAvailablePort(desiredPort);
    if (serverPort != desiredPort) {
        std::cout << "Port " + desiredPort << "using port ${serverPort} instead" << std::endl;
    }
    std::getenv("SERVER_PORT") = std::to_string(serverPort);
    server.start(serverPort);

    // If we have project agents, start them with their init functions
    if (options.projectAgents && options.projectAgents.size() > 0) {
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
