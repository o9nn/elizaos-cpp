#include "index.hpp"
#include <string>

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
            object::pair{std::string("dataDir"), std::string("dataDir")}
        });
    }
    return std::make_shared<PgliteDatabaseAdapter>(agentId, globalSingletons->pgLiteClientManager);
};


GLOBAL_SINGLETONS GLOBAL_SINGLETONS = Symbol->for(std::string("@elizaos/plugin-sql/global-singletons"));
std::shared_ptr<Record<any, std::shared_ptr<GlobalSingletons>>> globalSymbols = as<Record<any, std::shared_ptr<GlobalSingletons>>>(as<any>(global));
any globalSingletons = (*const_(globalSymbols))[GLOBAL_SINGLETONS];
std::shared_ptr<Plugin> plugin = object{
    object::pair{std::string("name"), std::string("@elizaos/plugin-sql")}, 
    object::pair{std::string("description"), std::string("A plugin for SQL database access with dynamic schema migrations")}, 
    object::pair{std::string("priority"), 0}, 
    object::pair{std::string("schema"), std::string("schema")}, 
    object::pair{std::string("init"), [=](auto _, auto runtime) mutable
    {
        logger->info(std::string("plugin-sql init starting..."));
        try
        {
            auto existingAdapter = (as<any>(runtime))["databaseAdapter"];
            if (existingAdapter) {
                logger->info(std::string("Database adapter already registered, skipping creation"));
                return std::shared_ptr<Promise<void>>();
            }
        }
        catch (const any& error)
        {
        }
        auto postgresUrl = runtime->getSetting(std::string("POSTGRES_URL"));
        auto dataDir = OR((OR((runtime->getSetting(std::string("PGLITE_PATH"))), (runtime->getSetting(std::string("DATABASE_PATH"))))), (std::string("./.eliza/.elizadb")));
        auto dbAdapter = createDatabaseAdapter(object{
            object::pair{std::string("dataDir"), std::string("dataDir")}, 
            object::pair{std::string("postgresUrl"), std::string("postgresUrl")}
        }, runtime->agentId);
        runtime->registerDatabaseAdapter(dbAdapter);
        logger->info(std::string("Database adapter created and registered"));
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
