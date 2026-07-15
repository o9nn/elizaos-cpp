#pragma once

#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <chrono>
#include <optional>

namespace elizaos {

/**
 * UUID type for agent and message identification
 */
using UUID = std::string;

/**
 * Agent ID type for unique agent identification
 */
using AgentId = std::string;

/**
 * Channel ID type for communication channels
 */
using ChannelId = std::string;

/**
 * Server ID type for server identification
 */
using ServerId = std::string;

/**
 * Message types for agent communication
 */
enum class MessageType {
    TEXT,
    COMMAND,
    RESPONSE,
    STATUS,
    ERROR
};

/**
 * Basic message structure for agent communication
 */
struct Message {
    UUID id;
    MessageType type;
    AgentId sender;
    AgentId receiver;
    ChannelId channel_id;
    ServerId server_id;
    std::string content;
    std::unordered_map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point timestamp;
    UUID in_reply_to_message_id;
    std::string source_type;
    
    Message(
        const UUID& id = "",
        MessageType type = MessageType::TEXT,
        const AgentId& sender = "",
        const AgentId& receiver = "",
        const std::string& content_or_channel = "",
        const std::string& content = ""
    );
    
    // Add metadata helper methods
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
};

/**
 * Message validation result
 */
struct MessageValidationResult {
    bool valid;
    std::string reason;
    
    MessageValidationResult(bool valid = true, const std::string& reason = "") 
        : valid(valid), reason(reason) {}
};

/**
 * Agent participation info for channels and servers
 */
struct AgentParticipation {
    AgentId agent_id;
    std::unordered_set<ChannelId> participating_channels;
    std::unordered_set<ServerId> subscribed_servers;
    
    AgentParticipation(const AgentId& agent_id) : agent_id(agent_id) {}
    
    bool isParticipatingInChannel(const ChannelId& channel_id) const;
    bool isSubscribedToServer(const ServerId& server_id) const;
    void addChannelParticipation(const ChannelId& channel_id);
    void removeChannelParticipation(const ChannelId& channel_id);
    void addServerSubscription(const ServerId& server_id);
    void removeServerSubscription(const ServerId& server_id);
};

/**
 * UUID generation utilities for agent-specific mapping
 */
class UUIDMapper {
public:
    /**
     * Create agent-specific UUID for a resource
     * @param agent_id Agent identifier
     * @param resource_id Global resource identifier
     * @return Agent-specific UUID
     */
    static UUID createAgentSpecificUUID(const AgentId& agent_id, const std::string& resource_id);
    
    /**
     * Generate a unique timestamp-based UUID
     * @return Unique UUID string
     */
    static UUID generateUUID();
};

/**
 * Message handler function type
 */
using MessageHandler = std::function<void(const Message&)>;

/**
 * Message validator function type
 */
using MessageValidator = std::function<MessageValidationResult(const Message&, const AgentId&)>;

/**
 * Communication channel for message passing
 */
class CommChannel {
public:
    CommChannel(const ChannelId& channelId, const ServerId& serverId = "");
    ~CommChannel();
    
    /**
     * Send a message through this channel
     * @param message Message to send
     * @param validate Whether to validate message before sending
     * @return True if message was queued successfully
     */
    bool sendMessage(const Message& message, bool validate = true);
    
    /**
     * Register a handler for incoming messages
     * @param handler Function to call when messages arrive
     */
    void setMessageHandler(MessageHandler handler);
    
    /**
     * Set message validator for this channel
     * @param validator Function to validate messages
     */
    void setMessageValidator(MessageValidator validator);
    
    /**
     * Add agent as participant in this channel
     * @param agent_id Agent to add
     */
    void addParticipant(const AgentId& agent_id);
    
    /**
     * Remove agent from channel participation
     * @param agent_id Agent to remove
     */
    void removeParticipant(const AgentId& agent_id);
    
    /**
     * Check if agent is participant in this channel
     * @param agent_id Agent to check
     * @return True if agent participates
     */
    bool isParticipant(const AgentId& agent_id) const;
    
    /**
     * Get list of participants
     * @return Vector of participant agent IDs
     */
    std::vector<AgentId> getParticipants() const;
    
    /**
     * Start processing messages
     */
    void start();
    
    /**
     * Stop processing messages
     */
    void stop();
    
    /**
     * Get channel ID
     */
    const ChannelId& getChannelId() const { return channelId_; }
    
    /**
     * Get server ID
     */
    const ServerId& getServerId() const { return serverId_; }
    
    /**
     * Check if channel is active
     */
    bool isActive() const { return active_; }
    
private:
    void processMessages();
    MessageValidationResult validateMessage(const Message& message) const;
    
