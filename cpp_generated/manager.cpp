#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/pglite/manager.h"

PGliteClientManager::PGliteClientManager(std::shared_ptr<PGliteOptions> options) {
    this->client = std::make_shared<PGlite>(utils::assign(object{
        , 
        object::pair{std::string("extensions"), object{
            object::pair{std::string("vector"), std::string("vector")}, 
            object::pair{std::string("fuzzystrmatch"), std::string("fuzzystrmatch")}
        }}
    }, options));
    this->setupShutdownHandlers();
}

std::shared_ptr<PGlite> PGliteClientManager::getConnection()
{
    return this->client;
}

boolean PGliteClientManager::isShuttingDown()
{
    return this->shuttingDown;
}

std::shared_ptr<Promise<void>> PGliteClientManager::initialize()
{
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> PGliteClientManager::close()
{
    this->shuttingDown = true;
    return std::shared_ptr<Promise<void>>();
}

void PGliteClientManager::setupShutdownHandlers()
{
}

