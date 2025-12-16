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
 * Represents a table schema for tasks in the database.
 *
 * @type {PgTable}
 */
const taskTable = pgTable('tasks', {
  id: uuid('id').primaryKey().defaultRandom(),
  name: text('name').notNull(),
  description: text('description'),
  roomId: uuid('room_id'),
  worldId: uuid('world_id'),
  entityId: uuid('entity_id'),
  agentId: uuid('agent_id')
    .notNull()
    .references(() => agentTable.id, { onDelete: 'cascade' }),
  tags: text('tags')
    .array()
    .default(sql`'{}'::text[]`),
  metadata: jsonb('metadata').default(sql`'{}'::jsonb`),
  createdAt: timestamp('created_at', { withTimezone: true }).defaultNow(),
  updatedAt: timestamp('updated_at', { withTimezone: true }).defaultNow(),
});

} // namespace elizaos
