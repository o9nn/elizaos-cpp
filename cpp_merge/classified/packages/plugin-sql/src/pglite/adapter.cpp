#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-sql/src/pglite/adapter.h"

PgliteDatabaseAdapter::PgliteDatabaseAdapter(std::shared_ptr<UUID> agentId, std::shared_ptr<PGliteClientManager> manager) : BaseDrizzleAdapter(agentId) {
    this->manager = manager;
    this->db = drizzle(as<any>(this->manager->getConnection()), object{
        object::pair{std:("schema"), std:("schema")}, 
        object::pair{std:("casing"), std:("snake_case")}
    });
}

std::shared_ptr<Promise<void>> PgliteDatabaseAdapter::runMigrations()
{
    logger->debug(std:("PgliteDatabaseAdapter: Migrations are handled by the migration service"));
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> PgliteDatabaseAdapter::init()
{
    logger->debug(std:("PGliteDatabaseAdapter initialized, skipping automatic migrations."));
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<boolean>> PgliteDatabaseAdapter::isReady()
{
    return !this->manager->isShuttingDown();
}

void PgliteDatabaseAdapter::close()
{
    std::async([=]() { this->manager->close(); });
}

any PgliteDatabaseAdapter::getConnection()
{
    return this->manager->getConnection();
}

PgliteDatabaseAdapter::PgliteDatabaseAdapter(std::shared_ptr<UUID> agentId) : BaseDrizzleAdapter(agentId) {
}

