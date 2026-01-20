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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use




class TavilyService {
public:
    TavilyService(IAgentRuntime runtime);
    static std::future<TavilyService> start(IAgentRuntime runtime);
    std::future<void> initialize(IAgentRuntime runtime);
    std::string capabilityDescription() const;
    std::future<void> stop();
    std::future<SearchResponse> search(const std::string& query, std::optional<SearchOptions> options);
};


} // namespace elizaos
