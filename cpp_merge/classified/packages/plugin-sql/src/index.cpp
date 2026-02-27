#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-sql/src/index.h"

std::shared_ptr<IDatabaseAdapter> createDatabaseAdapter(object config, std::shared_ptr<UUID> agentId)
{
    if (config["postgresUrl"]) {
        if (!globalSingletons->postgresConnectionManager) {
            globalSingletons->postgresConnectionManager = std::make_shared<PostgresConnectionManager>(config["postgresUrl"]);
        }
        return std::make_shared<PgDatabaseAdapter>(agentId, globalSingletons->postgresConnectionManager);
    }
    auto dataDir = resolvePgliteDir(config["dataDir"]);
    if (!globalSingletons->pgLiteClientManager) {
        globalSingletons->pgLiteClientManager = std::make_shared<PGliteClientManager>(object{
            object::pair{std:("dataDir"), std:("dataDir")}
        });
    }
    return std::make_shared<PgliteDatabaseAdapter>(agentId, globalSingletons->pgLiteClientManager);
};


GLOBAL_SINGLETONS GLOBAL_SINGLETONS = Symbol->for(std:("@elizaos/plugin-sql/global-singletons"));
std::shared_ptr<Record<any, std::shared_ptr<GlobalSingletons>>> globalSymbols = as<Record<any, std::shared_ptr<GlobalSingletons>>>(as<any>(global));
any globalSingletons = (*const_(globalSymbols))[GLOBAL_SINGLETONS];
std::shared_ptr<Plugin> plugin = object{
    object::pair{std:("name"), std:("@elizaos/plugin-sql")}, 
    object::pair{std:("description"), std:("A plugin for SQL database access with dynamic schema migrations")}, 
    object::pair{std:("priority"), 0}, 
    object::pair{std:("schema"), std:("schema")}, 
    object::pair{std:("init"), [=](auto _, auto runtime) mutable
    {
        logger->info(std:("plugin-sql init starting..."));
        try
        {
            auto existingAdapter = (as<any>(runtime))["databaseAdapter"];
            if (existingAdapter) {
                logger->info(std:("Database adapter already registered, skipping creation"));
                return std::shared_ptr<Promise<void>>();
            }
        }
        catch (const any& error)
        {
        }
        auto postgresUrl = runtime->getSetting(std:("POSTGRES_URL"));
        auto dataDir = OR((OR((runtime->getSetting(std:("PGLITE_PATH"))), (runtime->getSetting(std:("DATABASE_PATH"))))), (std:("./.eliza/.elizadb")));
        auto dbAdapter = createDatabaseAdapter(object{
            object::pair{std:("dataDir"), std:("dataDir")}, 
            object::pair{std:("postgresUrl"), std:("postgresUrl")}
        }, runtime->agentId);
        runtime->registerDatabaseAdapter(dbAdapter);
        logger->info(std:("Database adapter created and registered"));
    }
    }
};

void Main(void)
{
    if (!(*const_(globalSymbols))[GLOBAL_SINGLETONS]) {
        (*globalSymbols)[GLOBAL_SINGLETONS] = object{};
    }
}

MAIN
