#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "agent.hpp"
#include "entity.hpp"
#include "room.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Defines the schema for the "participants" table in the database.
 *
 * @type {import('knex').TableBuilder}
 */
    // unique("participants_user_room_agent_unique").on(table.entityId, table.roomId, table.agentId),

} // namespace elizaos
