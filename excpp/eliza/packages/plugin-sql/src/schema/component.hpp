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
  entityId: uuid('entityId')
    .references(() => entityTable.id, { onDelete: 'cascade' })
    .notNull(),
  agentId: uuid('agentId')
    .references(() => agentTable.id, { onDelete: 'cascade' })
    .notNull(),
  roomId: uuid('roomId')
    .references(() => roomTable.id, { onDelete: 'cascade' })
    .notNull(),
  worldId: uuid('worldId').references(() => worldTable.id, { onDelete: 'cascade' }),
  sourceEntityId: uuid('sourceEntityId').references(() => entityTable.id, { onDelete: 'cascade' }),

  // Data
  type: text('type').notNull(),
  data: jsonb('data').default(sql`'{}'::jsonb`),

  // Timestamps
  createdAt: timestamp('createdAt')
    .default(sql`now()`)
    .notNull(),
});

} // namespace elizaos