    ChannelId channelId_;
    ServerId serverId_;
    std::atomic<bool> active_;
    std::atomic<bool> stopRequested_;
    
    std::queue<Message> messageQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCondition_;
    
    MessageHandler messageHandler_;
    MessageValidator messageValidator_;
    
    std::unordered_set<AgentId> participants_;
    mutable std::mutex participantsMutex_;
    
    std::unique_ptr<std::thread> processingThread_;
};

/**
 * Communication manager for agent networking
 */
class AgentComms {
public:
    AgentComms(const AgentId& agent_id = "");
    ~AgentComms();
    
    /**
     * Set the agent ID for this communication manager
     * @param agent_id Unique agent identifier
     */
    void setAgentId(const AgentId& agent_id);
    
    /**
     * Get the agent ID
     * @return Current agent ID
     */
    const AgentId& getAgentId() const { return agent_id_; }
    
    /**
     * Create a new communication channel
     * @param channelId Unique identifier for the channel
     * @param serverId Server identifier for the channel
     * @return Shared pointer to the created channel
     */
    std::shared_ptr<CommChannel> createChannel(const ChannelId& channelId, const ServerId& serverId = "");
    
    /**
     * Get existing channel by ID
     * @param channelId Channel identifier
     * @return Shared pointer to channel, or nullptr if not found
     */
    std::shared_ptr<CommChannel> getChannel(const ChannelId& channelId);
    std::shared_ptr<CommChannel> getChannel(const ChannelId& channelId) const;
    
    /**
     * Remove a channel
     * @param channelId Channel identifier
     */
    void removeChannel(const ChannelId& channelId);
    
    /**
     * Send a message to a specific channel
     * @param channelId Target channel
     * @param message Message to send
     * @param validate Whether to validate message before sending
     * @return True if message was sent successfully
     */
    bool sendMessage(const ChannelId& channelId, const Message& message, bool validate = true);
    
    /**
     * Broadcast a message to all channels
     * @param message Message to broadcast
     * @param validate Whether to validate message before sending
     */
    void broadcastMessage(const Message& message, bool validate = true);
    
    /**
     * Add agent as participant to a channel
     * @param channelId Channel identifier
     * @param agent_id Agent to add as participant
     * @return True if successfully added
     */
    bool addChannelParticipant(const ChannelId& channelId, const AgentId& agent_id);
    
    /**
     * Remove agent from channel participation
     * @param channelId Channel identifier
     * @param agent_id Agent to remove
     * @return True if successfully removed
     */
    bool removeChannelParticipant(const ChannelId& channelId, const AgentId& agent_id);
    
    /**
     * Check if agent participates in channel
     * @param channelId Channel identifier
     * @param agent_id Agent to check
     * @return True if agent participates
     */
    bool isChannelParticipant(const ChannelId& channelId, const AgentId& agent_id) const;
    
    /**
     * Subscribe agent to a server
     * @param serverId Server identifier
     * @param agent_id Agent to subscribe (defaults to this agent)
     */
    void subscribeToServer(const ServerId& serverId, const AgentId& agent_id = "");
    
    /**
     * Unsubscribe agent from a server
     * @param serverId Server identifier
     * @param agent_id Agent to unsubscribe (defaults to this agent)
     */
    void unsubscribeFromServer(const ServerId& serverId, const AgentId& agent_id = "");
    
    /**
     * Check if agent is subscribed to server
     * @param serverId Server identifier
     * @param agent_id Agent to check (defaults to this agent)
     * @return True if subscribed
     */
    bool isSubscribedToServer(const ServerId& serverId, const AgentId& agent_id = "") const;
    
    /**
     * Get list of active channel IDs
     */
    std::vector<ChannelId> getActiveChannels() const;
    
    /**
     * Set global message handler (receives all messages)
     * @param handler Function to call for all messages
     */
    void setGlobalMessageHandler(MessageHandler handler);
    
    /**
     * Set global message validator
     * @param validator Function to validate all messages
     */
    void setGlobalMessageValidator(MessageValidator validator);
    
    /**
     * Create agent-specific UUID for a resource
     * @param resource_id Global resource identifier
     * @return Agent-specific UUID
     */
    UUID createAgentSpecificUUID(const std::string& resource_id) const;
    
    /**
     * Start the communication system
     */
    void start();
    
    /**
     * Stop the communication system
     */
    void stop();
    
private:
    MessageValidationResult validateMessage(const Message& message, const AgentId& target_agent_id = "") const;
    AgentParticipation& getOrCreateParticipation(const AgentId& agent_id);
    
