#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Extended Memory type with optional createdAt

// Next.js route context type
struct RouteContext {
};

// POST /api/rooms/[roomId]/messages - Send a message
std::future<void> POST(Request request, RouteContext ctx);

// GET /api/rooms/[roomId]/messages - Get messages (for polling)
std::future<void> GET(Request request, RouteContext ctx);

} // namespace elizaos
