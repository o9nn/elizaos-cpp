#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Socket message types (must match server's SOCKET_MESSAGE_TYPE enum)

class SocketManager {
public:
    void connect(const std::string& userId);
    void joinChannel(const std::string& channelId, const std::string& serverId, std::optional<Record<string> metadata, auto any>);
    void leaveChannel(const std::string& channelId);
    void sendMessage(const std::string& channelId, const std::string& message, const std::string& serverId, std::optional<Record<string> metadata, auto any>);
    void if(auto !this.socket);
    void disconnect();
    bool isConnected();
};


} // namespace elizaos