    AgentId agent_id_;
    std::unordered_map<ChannelId, std::shared_ptr<CommChannel>> channels_;
    mutable std::mutex channelsMutex_;
    
    std::unordered_map<AgentId, AgentParticipation> participations_;
    mutable std::mutex participationsMutex_;
    
    MessageHandler globalHandler_;
    MessageValidator globalValidator_;
    std::atomic<bool> started_;
};

/**
 * Network connector interface for external communication
 */
class NetworkConnector {
public:
    virtual ~NetworkConnector() = default;
    
    /**
     * Connect to external service
     * @param connectionString Connection details
     * @return True if connection successful
     */
    virtual bool connect(const std::string& connectionString) = 0;
    
    /**
     * Disconnect from external service
     */
    virtual void disconnect() = 0;
    
    /**
     * Send data to external service
     * @param data Data to send
     * @return True if sent successfully
     */
    virtual bool sendData(const std::string& data) = 0;
    
    /**
     * Set handler for incoming data
     * @param handler Function to call when data arrives
     */
    virtual void setDataHandler(std::function<void(const std::string&)> handler) = 0;
    
    /**
     * Check if connected
     */
    virtual bool isConnected() const = 0;
};

/**
 * Simple TCP connector implementation
 */
class TCPConnector : public NetworkConnector {
public:
    TCPConnector();
    ~TCPConnector() override;
    
    bool connect(const std::string& connectionString) override;
    void disconnect() override;
    bool sendData(const std::string& data) override;
    void setDataHandler(std::function<void(const std::string&)> handler) override;
    bool isConnected() const override;
    
private:
    void receiveLoop();
    void closeSocketNoThrow();

    std::atomic<bool> connected_;
    int socket_fd_;
    mutable std::mutex socketMutex_;
    std::thread receiverThread_;
    std::function<void(const std::string&)> dataHandler_;
    mutable std::mutex handlerMutex_;
};

/**
 * Global communication manager instance
 */
extern std::shared_ptr<AgentComms> globalComms;

/**
 * Convenience functions for common communication operations
 */
void initializeComms();
void shutdownComms();
bool sendAgentMessage(const ChannelId& channelId, const std::string& content, const AgentId& sender = "");
void setGlobalMessageReceiver(MessageHandler handler);

/**
 * Message validation utilities
 */
namespace MessageValidation {
    /**
     * Default message validator that implements basic validation rules
     * @param message Message to validate
     * @param agent_id Target agent ID
     * @return Validation result
     */
    MessageValidationResult defaultValidator(const Message& message, const AgentId& agent_id);
    
    /**
     * Validate channel participation
     * @param message Message to validate
     * @param agent_id Target agent ID
     * @param participation Agent participation info
     * @return Validation result
     */
    MessageValidationResult validateChannelParticipation(
        const Message& message, 
        const AgentId& agent_id, 
        const AgentParticipation& participation
    );
    
    /**
     * Validate server subscription
     * @param message Message to validate
     * @param agent_id Target agent ID
     * @param participation Agent participation info
     * @return Validation result
     */
    MessageValidationResult validateServerSubscription(
        const Message& message, 
        const AgentId& agent_id, 
        const AgentParticipation& participation
    );
    
    /**
     * Check if message is self-sent (agent shouldn't process its own messages)
     * @param message Message to validate
     * @param agent_id Target agent ID
     * @return Validation result
     */
    MessageValidationResult validateNotSelfMessage(const Message& message, const AgentId& agent_id);
}

} // namespace elizaos

// ============================================================================
// Phase 3.3: Advanced Inter-Agent Communication
// ============================================================================

namespace elizaos {

/**
 * Structured negotiation protocol for agent agreements
 */
class NegotiationProtocol {
public:
    enum class NegotiationState {
        PROPOSED,
        COUNTER_OFFERED,
        ACCEPTED,
        REJECTED,
        EXPIRED,
        CANCELLED
    };
    
    struct NegotiationTerms {
        std::string proposalId;
        std::string proposer;
        std::string responder;
        std::unordered_map<std::string, std::string> terms;
        double value = 0.0;  // Numeric value if applicable
        std::chrono::system_clock::time_point deadline;
        int maxCounterOffers = 3;
    };
    
    struct NegotiationSession {
        std::string sessionId;
        NegotiationTerms currentTerms;
        std::vector<NegotiationTerms> history;
        NegotiationState state = NegotiationState::PROPOSED;
        int counterOfferCount = 0;
    };
    
    NegotiationProtocol(const AgentId& agentId);
    
    // Initiate negotiation
    std::string propose(const AgentId& responder, const NegotiationTerms& terms);
    
