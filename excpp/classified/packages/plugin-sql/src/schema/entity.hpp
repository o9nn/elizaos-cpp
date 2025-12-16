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
 * Represents an entity table in the database.
 * Includes columns for id, agentId, createdAt, names, and metadata.
 */
const entityTable = pgTable(
  'entities',
  {
    id: uuid('id').notNull().primaryKey(),
    agentId: uuid('agent_id')
      .notNull()
      .references(() => agentTable.id, {
        onDelete: 'cascade',
      }),
    createdAt: timestamp('created_at')
      .default(sql`now()`)
      .notNull(),
    updatedAt: timestamp('updated_at')
      .default(sql`now()`)
      .notNull(),
    names: text('names')
      .array()
      .default(sql`'{}'::text[]`)
      .notNull(),
    metadata: jsonb('metadata')
      .default(sql`'{}'::jsonb`)
      .notNull(),
  },
  (table) => {
    return {
      idAgentIdUnique: unique('id_agent_id_unique').on(table.id, table.agentId),
    };
  }
);

} // namespace elizaos
