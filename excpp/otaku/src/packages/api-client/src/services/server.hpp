#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "lib/base-client.hpp"
#include "types/server.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class ServerService {
public:
    std::future<ServerHealth> checkHealth();
    Promise< ping();
    Promise< hello();
    std::future<ServerStatus> getStatus();
    Promise< stopServer();
    std::future<ServerDebugInfo> getDebugInfo();
    Promise< submitLogs(const std::vector<LogSubmitParams>& logs);
    Promise< clearLogs();
};


} // namespace elizaos
