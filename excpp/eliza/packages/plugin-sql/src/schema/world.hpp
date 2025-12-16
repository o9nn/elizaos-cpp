#include "agent.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;

/**
 * Represents a table schema for worlds in the database.
 *
 * @type {PgTable}
 */

const worldTable = pgTable('worlds', {
  id: uuid('id')
    .notNull()
    .primaryKey()
    .default(sql`gen_random_uuid()`),
  agentId: uuid('agentId')
    .notNull()
    .references(() => agentTable.id, { onDelete: 'cascade' }),
  name: text('name').notNull(),
  metadata: jsonb('metadata'),
  serverId: text('serverId').notNull().default('local'),
  createdAt: timestamp('createdAt')
    .default(sql`now()`)
    .notNull(),
});

} // namespace elizaos
