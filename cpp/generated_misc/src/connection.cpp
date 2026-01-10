#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/server/src/db/connection.h"

std::shared_ptr<Promise<DatabaseConnection>> createDatabaseConnection(std::shared_ptr<ConnectionOptions> options)
{
    if (db) {
        return db;
    }
    if (options->postgresUrl) {
        logger->info(std::string("[MessageDB] Connecting to PostgreSQL database..."));
        pgPool = std::make_shared<Pool>(object{
            object::pair{std::string("connectionString"), options->postgresUrl}, 
            object::pair{std::string("max"), 10}, 
            object::pair{std::string("idleTimeoutMillis"), 30000}, 
            object::pair{std::string("connectionTimeoutMillis"), 30000}
        });
        db = drizzle(pgPool, object{
            object::pair{std::string("schema"), std::string("schema")}
        });
        logger->success(std::string("[MessageDB] Connected to PostgreSQL database"));
    } else {
        auto dataDir = OR((options->dataDir), (std::string(".eliza/message-server/db")));
        logger->info(std::string("[MessageDB] Initializing PGLite database at: ") + dataDir + string_empty);
        pgliteConnection = std::make_shared<PGlite>(dataDir);
        std::async([=]() { pgliteConnection["waitReady"]; });
        db = drizzlePglite(pgliteConnection, object{
            object::pair{std::string("schema"), std::string("schema")}
        });
        logger->success(std::string("[MessageDB] PGLite database initialized"));
    }
    return db;
};


DatabaseConnection getDatabase()
{
    if (!db) {
        throw any(std::make_shared<Error>(std::string("Database not initialized. Call createDatabaseConnection first.")));
    }
    return db;
};


std::shared_ptr<Promise<void>> closeDatabase()
{
    if (pgPool) {
        std::async([=]() { pgPool["end"](); });
        pgPool = nullptr;
    }
    if (pgliteConnection) {
        std::async([=]() { pgliteConnection["close"](); });
        pgliteConnection = nullptr;
    }
    db = nullptr;
    logger->info(std::string("[MessageDB] Database connection closed"));
    return std::shared_ptr<Promise<void>>();
};


any db = nullptr;
any pgPool = nullptr;
any pgliteConnection = nullptr;

void Main(void)
{
}

MAIN
