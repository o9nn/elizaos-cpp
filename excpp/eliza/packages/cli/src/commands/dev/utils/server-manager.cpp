#include "server-manager.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

DevServerManager getServerManager() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!serverInstance) {
        serverInstance = new DevServerManager();
    }
    return serverInstance;

}

std::future<void> stopServer() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto server = getServerManager();
    server.stop();

}

std::future<void> startServer(const std::vector<string[] =>& args) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto server = getServerManager();
    server.start(args);

}

std::future<void> restartServer(const std::vector<string[] =>& args) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto server = getServerManager();
    server.restart(args);

}

} // namespace elizaos
