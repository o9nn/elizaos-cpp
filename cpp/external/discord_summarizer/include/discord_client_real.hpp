// discord_client_real.hpp - Real Discord API client implementation using D++
// This header defines the DiscordClientReal class that uses the D++ library
// to provide real Discord bot functionality

#pragma once

#include "elizaos/discord_summarizer.hpp"

#ifdef DISCORD_REAL_IMPLEMENTATION

#include <dpp/dpp.h>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <future>
#include <queue>
#include <atomic>

namespace elizaos {

/**
 * Real Discord client implementation using D++ library
 * 
 * This class provides full Discord bot functionality including:
 * - WebSocket connection for real-time events
 * - REST API for message retrieval and sending
 * - Channel and guild management
 * - Event handling and callbacks
 * 
 * Usage:
 *   DiscordClientReal client;
 *   client.connect("YOUR_BOT_TOKEN");
 *   auto messages = client.getMessages("channel_id", 100);
 *   client.sendMessage("channel_id", "Hello from ElizaOS!");
 *   client.disconnect();
 */
class DiscordClientReal : public DiscordClient {
public:
    DiscordClientReal();
    ~DiscordClientReal() override;

    // Connection management
    bool connect(const std::string& token) override;
    bool disconnect() override;
    bool isConnected() const override;

    // Message operations
    std::vector<DiscordMessage> getMessages(const std::string& channelId, int limit = 100) override;
    std::vector<DiscordMessage> getMessagesSince(const std::string& channelId, 
                                                 const std::chrono::system_clock::time_point& since) override;
    bool sendMessage(const std::string& channelId, const std::string& content) override;
    bool deleteMessage(const std::string& channelId, const std::string& messageId) override;
    
    // Channel operations
    std::vector<DiscordChannel> getChannels(const std::string& guildId) override;
    DiscordChannel getChannel(const std::string& channelId) override;
    
    // Guild operations
    std::vector<DiscordGuild> getGuilds() override;
    DiscordGuild getGuild(const std::string& guildId) override;
    
    // Event handling
    void setMessageHandler(std::function<void(const DiscordMessage&)> handler) override;
    void setChannelHandler(std::function<void(const DiscordChannel&)> handler) override;

private:
    // D++ bot instance
    std::unique_ptr<dpp::cluster> bot_;
    
    // Connection state
    std::atomic<bool> connected_;
    std::atomic<bool> ready_;
    
    // Thread management
    std::unique_ptr<std::thread> botThread_;
    std::atomic<bool> shouldStop_;
    
    // Synchronization primitives
    mutable std::mutex clientMutex_;
    std::condition_variable readyCondition_;
    
    // Helper methods for type conversion
    DiscordMessage convertMessage(const dpp::message& msg);
    DiscordChannel convertChannel(const dpp::channel& chan);
    DiscordGuild convertGuild(const dpp::guild& guild);
    
    // Helper for synchronous operations
    template<typename T>
    T syncCall(std::function<void(std::function<void(const dpp::confirmation_callback_t&)>)> asyncFunc);
    
    // Bot event handlers
    void setupEventHandlers();
    void onReady(const dpp::ready_t& event);
    void onMessageCreate(const dpp::message_create_t& event);
    
    // Bot thread function
    void runBot();
};

} // namespace elizaos

#endif // DISCORD_REAL_IMPLEMENTATION
