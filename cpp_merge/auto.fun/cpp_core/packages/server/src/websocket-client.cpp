#include "websocket-client.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

WebSocketClient getWebSocketClient() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!wsClient) {
        wsClient = std::make_unique<WebSocketClient>();
    }
    return wsClient;

}

} // namespace elizaos
