#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_DRIZZLE_MIGRATE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_DRIZZLE_MIGRATE_H
#include "core.h"
#include "drizzle-orm/bun-sqlite.h"
#include "bun:sqlite.h"
using Database = _default;
#include "node:crypto.h"
using crypto = _default;
#include "node:fs.h"
using fs = _default;
#include "node:path.h"
using path = _default;

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
