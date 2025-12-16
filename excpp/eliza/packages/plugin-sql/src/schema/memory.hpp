#include "agent.hpp"
#include "embedding.hpp"
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
;
;

/**
 * Definition of the memory table in the database.
 *
 * @param {string} tableName - The name of the table.
 * @param {object} columns - An object containing the column definitions.
 * @param {function} indexes - A  - The memory table object.
 */
const memoryTable = pgTable(
  'memories',
  {
    id: uuid('id').primaryKey().notNull(),
    type: text('type').notNull(),
    createdAt: timestamp('createdAt')
      .default(sql`now()`)
      .notNull(),
    content: jsonb('content').notNull(),
    entityId: uuid('entityId').references(() => entityTable.id, {
      onDelete: 'cascade',
    }),
    agentId: uuid('agentId')
      .references(() => agentTable.id, {
        onDelete: 'cascade',
      })
      .notNull(),
    roomId: uuid('roomId').references(() => roomTable.id, {
      onDelete: 'cascade',
    }),
    worldId: uuid('worldId'),
    // .references(() => worldTable.id, {
    //   onDelete: 'set null',
    // }),
    unique: boolean('unique').default(true).notNull(),
    metadata: jsonb('metadata').default({}).notNull(),
  },
  (table) => [
    index('idx_memories_type_room').on(table.type, table.roomId),
    index('idx_memories_world_id').on(table.worldId),
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
    foreignKey({
      name: 'fk_agent',
      columns: [table.agentId],
      foreignColumns: [agentTable.id],
    }).onDelete('cascade'),
    // foreignKey({
    //   name: 'fk_world',
    //   columns: [table.worldId],
    //   foreignColumns: [worldTable.id],
    // }).onDelete('set null'),
    index('idx_memories_metadata_type').on(sql`((metadata->>'type'))`),
    index('idx_memories_document_id').on(sql`((metadata->>'documentId'))`),
    index('idx_fragments_order').on(
      sql`((metadata->>'documentId'))`,
      sql`((metadata->>'position'))`
    ),
    check(
      'fragment_metadata_check',
      sql`
            CASE 
                WHEN metadata->>'type' = 'fragment' THEN
                    metadata ? 'documentId' AND 
                    metadata ? 'position'
                ELSE true
            END
        `
    ),
    check(
      'document_metadata_check',
      sql`
            CASE 
                WHEN metadata->>'type' = 'document' THEN
                    metadata ? 'timestamp'
                ELSE true
            END
        `
    ),
  ]
);

const memoryRelations = relations(memoryTable, ({ one }) => ({
  embedding: one(embeddingTable),
}));

} // namespace elizaos
