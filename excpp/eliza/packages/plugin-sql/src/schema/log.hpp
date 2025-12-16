#include "entity.hpp"
#include "room.hpp"
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

/**
 * Represents a PostgreSQL table for storing logs.
 *
 * @type {Table}
 */

const logTable = pgTable(
  'logs',
  {
    id: uuid('id').defaultRandom().notNull(),
    createdAt: timestamp('created_at', { withTimezone: true })
      .default(sql`now()`)
      .notNull(),
    entityId: uuid('entityId')
      .notNull()
      .references(() => entityTable.id, { onDelete: 'cascade' }),
    body: jsonb('body').notNull(),
    type: text('type').notNull(),
    roomId: uuid('roomId')
      .notNull()
      .references(() => roomTable.id, { onDelete: 'cascade' }),
  },
  (table) => [
    foreignKey({
      name: 'fk_room',
      columns: [table.roomId],
      foreignColumns: [roomTable.id],
    }).onDelete('cascade'),
    foreignKey({
      name: 'fk_user',
      columns: [table.entityId],
      foreignColumns: [entityTable.id],
    }).onDelete('cascade'),
  ]
);

} // namespace elizaos
