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



// This should be called daily via a cron job (e.g., Vercel Cron or external scheduler)
// It checks for matured OTC and claims them on behalf of users

std::future<void> GET(NextRequest request);

// Also support POST for some cron services
std::future<void> POST(NextRequest request);

} // namespace elizaos
