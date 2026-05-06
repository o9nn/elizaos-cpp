#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".lib/base-client.hpp"
#include ".types/runs.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class RunsService {
public:
    Promise< listRuns(UUID agentId, std::optional<ListRunsParams> params);
    std::future<RunDetail> getRun(UUID agentId, UUID runId, std::optional<UUID> roomId);
};


} // namespace elizaos
