#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Socket message types from ElizaOS core
enum SOCKET_MESSAGE_TYPE {
}

// Direct connection to ElizaOS server for Socket.IO (proxying doesn't work for WebSocket)

// Enhanced types for ElizaOS Socket.IO events (matching official client)
using MessageBroadcastData = {

using MessageCompleteData = {

using ControlMessageData = std::variant<{

using MessageDeletedData = {

using ChannelClearedData = {

using ChannelDeletedData = {

using LogStreamData = {

// A simple class that provides EventEmitter-like interface using Evt internally
class EventAdapter {
public:
    EventAdapter();
    void if(auto !this.events[eventName]);
    void if(auto this.events[eventName]);
    void emit(const std::string& eventName, const std::vector<std::any>& ...args);
    void if(auto !this.events[eventName]);
    double listenerCount(const std::string& eventName);
    Evt<any> _getEvt(const std::string& eventName);
};

/**
 * SocketIOManager handles real-time communication between the client and server
 * using Socket.io. Based on the official ElizaOS client implementation.
 */
class SocketIOManager {
public:
    SocketIOManager();
    void evtMessageBroadcast();
    void evtMessageComplete();
    void evtControlMessage();
    void evtMessageDeleted();
    void evtChannelCleared();
    void evtChannelDeleted();
    void evtLogStream();
    SocketIOManager getInstance();
    void initialize(const std::string& entityId, std::optional<std::string> serverId);
    std::future<void> joinChannel(const std::string& channelId, std::optional<std::string> serverId);
    std::future<void> joinRoom(const std::string& roomId);
    void leaveChannel(const std::string& channelId);
    void leaveRoom(const std::string& roomId);
    std::future<void> sendChannelMessage(const std::string& message, const std::string& channelId, const std::string& source, std::optional<std::string> sessionChannelId, std::optional<std::string> serverId);
    std::future<void> sendMessage(const std::string& message, const std::string& roomId, const std::string& source);
    void subscribeToLogs();
    void unsubscribeFromLogs();
    void updateLogFilters(std::optional<std::any> filters);
    std::unordered_set<std::string> getActiveChannels();
    std::unordered_set<std::string> getActiveRooms();
    bool isSocketConnected();
    std::optional<std::string> getEntityId();
    std::optional<std::string> getServerId();
    void setActiveSessionChannelId(const std::string& sessionChannelId);
    std::optional<std::string> getActiveSessionChannelId();
    void clearActiveSessionChannelId();
    void disconnect();
};


} // namespace elizaos
