#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-sql/src/pg/manager.h"

PostgresConnectionManager::PostgresConnectionManager(string connectionString) {
    this->pool = std::make_shared<Pool>(object{
        object::pair{std::string("connectionString"), std::string("connectionString")}
    });
    this->db = drizzle(as<any>(this->pool), object{
        object::pair{std::string("casing"), std::string("snake_case")}
    });
}

std::shared_ptr<NodePgDatabase> PostgresConnectionManager::getDatabase()
{
    return this->db;
}

std::shared_ptr<Pool> PostgresConnectionManager::getConnection()
{
    return this->pool;
}

std::shared_ptr<Promise<std::shared_ptr<PoolClient>>> PostgresConnectionManager::getClient()
{
    return this->pool->connect();
}

std::shared_ptr<Promise<boolean>> PostgresConnectionManager::testConnection()
{
    auto client = nullptr;
    {
        utils::finally __finally951_1013([&]() mutable
        {
            if (client) {
                client["release"]();
            }
        });
        try
        {
            client = std::async([=]() { this->pool->connect(); });
            std::async([=]() { client["query"](std::string("SELECT 1")); });
            return true;
        }
        catch (const any& error)
        {
            logger->error(std::string("Failed to connect to the database:"), error);
            return false;
        }
    }
}

std::shared_ptr<Promise<void>> PostgresConnectionManager::close()
{
    std::async([=]() { this->pool->end(); });
    return std::shared_ptr<Promise<void>>();
}

