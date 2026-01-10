#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-SQL_SRC___TESTS___TEST-HELPERS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-SQL_SRC___TESTS___TEST-HELPERS_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"
#include "drizzle-orm.h"
#include "node:fs.h"
using fs = _default;
#include "node:os.h"
using os = _default;
#include "node:path.h"
using path = _default;
#include "uuid.h"
#include "../index.h"
using sqlPlugin = plugin;
#include "../migration-service.h"
#include "../pg/adapter.h"
#include "../pg/manager.h"
#include "../pglite/adapter.h"
#include "../pglite/manager.h"
#include "./fixtures.h"

std::shared_ptr<Promise<object>> createTestDatabase(std::shared_ptr<UUID> testAgentId, array<std::shared_ptr<Plugin>> testPlugins = array<std::shared_ptr<Plugin>>());

std::shared_ptr<Promise<object>> createIsolatedTestDatabase(string testName, array<std::shared_ptr<Plugin>> testPlugins = array<std::shared_ptr<Plugin>>());

#endif
