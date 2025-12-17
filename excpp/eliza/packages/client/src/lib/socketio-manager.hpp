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
#include "elizaos/core.hpp"
#include "logger.hpp"
#include "utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define types for the events
using MessageBroadcastData = {

using MessageCompleteData = {

// Define type for control messages
using ControlMessageData = std::variant<{

// Define type for message deletion events
using MessageDeletedData = {

// Define type for channel cleared events
using ChannelClearedData = {

// Define type for channel deleted events
using ChannelDeletedData = {

// Define type for log stream messages
using LogStreamData = {

// A simple class that provides EventEmitter-like interface using Evt internally
class EventAdapter {
public:
    EventAdapter();
    void emit(const std::string& eventName);
    double listenerCount(const std::string& eventName);
    Evt<any> _getEvt(const std::string& eventName);
};

/**
 * SocketIOManager handles real-time communication between the client and server
 * using Socket.io. It maintains a single connection to the server and allows
 * joining and messaging in multiple rooms.
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
    static SocketIOManager getInstance();
    static bool isConnected();
    bool isChannelActive(const std::string& channelId);
    void initialize(const std::string& clientEntityId);
    std::future<void> joinChannel(const std::string& channelId);
    std::future<void> joinRoom(const std::string& channelId);
    void leaveChannel(const std::string& channelId);
    void leaveRoom(const std::string& channelId);
    std::future<void> sendMessage(const std::string& message, const std::string& channelId, const std::string& serverId, const std::string& source, std::optional<std::vector<std::any>> attachments, std::optional<std::string> messageId, std::optional<std::unordered_map<std::string, std::any>> metadata);
    std::future<void> subscribeToLogStream();
    std::future<void> unsubscribeFromLogStream();
    std::future<void> updateLogStreamFilters(std::optional<std::any> filters);
    bool isLogStreamSubscribed();
    void disconnect();
};


} // namespace elizaos
