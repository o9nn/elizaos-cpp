#include "agent.hpp"
#include "messageServer.hpp"
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

const serverAgentsTable = pgTable(
  'server_agents',
  {
    serverId: uuid('server_id')
      .notNull()
      .references(() => messageServerTable.id, { onDelete: 'cascade' }),
    agentId: uuid('agent_id')
      .notNull()
      .references(() => agentTable.id, { onDelete: 'cascade' }),
  },
  (table) => ({
    pk: primaryKey({ columns: [table.serverId, table.agentId] }),
  })
);

} // namespace elizaos
