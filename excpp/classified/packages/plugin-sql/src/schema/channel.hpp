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

const channelTable = pgTable('channels', {
  id: text('id').primaryKey(), // UUID stored as text
  serverId: uuid('server_id').notNull(),
  // Note: References message_servers table which is managed by agentserver
  name: text('name').notNull(),
  type: text('type').notNull(), // Store ChannelType enum values as text
  sourceType: text('source_type'),
  sourceId: text('source_id'),
  topic: text('topic'),
  metadata: jsonb('metadata'),
  createdAt: timestamp('created_at', { mode: 'date' })
    .default(sql`CURRENT_TIMESTAMP`)
    .notNull(),
  updatedAt: timestamp('updated_at', { mode: 'date' })
    .default(sql`CURRENT_TIMESTAMP`)
    .notNull(),
});

} // namespace elizaos
