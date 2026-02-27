#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_SQL_SRC___TESTS___TEST_HELPERS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_SQL_SRC___TESTS___TEST_HELPERS_H
#include "core.hpp"
// External dependency removed
// External dependency removed
#include "drizzle-orm.h"
#include "node:fs.h"
// Using alias removed (invalid transpilation)
#include "node:os.h"
// Using alias removed (invalid transpilation)
#include "node:path.h"
// Using alias removed (invalid transpilation)
#include "uuid.hpp"
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
