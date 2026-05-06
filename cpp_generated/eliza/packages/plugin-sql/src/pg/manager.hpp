#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-SQL_SRC_PG_MANAGER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-SQL_SRC_PG_MANAGER_H
#include "core.h"
#include "drizzle-orm/node-postgres.h"
#include "pg.h"
#include "@elizaos/core.h"

class PostgresConnectionManager;

class PostgresConnectionManager : public object, public std::enable_shared_from_this<PostgresConnectionManager> {
public:
    using std::enable_shared_from_this<PostgresConnectionManager>::shared_from_this;
    std::shared_ptr<Pool> pool;

    std::shared_ptr<NodePgDatabase> db;

    PostgresConnectionManager(string connectionString);
    virtual std::shared_ptr<NodePgDatabase> getDatabase();
    virtual std::shared_ptr<Pool> getConnection();
    virtual std::shared_ptr<Promise<std::shared_ptr<PoolClient>>> getClient();
    virtual std::shared_ptr<Promise<boolean>> testConnection();
    virtual std::shared_ptr<Promise<void>> close();
};

#endif
