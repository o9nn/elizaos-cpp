#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types/index.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class GatewayRegistry {
public:
    GatewayRegistry(GatewayConfig config, Console logger = console);
    std::future<void> refresh(const std::unordered_map<std::string, ServerConnection>& connections);
    std::future<void> collectTools(const std::string& serverId, ServerConnection connection);
    std::future<void> collectResources(const std::string& serverId, ServerConnection connection);
    std::future<void> collectPrompts(const std::string& serverId, ServerConnection connection);
    std::string getNameWithNamespace(const std::string& name, std::optional<std::string> namespace);
    std::string getUriWithNamespace(const std::string& uri, std::optional<std::string> namespace);
    std::vector<AggregatedTool> getTools();
    std::vector<AggregatedResource> getResources();
    std::vector<AggregatedPrompt> getPrompts();
    AggregatedTool findTool(const std::string& name);
    AggregatedResource findResource(const std::string& uri);
    AggregatedPrompt findPrompt(const std::string& name);
     getStats();

private:
    GatewayConfig config_;
    Console logger_;
};


} // namespace elizaos
