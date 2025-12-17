#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
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



// WebSocket message interfaces
struct WebSocketConnectMessage {
    std::optional<std::string> agent_id;
    std::optional<std::string> channel_id;
};

struct WebSocketChannelJoinMessage {
    std::optional<std::string> roomId;
    std::optional<std::string> agentId;
};

struct WebSocketUserMessage {
    std::string content;
    std::optional<std::string> author;
    std::optional<std::string> channel_id;
    std::optional<std::string> agent_id;
    std::optional<double> timestamp;
};

struct WebSocketSendMessage {
    std::string content;
    std::optional<std::string> senderName;
    std::optional<std::string> author;
    std::optional<std::string> roomId;
    std::optional<std::string> channelId;
    std::optional<std::string> agentId;
    std::optional<double> timestamp;
};

struct ProcessAgentMessageData {
    std::string content;
    std::string author;
    std::optional<std::string> channel_id;
    std::optional<std::string> agent_id;
    double timestamp;
};

struct WebSocketOutgoingMessage {
    std::string type;
};

class WebSocketRouter {
public:
    WebSocketRouter(const std::unordered_map<UUID, IAgentRuntime>& agents, AgentServer serverInstance);
    void setupServer(WebSocketServer wss);
    void handleNewConnection(WebSocket ws, IncomingMessage request);
    void handleMessage(WebSocket ws, const std::vector<uint8_t>& data);
    void handleConnect(WebSocket ws, WebSocketConnectMessage message);
    void handleChannelJoining(WebSocket ws, WebSocketChannelJoinMessage message);
    void handleUserMessage(WebSocket ws, WebSocketUserMessage message);
    void handleSendMessage(WebSocket ws, WebSocketSendMessage message);
    void processAgentMessage(WebSocket ws, ProcessAgentMessageData messageData);
    void sendMessage(WebSocket ws, WebSocketOutgoingMessage message);
    void sendError(WebSocket ws, const std::string& error);
    void broadcastMessage(UUID agentId, const std::string& channelId, WebSocketOutgoingMessage message);

private:
    std::unordered_map<UUID, IAgentRuntime> agents_;
    AgentServer serverInstance_;
};


} // namespace elizaos
