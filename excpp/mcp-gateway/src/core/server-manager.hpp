#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".transports/factory.hpp"
#include ".types/index.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class ServerManager {
public:
    ServerManager(Console logger = console);
    std::future<void> initializeServers(const std::vector<McpServerConfig>& serverConfigs);
    std::future<void> initializeServer(McpServerConfig config);
    Promise< getServerCapabilities(Client client);
    std::unordered_map<std::string, ServerConnection> getConnectedServers();
    ServerConnection getConnection(const std::string& serverId);
    std::future<void> closeAll();
    std::future<void> performHealthChecks();
     getConnectionStatus();

private:
    Console logger_;
};


} // namespace elizaos
