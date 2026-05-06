#include "debugWebSockets.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void debugWebSockets() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "🔍 Starting WebSocket debugging..." << std::endl;

    // Store the original WebSocket
    const auto OriginalWebSocket = window.WebSocket;

    // Create a wrapper
    (window & Record<std::string, unknown>).WebSocket = std::function (;
    url: std::string,
    protocols?: std::string | std::string[]
    ) {
        std::cout << "🔌 New WebSocket connection:" << url << std::endl;

        const auto ws = new OriginalWebSocket(url, protocols);

        // Intercept send
        const auto originalSend = ws.send.bind(ws);
        ws.send = std::function (data: std::string | ArrayBufferLike | Blob | ArrayBufferView) {
            std::cout << "📤 WebSocket send:" << data << std::endl;

            // Check if this is the problematic message
            if (data && data.toString().includes('admin has opened')) {
                std::cerr << "🚨 FOUND THE CULPRIT! Blocking message:" << data << std::endl;
                console.trace(); // This will show us the call stack
                return; // Block the message;
            }

            return originalSend(data);
            };

            // Log events
            ws.addEventListener('open', () => console.log('✅ WebSocket opened:', url));
            ws.addEventListener('close', () => console.log('❌ WebSocket closed:', url));
            ws.addEventListener("message", (e: MessageEvent) =>
            std::cout << "📥 WebSocket received:" << e.data << std::endl;
            );
            ws.addEventListener('error', (e: Event) => console.error('⚠️ WebSocket error:', e));

            return ws;
            };

            std::cout << "✅ WebSocket debugging enabled" << std::endl;

}

} // namespace elizaos
