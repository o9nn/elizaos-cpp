#include "agent.hpp"
#include "entity.hpp"
#include "room.hpp"
#include "world.hpp"
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
;
;
;

/**
 * Represents a component table in the database.
 */
const componentTable = pgTable('components', {
  id: uuid('id')
    .primaryKey()
    .default(sql`gen_random_uuid()`)
    .notNull(),

  // Foreign keys
  entityId: uuid('entity_id')
    .references(() => entityTable.id, { onDelete: 'cascade' })
    .notNull(),
  agentId: uuid('agent_id')
    .references(() => agentTable.id, { onDelete: 'cascade' })
    .notNull(),
  roomId: uuid('room_id')
    .references(() => roomTable.id, { onDelete: 'cascade' })
    .notNull(),
  worldId: uuid('world_id').references(() => worldTable.id, { onDelete: 'cascade' }),
  sourceEntityId: uuid('source_entity_id').references(() => entityTable.id, {
    onDelete: 'cascade',
  }),

  // Data
  type: text('type').notNull(),
  data: jsonb('data').default(sql`'{}'::jsonb`),

  // Timestamps
  createdAt: timestamp('created_at')
    .default(sql`now()`)
    .notNull(),
});

} // namespace elizaos
