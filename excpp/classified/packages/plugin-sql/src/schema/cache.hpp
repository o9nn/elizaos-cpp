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
 * Represents a PostgreSQL table for caching data.
 *
 * @type {pgTable}
 */
const cacheTable = pgTable(
  'cache',
  {
    key: text('key').notNull(),
    agentId: uuid('agent_id')
      .notNull()
      .references(() => agentTable.id, { onDelete: 'cascade' }),
    value: jsonb('value').notNull(),
    createdAt: timestamp('created_at', { withTimezone: true })
      .default(sql`now()`)
      .notNull(),
    expiresAt: timestamp('expires_at', { withTimezone: true }),
  },
  (table) => ({
    pk: primaryKey({ columns: [table.key, table.agentId] }),
  })
);

} // namespace elizaos
