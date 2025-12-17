#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void createAgent(Character character, const std::any& db, const std::any& cache, const std::string& token) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    elizaLogger.success(;
    elizaLogger.successesTitle,
    "Creating runtime for character",
    character.name,
    );

    nodePlugin ||= createNodePlugin();

    return new AgentRuntime({;
        databaseAdapter: db,
        token,
        modelProvider: character.modelProvider,
        evaluators: [],
        character,
        plugins: [
        bootstrapPlugin,
        nodePlugin,
        character.settings.secrets.WALLET_PUBLIC_KEY ? solanaPlugin : nullptr,
        ].filter(Boolean),
        providers: [],
        actions: [],
        services: [],
        managers: [],
        cacheManager: cache,
        });

}

std::future<void> startAgent(Character character, DirectClient directClient) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            character.id ||= stringToUuid(character.name);
            character.username ||= character.name;

            const auto token = getTokenForProvider(character.modelProvider, character);
            const auto dataDir = path.join(__dirname, "../data");

            if (!fs.existsSync(dataDir)) {
                fs.mkdirSync(dataDir, { recursive: true });
            }

            const auto db = initializeDatabase(dataDir);

            db.init();

            const auto cache = initializeDbCache(character, db);
            const auto runtime = createAgent(character, db, cache, token);

            runtime.initialize();

            runtime.clients = initializeClients(character, runtime);

            directClient.registerAgent(runtime);

            // report to console
            "Started " + std::to_string(character.name) + " as " + std::to_string(runtime.agentId);

            return runtime;
            } catch (error) {
                elizaLogger.error(;
                "Error starting agent for character " + std::to_string(character.name) + ":"
                error,
                );
                std::cerr << error << std::endl;
                throw;
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
