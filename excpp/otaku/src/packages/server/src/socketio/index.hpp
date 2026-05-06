#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "utils/media-transformer.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class SocketIORouter {
public:
    SocketIORouter(ElizaOS elizaOS, AgentServer serverInstance);
    void setupListeners(SocketIOServer io);
    void handleNewConnection(Socket socket, SocketIOServer _io);
    void handleGenericMessage(Socket socket, const std::any& data);
    void handleChannelJoining(Socket socket, const std::any& payload);
    void handleMessageSubmission(Socket socket, const std::any& payload);
    void sendErrorResponse(Socket socket, const std::string& errorMessage);
    void handleLogSubscription(Socket socket);
    void handleLogUnsubscription(Socket socket);
    void handleLogFilterUpdate(Socket socket, std::optional<std::any> filters);
    void broadcastLog(SocketIOServer io, const std::any& logEntry);
    void handleDisconnect(Socket socket);

private:
    ElizaOS elizaOS_;
    std::unordered_map<std::string, UUID> connections_;
    AgentServer serverInstance_;
};


} // namespace elizaos
