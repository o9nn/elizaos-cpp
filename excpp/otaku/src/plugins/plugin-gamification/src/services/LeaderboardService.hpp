#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "schema.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class LeaderboardService {
public:
    DatabaseAdapter getDb();
    bool isAgent(UUID userId);
    static std::future<LeaderboardService> start(IAgentRuntime runtime);
    void startSnapshotWorker();
    std::future<void> runSnapshotLoop();
    std::future<void> aggregateSnapshots();
    void scheduleWeeklyReset();
    std::future<void> runWeeklyResetLoop();
    std::future<void> resetWeeklyPoints();
    std::chrono::system_clock::time_point getNextMonday(const std::chrono::system_clock::time_point& date);
    std::future<void> stop();
};


} // namespace elizaos
