#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/pglite/adapter.h"

PgliteDatabaseAdapter::PgliteDatabaseAdapter(std::shared_ptr<UUID> agentId, std::shared_ptr<PGliteClientManager> manager) : BaseDrizzleAdapter(agentId) {
    this->manager = manager;
    this->db = drizzle(as<any>(this->manager->getConnection()));
}

std::shared_ptr<Promise<void>> PgliteDatabaseAdapter::runMigrations()
{
    logger->debug(std::string("PgliteDatabaseAdapter: Migrations are handled by the migration service"));
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> PgliteDatabaseAdapter::init()
{
    logger->debug(std::string("PGliteDatabaseAdapter initialized, skipping automatic migrations."));
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

