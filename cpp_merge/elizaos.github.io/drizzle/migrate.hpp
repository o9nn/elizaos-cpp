#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_DRIZZLE_MIGRATE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_DRIZZLE_MIGRATE_H
#include "core.hpp"
#include "drizzle-orm/bun-sqlite.h"
#include "bun:sqlite.h"
// Using alias removed (invalid transpilation)
// Node.js module removed - use C++ filesystem
// Using alias removed (invalid transpilation)
// Node.js module removed - use C++ filesystem
// Using alias removed (invalid transpilation)
// Node.js module removed - use C++ filesystem
// Using alias removed (invalid transpilation)

class MigrationMeta;

class MigrationMeta : public object, public std::enable_shared_from_this<MigrationMeta> {
public:
    using std::enable_shared_from_this<MigrationMeta>::shared_from_this;
    array<string> sql;

    boolean bps;

    double folderMillis;

    string hash;
};

extern std::function<void(any, any, any)> customMigrate;
extern string dbPath;
extern any dbDir;
extern any sqlite;
extern any db;
extern string maxMigrationArg;
extern any maxMigration;
#endif
