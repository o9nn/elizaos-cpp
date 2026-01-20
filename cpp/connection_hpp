#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_SERVER_SRC_DB_CONNECTION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_SERVER_SRC_DB_CONNECTION_H
#include "core.h"
#include "drizzle-orm/node-postgres.h"
#include "drizzle-orm/pglite.h"
using drizzlePglite = drizzle;
#include "pg.h"
#include "@electric-sql/pglite.h"
#include "@elizaos/core.h"
#include "./schema.h"

typedef any DatabaseConnection;

class ConnectionOptions;

class ConnectionOptions : public object, public std::enable_shared_from_this<ConnectionOptions> {
public:
    using std::enable_shared_from_this<ConnectionOptions>::shared_from_this;
    string postgresUrl;

    string dataDir;
};

extern any db;
extern any pgPool;
extern any pgliteConnection;
std::shared_ptr<Promise<DatabaseConnection>> createDatabaseConnection(std::shared_ptr<ConnectionOptions> options);

DatabaseConnection getDatabase();

std::shared_ptr<Promise<void>> closeDatabase();

#endif
