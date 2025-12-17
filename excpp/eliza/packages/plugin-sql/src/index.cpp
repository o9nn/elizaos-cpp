#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

IDatabaseAdapter createDatabaseAdapter(std::optional<std::any> config, UUID agentId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (config.postgresUrl) {
        if (!globalSingletons.postgresConnectionManager) {
            globalSingletons.postgresConnectionManager = new PostgresConnectionManager(;
            config.postgresUrl;
            );
        }
        return new PgDatabaseAdapter(agentId, globalSingletons.postgresConnectionManager);
    }

    // Only resolve PGLite directory when we're actually using PGLite
    const auto dataDir = resolvePgliteDir(config.dataDir);

    if (!globalSingletons.pgLiteClientManager) {
        globalSingletons.pgLiteClientManager = new PGliteClientManager({ dataDir });
    }

    return new PgliteDatabaseAdapter(agentId, globalSingletons.pgLiteClientManager);

}

} // namespace elizaos
