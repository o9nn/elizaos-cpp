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



struct RouteParams {
    Promise<{ params;
    std::string sessionId;
};

std::future<void> GET(NextRequest request, RouteParams { params });

} // namespace elizaos
