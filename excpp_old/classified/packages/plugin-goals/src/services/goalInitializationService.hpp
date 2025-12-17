#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "goalService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Service to handle initialization of default goals after database is ready
 */
class GoalInitializationService {
public:
    std::future<void> stop();
    std::future<GoalInitializationService> start(IAgentRuntime runtime);
    std::future<void> createInitialGoalsDeferred(IAgentRuntime runtime);
};


} // namespace elizaos
