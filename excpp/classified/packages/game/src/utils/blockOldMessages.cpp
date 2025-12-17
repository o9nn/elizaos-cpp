#include "blockOldMessages.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void blockOldMessages() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "🛡️ Installing WebSocket message blocker..." << std::endl;

    // Intercept WebSocket send method
    const auto originalSend = WebSocket.prototype.send;

    WebSocket.prototype.send = function (data: string | ArrayBufferLike | Blob | ArrayBufferView) {
        // Try to parse and check the message
        try {
            std::optional<WebSocketMessage> message = nullptr;
            if (typeof data == 'string') {
                message = /* JSON.parse */ data;
            }

            // Block specific message types or content
            if (message && message.type == 'send_message') {
                const auto messageText = message.message.text || message.message.content || "";
                if (messageText.includes('admin has opened the terminal')) {
                    std::cerr << "🚫 BLOCKED problematic message!" << message << std::endl;
                    console.trace();
                    return; // Don't send it;
                }
            }
            } catch (_e) {
                // Not JSON or couldn't parse, let it through
            }

            // Allow other messages
            return originalSend.apply(this, [data]);
            };

            std::cout << "✅ WebSocket message blocker installed" << std::endl;

}

} // namespace elizaos
