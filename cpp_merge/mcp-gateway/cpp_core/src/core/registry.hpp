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
#include "types/index.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class GatewayRegistry {
public:
    GatewayRegistry(GatewayConfig config, Console logger = console);
    std::future<void> refresh(const std::unordered_map<std:, ServerConnection>& connections);
    std::future<void> collectTools(const std:& serverId, ServerConnection connection);
    std::future<void> collectResources(const std:& serverId, ServerConnection connection);
    std::future<void> collectPrompts(const std:& serverId, ServerConnection connection);
    std: getNameWithNamespace(const std:& name, std::optional<std:> namespace);
    std: getUriWithNamespace(const std:& uri, std::optional<std:> namespace);
    std::vector<AggregatedTool> getTools();
    std::vector<AggregatedResource> getResources();
    std::vector<AggregatedPrompt> getPrompts();
    AggregatedTool findTool(const std:& name);
    AggregatedResource findResource(const std:& uri);
    AggregatedPrompt findPrompt(const std:& name);
     getStats();

private:
    GatewayConfig config_;
    Console logger_;
};


} // namespace elizaos
