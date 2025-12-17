#include "connection.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<DatabaseConnection> createDatabaseConnection(ConnectionOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (db) {
        return db;
    }

    if (options.postgresUrl) {
        // PostgreSQL connection
        std::cout << '[MessageDB] Connecting to PostgreSQL database...' << std::endl;
        pgPool = new Pool({
            connectionString: options.postgresUrl,
            max: 10,
            idleTimeoutMillis: 30000,
            connectionTimeoutMillis: 30000,
            });

            db = drizzle(pgPool, { schema });
            logger.success('[MessageDB] Connected to PostgreSQL database');
            } else {
                // PGLite connection (default)
                const auto dataDir = options.dataDir || '.eliza/message-server/db';
                std::cout << "[MessageDB] Initializing PGLite database at: " + std::to_string(dataDir) << std::endl;

                pgliteConnection = new PGlite(dataDir);
                pgliteConnection.waitReady;

                db = drizzlePglite(pgliteConnection, { schema });
                logger.success('[MessageDB] PGLite database initialized');
            }

            return db;

}

DatabaseConnection getDatabase() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!db) {
            throw std::runtime_error('Database not initialized. Call createDatabaseConnection first.');
        }
        return db;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> closeDatabase() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (pgPool) {
        pgPool.end();
        pgPool = nullptr;
    }

    if (pgliteConnection) {
        pgliteConnection.close();
        pgliteConnection = nullptr;
    }

    db = nullptr;
    std::cout << '[MessageDB] Database connection closed' << std::endl;

}

} // namespace elizaos