    // Respond to negotiation
    bool accept(const std::string& sessionId);
    bool reject(const std::string& sessionId, const std::string& reason = "");
    bool counterOffer(const std::string& sessionId, const NegotiationTerms& newTerms);
    
    // Query state
    std::optional<NegotiationSession> getSession(const std::string& sessionId) const;
    std::vector<NegotiationSession> getPendingSessions() const;
    std::vector<NegotiationSession> getCompletedSessions() const;
    
    // Callbacks
    using ProposalHandler = std::function<void(const NegotiationSession&)>;
    using ResponseHandler = std::function<void(const std::string& sessionId, NegotiationState state)>;
    void onProposal(ProposalHandler handler);
    void onResponse(ResponseHandler handler);
    
private:
    AgentId agentId_;
    std::unordered_map<std::string, NegotiationSession> sessions_;
    ProposalHandler proposalHandler_;
    ResponseHandler responseHandler_;
    mutable std::mutex negotiationMutex_;
};

/**
 * Contract formation and verification
 */
class ContractManager {
public:
    struct Contract {
        std::string contractId;
        std::vector<AgentId> parties;
        std::unordered_map<std::string, std::string> terms;
        std::unordered_map<AgentId, std::string> signatures;
        std::chrono::system_clock::time_point createdAt;
        std::chrono::system_clock::time_point expiresAt;
        bool fullyExecuted = false;
    };
    
    struct ContractObligation {
        std::string obligationId;
        std::string contractId;
        AgentId responsible;
        std::string description;
        bool fulfilled = false;
        std::chrono::system_clock::time_point deadline;
    };
    
    ContractManager();
    
    // Contract lifecycle
    std::string createContract(const std::vector<AgentId>& parties,
                              const std::unordered_map<std::string, std::string>& terms);
    bool signContract(const std::string& contractId, const AgentId& signer,
                     const std::string& signature);
    bool isFullySigned(const std::string& contractId) const;
    bool verifyContract(const std::string& contractId) const;
    
    // Obligations
    void addObligation(const ContractObligation& obligation);
    bool fulfillObligation(const std::string& obligationId);
    std::vector<ContractObligation> getPendingObligations(const AgentId& agent) const;
    
    // Query
    std::optional<Contract> getContract(const std::string& contractId) const;
    std::vector<Contract> getContractsForAgent(const AgentId& agent) const;
    
private:
    std::unordered_map<std::string, Contract> contracts_;
    std::vector<ContractObligation> obligations_;
    mutable std::mutex contractMutex_;
};

/**
 * Gossip protocol for scalable messaging
 */
class GossipProtocol {
public:
    struct GossipConfig {
        int fanout = 3;              // Number of peers to forward to
        int maxHops = 5;             // Maximum hop count
        std::chrono::seconds messageLifetime{60};
        double redundancyFactor = 0.3;
    };
    
    struct GossipMessage {
        std::string messageId;
        std::string originAgent;
        std::string content;
        std::string topic;
        int hopCount = 0;
        std::chrono::system_clock::time_point createdAt;
        std::vector<AgentId> path;  // Agents that have seen this
    };
    
    GossipProtocol(const AgentId& agentId, const GossipConfig& config);
    ~GossipProtocol();
    
    // Peer management
    void addPeer(const AgentId& peerId);
    void removePeer(const AgentId& peerId);
    std::vector<AgentId> getPeers() const;
    
    // Messaging
    void spread(const std::string& content, const std::string& topic = "");
    
    // Subscriptions
    using MessageHandler = std::function<void(const GossipMessage&)>;
    void subscribe(const std::string& topic, MessageHandler handler);
    void unsubscribe(const std::string& topic);
    
    // Anti-entropy (periodic sync)
    void startAntiEntropy(std::chrono::milliseconds interval);
    void stopAntiEntropy();
    
    // Statistics
    struct GossipStats {
        uint64_t messagesOriginated = 0;
        uint64_t messagesForwarded = 0;
        uint64_t messagesReceived = 0;
        uint64_t duplicatesDropped = 0;
        double averageHopCount = 0.0;
    };
    GossipStats getStats() const;
    
private:
    AgentId agentId_;
    GossipConfig config_;
    std::vector<AgentId> peers_;
    std::unordered_set<std::string> seenMessages_;
    std::unordered_map<std::string, MessageHandler> topicHandlers_;
    GossipStats stats_;
    mutable std::mutex gossipMutex_;
    std::thread antiEntropyThread_;
    std::atomic<bool> running_{false};
    // Interruptible sleep for the anti-entropy loop so stopAntiEntropy()
    // returns promptly instead of blocking a full interval.
    std::mutex antiEntropySleepMutex_;
    std::condition_variable antiEntropyCv_;
    
