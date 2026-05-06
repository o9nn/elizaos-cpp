#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types/index.hpp"
#include "payment-middleware.hpp"
#include "registry.hpp"
#include "server-manager.hpp"
#include "x402-client.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class GatewayServer {
public:
    GatewayServer(GatewayConfig config, Console = console logger);
    void setupHandlers();
    std::future<void> start();
    std::future<void> connect(StdioServerTransport transport);
    std::future<void> performHealthCheck();
    void logStatus();
    void getStatus();
    std::future<void> refreshRegistry();
    std::future<void> stop();

private:
    Server server_;
    ServerManager serverManager_;
    GatewayRegistry registry_;
    GatewayConfig config_;
    Console logger_;
};


} // namespace elizaos
