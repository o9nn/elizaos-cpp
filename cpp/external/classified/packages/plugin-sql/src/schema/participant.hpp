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



    // Indexes for faster queries
    // Index for finding participants in a room
    // Index for finding all rooms a participant is in

using Participant = typeof participantTable::$inferSelect;
using NewParticipant = typeof participantTable::$inferInsert;


} // namespace elizaos
