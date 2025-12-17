#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// GET /api/rooms - Get user's rooms
std::future<void> GET(NextRequest request);

// POST /api/rooms - Create new room
std::future<void> POST(NextRequest request);

} // namespace elizaos
