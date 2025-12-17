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
#include "elizaos/core.hpp"
#include "guards.hpp"
#include "service.hpp"
#include "templates.hpp"
#include "utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class BehaviorManager {
public:
    BehaviorManager(IAgentRuntime runtime);
    void start();
    void stop();
    std::future<void> runLoop();
    void getService();
    std::future<void> executeBehavior();

private:
    IAgentRuntime runtime_;
};


} // namespace elizaos
