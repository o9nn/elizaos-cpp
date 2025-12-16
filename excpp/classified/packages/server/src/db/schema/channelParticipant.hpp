#include "channel.hpp"
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

const channelParticipantsTable = pgTable(
  'channel_participants',
  {
    channelId: text('channel_id')
      .notNull()
      .references(() => channelTable.id, { onDelete: 'cascade' }),
    userId: text('user_id').notNull(), // This is a central UUID (can be an agentId or a dedicated central user ID)
  },
  (table) => ({
    pk: primaryKey({ columns: [table.channelId, table.userId] }),
  })
);

} // namespace elizaos
