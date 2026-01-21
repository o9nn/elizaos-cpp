#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_DATA_DB_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_DATA_DB_H
#include "core.h"
#include "better-sqlite3.h"
using Database = _default;
#include "drizzle-orm/better-sqlite3.h"
#include "path.h"
using path = _default;
#include "./schema.h"

extern std::shared_ptr<Database::Database> sqlite;
extern any db;
#endif