    void forwardMessage(const GossipMessage& msg);
    std::vector<AgentId> selectPeers(const GossipMessage& msg);
};

/**
 * Message encryption and authentication
 */
class SecureMessaging {
public:
    struct KeyPair {
        std::string publicKey;
        std::string privateKey;
    };
    
    SecureMessaging(const AgentId& agentId);
    
    // Key management
    KeyPair generateKeyPair();
    void setKeyPair(const KeyPair& keys);
    std::string getPublicKey() const;
    void registerPeerPublicKey(const AgentId& peerId, const std::string& publicKey);
    
    // Encryption
    std::string encrypt(const std::string& plaintext, const AgentId& recipient);
    std::string decrypt(const std::string& ciphertext);
    
    // Signing
    std::string sign(const std::string& message);
    bool verify(const std::string& message, const std::string& signature, const AgentId& signer);
    
    // Secure channel
    std::string establishSecureChannel(const AgentId& peerId);
    bool hasSecureChannel(const AgentId& peerId) const;
    
private:
    AgentId agentId_;
    KeyPair keyPair_;
    std::unordered_map<AgentId, std::string> peerPublicKeys_;
    std::unordered_map<AgentId, std::string> secureChannels_;
    mutable std::mutex cryptoMutex_;
};

/**
 * Dead letter handling and message tracing
 */
class MessageTracer {
public:
    struct DeadLetter {
        Message originalMessage;
        std::string failureReason;
        int deliveryAttempts;
        std::chrono::system_clock::time_point lastAttempt;
    };
    
    struct MessageTrace {
        std::string messageId;
        std::vector<std::pair<std::chrono::system_clock::time_point, std::string>> events;
        bool delivered = false;
        double totalLatencyMs = 0.0;
    };
    
    MessageTracer();
    
    // Tracing
    void startTrace(const std::string& messageId);
    void addTraceEvent(const std::string& messageId, const std::string& event);
    void endTrace(const std::string& messageId, bool success);
    std::optional<MessageTrace> getTrace(const std::string& messageId) const;
    
    // Dead letters
    void addDeadLetter(const DeadLetter& dl);
    std::vector<DeadLetter> getDeadLetters() const;
    bool retryDeadLetter(const std::string& messageId);
    void purgeDeadLetters(std::chrono::hours maxAge);
    
    // Statistics
    struct TracingStats {
        uint64_t tracesStarted = 0;
        uint64_t tracesCompleted = 0;
        uint64_t deadLettersCount = 0;
        uint64_t retriesAttempted = 0;
        double averageDeliveryLatency = 0.0;
    };
    TracingStats getStats() const;
    
private:
    std::unordered_map<std::string, MessageTrace> traces_;
    std::vector<DeadLetter> deadLetters_;
    TracingStats stats_;
    mutable std::mutex tracerMutex_;
};

/**
 * Enhanced agent communication system
 */
class EnhancedAgentComms : public AgentComms {
public:
    EnhancedAgentComms(const AgentId& agentId);
    ~EnhancedAgentComms();
    
    // Negotiation
    NegotiationProtocol& getNegotiationProtocol();
    
    // Contracts
    ContractManager& getContractManager();
    
    // Gossip
    void enableGossipProtocol(const GossipProtocol::GossipConfig& config);
    GossipProtocol& getGossipProtocol();
    void spreadMessage(const std::string& content, const std::string& topic = "");
    
    // Security
    void enableEncryption();
    SecureMessaging& getSecureMessaging();
    void sendEncrypted(const Message& message);
    
    // Tracing
    void enableTracing();
    MessageTracer& getMessageTracer();
    std::string traceMessage(const Message& message);
    
    // Enhanced statistics
    struct EnhancedStats {
        uint64_t totalMessages;
        uint64_t negotiationsCompleted;
        uint64_t contractsExecuted;
        uint64_t gossipMessagesSpread;
        uint64_t encryptedMessages;
        double averageLatency;
    };
    EnhancedStats getEnhancedStats() const;
    
private:
    std::unique_ptr<NegotiationProtocol> negotiation_;
    std::unique_ptr<ContractManager> contracts_;
    std::unique_ptr<GossipProtocol> gossip_;
    std::unique_ptr<SecureMessaging> security_;
    std::unique_ptr<MessageTracer> tracer_;
    
    bool encryptionEnabled_ = false;
    bool tracingEnabled_ = false;
    mutable std::mutex enhancedMutex_;
};

} // namespace elizaos