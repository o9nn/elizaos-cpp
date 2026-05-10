#pragma once

/**
 * ElizaOS C++ - AgentComms Module
 *
 * Inter-agent messaging: channels, servers, message types, and the
 * AgentComms coordinator class.
 */

#include "elizaos.hpp"
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <ostream>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace elizaos {

// ============================================================================
// Type aliases
// ============================================================================

using AgentId   = std::string;
using ChannelId = std::string;
using ServerId  = std::string;

// ============================================================================
// MessageType
// ============================================================================

enum class MessageType {
    TEXT,
    DIRECT,
    BROADCAST,
    CHANNEL,
    SYSTEM,
    RESPONSE,
    PING,
    PONG
};

inline std::ostream& operator<<(std::ostream& os, MessageType t) {
    switch (t) {
        case MessageType::TEXT:      return os << "TEXT";
        case MessageType::DIRECT:    return os << "DIRECT";
        case MessageType::BROADCAST: return os << "BROADCAST";
        case MessageType::CHANNEL:   return os << "CHANNEL";
        case MessageType::SYSTEM:    return os << "SYSTEM";
        case MessageType::RESPONSE:  return os << "RESPONSE";
        case MessageType::PING:      return os << "PING";
        case MessageType::PONG:      return os << "PONG";
        default:                     return os << "UNKNOWN";
    }
}

// ============================================================================
// MessageValidationResult
// ============================================================================

struct MessageValidationResult {
    bool        valid = true;
    std::string reason;
};

// ============================================================================
// Message
// ============================================================================

struct Message {
    UUID        id;
    MessageType type       = MessageType::TEXT;
    AgentId     sender;
    AgentId     receiver;
    std::string channel_id;
    std::string content;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;

    Message() = default;
    Message(const UUID& id,
            MessageType type,
            const AgentId& sender,
            const AgentId& receiver,
            const std::string& content_or_channel,
            const std::string& content = "");

    void        setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool        hasMetadata(const std::string& key) const;
};

// ============================================================================
// AgentParticipation
// ============================================================================

struct AgentParticipation {
    AgentId               agent_id;
    std::set<ChannelId>   participating_channels;
    std::set<ServerId>    subscribed_servers;

    bool isParticipatingInChannel(const ChannelId& channel_id) const;
    bool isSubscribedToServer(const ServerId& server_id) const;
    void addChannelParticipation(const ChannelId& channel_id);
    void removeChannelParticipation(const ChannelId& channel_id);
    void addServerSubscription(const ServerId& server_id);
    void removeServerSubscription(const ServerId& server_id);
};

// ============================================================================
// UUIDMapper
// ============================================================================

class UUIDMapper {
public:
    static UUID generateUUID();
    static UUID createAgentSpecificUUID(const AgentId& agent_id, const std::string& resource_id);
};

// ============================================================================
// CommChannel
// ============================================================================

using MessageHandler   = std::function<void(const Message&)>;
using MessageValidator = std::function<MessageValidationResult(const Message&)>;

class CommChannel {
public:
    CommChannel(const ChannelId& channelId, const ServerId& serverId = "");

    const ChannelId& getChannelId() const { return channelId_; }
    const ServerId&  getServerId()  const { return serverId_; }
    bool             isActive()     const { return active_; }

    bool sendMessage(const Message& message, bool validate = true);
    void setMessageHandler(MessageHandler handler);
    void setMessageValidator(MessageValidator validator);

    void addParticipant(const AgentId& agent_id);
    void removeParticipant(const AgentId& agent_id);
    bool isParticipant(const AgentId& agent_id) const;
    std::vector<AgentId> getParticipants() const;

    void start();
    void stop();

private:
    void processMessages();
    MessageValidationResult validateMessage(const Message& message) const;

    ChannelId          channelId_;
    ServerId           serverId_;
    std::atomic<bool>  active_{false};

    std::vector<Message> messageQueue_;
    std::set<AgentId>    participants_;
    MessageHandler       messageHandler_;
    MessageValidator     messageValidator_;

    mutable std::mutex              channelMutex_;
    std::unique_ptr<std::thread>    processingThread_;
    std::atomic<bool>               stopProcessing_{false};
};

// ============================================================================
// TCPConnector (basic TCP connectivity)
// ============================================================================

class TCPConnector {
public:
    TCPConnector();
    bool connect(const std::string& connectionString);
    void disconnect();
    bool isConnected() const { return connected_; }
    bool sendData(const std::string& data);
    std::string receiveData();

private:
    std::atomic<bool> connected_{false};
};

// ============================================================================
// AgentComms – main communication coordinator
// ============================================================================

class AgentComms {
public:
    explicit AgentComms(const AgentId& agent_id = "");
    ~AgentComms() = default;

    void setAgentId(const AgentId& agent_id);
    const AgentId& getAgentId() const { return agent_id_; }

    // Channel management
    std::shared_ptr<CommChannel> createChannel(const ChannelId& channelId,
                                               const ServerId&  serverId = "");
    std::shared_ptr<CommChannel> getChannel(const ChannelId& channelId);
    std::shared_ptr<CommChannel> getChannel(const ChannelId& channelId) const;
    void                         removeChannel(const ChannelId& channelId);
    std::vector<ChannelId>       getActiveChannels() const;

    // Messaging
    bool sendMessage(const ChannelId& channelId, const Message& message, bool validate = true);
    void broadcastMessage(const Message& message, bool validate = true);

    // Participant management
    bool addChannelParticipant(const ChannelId& channelId, const AgentId& agent_id);
    bool removeChannelParticipant(const ChannelId& channelId, const AgentId& agent_id);
    bool isChannelParticipant(const ChannelId& channelId, const AgentId& agent_id) const;

    // Server subscriptions
    void subscribeToServer(const ServerId& serverId, const AgentId& agent_id);
    void unsubscribeFromServer(const ServerId& serverId, const AgentId& agent_id);
    bool isSubscribedToServer(const ServerId& serverId, const AgentId& agent_id) const;

    // Global handlers
    void setGlobalMessageHandler(MessageHandler handler);
    void setGlobalMessageValidator(MessageValidator validator);

    // Lifecycle
    void start();
    void stop();
    bool isStarted() const { return started_; }

    // UUID helpers
    UUID createAgentSpecificUUID(const std::string& resource_id) const;

    // Validation
    MessageValidationResult validateMessage(const Message& message,
                                            const AgentId& target_agent_id = "") const;

private:
    AgentParticipation& getOrCreateParticipation(const AgentId& agent_id);

    AgentId agent_id_;
    std::atomic<bool> started_{false};

    std::unordered_map<ChannelId, std::shared_ptr<CommChannel>> channels_;
    std::unordered_map<AgentId,   AgentParticipation>            participations_;

    MessageHandler   globalMessageHandler_;
    MessageValidator globalMessageValidator_;

    mutable std::mutex commsMutex_;
};

// Global instance
extern std::shared_ptr<AgentComms> globalComms;

} // namespace elizaos
