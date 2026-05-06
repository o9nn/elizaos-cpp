#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "base.hpp"
#include "elizaos/core.hpp"
#include "schema/embedding.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * PgliteDatabaseAdapter class represents an adapter for interacting with a PgliteDatabase.
 * Extends BaseDrizzleAdapter.
 *
 * @constructor
 * @param {UUID} agentId - The ID of the agent.
 * @param {PGliteClientManager} manager - The manager for the Pglite client.
 *
 * @method withDatabase
 * @param {() => Promise<T>} operation - The operation to perform on the database.
 *
 * @method init
 *
 * @method close
 */
class PgliteDatabaseAdapter {
public:
    PgliteDatabaseAdapter(UUID agentId, PGliteClientManager manager);
    std::future<void> runMigrations();
    std::future<void> init();
    std::future<bool> isReady();
    void close();
    void getConnection();

private:
    PGliteClientManager manager_;
};


} // namespace elizaos
