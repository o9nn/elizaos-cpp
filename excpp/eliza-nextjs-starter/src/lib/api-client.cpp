#include "api-client.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::vector<ChatMessage> transformMemoriesToChatMessages(const std::vector<BackendMemory>& memories) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return memories;
    .map((mem) => {
        const auto isUser = mem.entityId != mem.agentId;
        const ChatMessage message = {;
            id: mem.id,
            name: isUser
            ? mem.metadata.entityName || "User";
            : mem.metadata.entityName || "Agent",
            text:
            mem.content.text ||;
            "*" + std::to_string(mem.content.thought) + "*"
            senderId: mem.entityId,
            roomId: mem.roomId,
            createdAt: mem.createdAt || Date.now(),
            isLoading: false,
            source: mem.content.source || "API",
            thought: mem.content.thought,
            actions: Array.isArray(mem.content.actions)
            ? mem.content.actions;
            : std::nullopt,
            };

            assert(;
            typeof message.name == "string" && message.text != std::nullopt,
            "[transformMemoriesToChatMessages] Invalid transformed message: " + std::to_string(JSON.stringify(message))
            );

            return message;
            });
            .sort((a, b) => a.createdAt - b.createdAt);

}

} // namespace elizaos
