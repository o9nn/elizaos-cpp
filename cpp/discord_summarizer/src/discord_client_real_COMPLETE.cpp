// discord_client_real.cpp - Real Discord API client implementation using D++
// This file implements the DiscordClientReal class using the D++ library
// for full Discord bot functionality

#include "discord_client_real.hpp"

#ifdef DISCORD_REAL_IMPLEMENTATION

#include "elizaos/agentlogger.hpp"
#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>

namespace elizaos {

// ==============================================================================
// HELPER FUNCTIONS
// ==============================================================================

namespace {
    // Helper to log messages
    void logInfo(const std::string& message, const std::string& component = "discord") {
        AgentLogger logger;
        logger.log(message, component, "DiscordClient", LogLevel::INFO);
    }
    
    void logSuccess(const std::string& message, const std::string& component = "discord") {
        AgentLogger logger;
        logger.log(message, component, "DiscordClient", LogLevel::SUCCESS);
    }
    
    void logError(const std::string& message, const std::string& component = "discord") {
        AgentLogger logger;
        logger.log(message, component, "DiscordClient", LogLevel::ERROR);
    }
    
    void logWarning(const std::string& message, const std::string& component = "discord") {
        AgentLogger logger;
        logger.log(message, component, "DiscordClient", LogLevel::WARNING);
    }
    
    // Convert timestamp to system_clock::time_point
    std::chrono::system_clock::time_point timestampToTimePoint(time_t timestamp) {
        return std::chrono::system_clock::from_time_t(timestamp);
    }
}

// ==============================================================================
// DISCORDCLIENTREAL IMPLEMENTATION
// ==============================================================================

DiscordClientReal::DiscordClientReal() 
    : DiscordClient()
    , connected_(false)
    , ready_(false)
    , shouldStop_(false) {
    
    logInfo("DiscordClientReal created (REAL implementation using D++)");
}

DiscordClientReal::~DiscordClientReal() {
    disconnect();
}

bool DiscordClientReal::connect(const std::string& token) {
    std::lock_guard<std::mutex> lock(clientMutex_);
    
    if (connected_.load()) {
        logWarning("Already connected to Discord");
        return true;
    }
    
    if (token.empty()) {
        logError("Cannot connect: empty bot token");
        return false;
    }
    
    try {
        // Create the bot with the token
        bot_ = std::make_unique<dpp::cluster>(token, dpp::i_default_intents | dpp::i_message_content);
        
        // Setup event handlers
        setupEventHandlers();
        
        // Start the bot in a separate thread
        shouldStop_.store(false);
        botThread_ = std::make_unique<std::thread>(&DiscordClientReal::runBot, this);
        
        // Wait for the bot to be ready (with timeout)
        std::unique_lock<std::mutex> readyLock(clientMutex_);
        auto waitResult = readyCondition_.wait_for(readyLock, std::chrono::seconds(30), 
            [this]() { return ready_.load() || shouldStop_.load(); });
        
        if (!waitResult || !ready_.load()) {
            logError("Failed to connect to Discord: timeout or connection error");
            disconnect();
            return false;
        }
        
        connected_.store(true);
        token_ = token;
        
        logSuccess("Successfully connected to Discord");
        return true;
        
    } catch (const std::exception& e) {
        logError("Failed to connect to Discord: " + std::string(e.what()));
        return false;
    }
}

bool DiscordClientReal::disconnect() {
    std::lock_guard<std::mutex> lock(clientMutex_);
    
    if (!connected_.load()) {
        return true;
    }
    
    // Signal the bot thread to stop
    shouldStop_.store(true);
    
    // Shutdown the bot
    if (bot_) {
        bot_->shutdown();
    }
    
    // Wait for the bot thread to finish
    if (botThread_ && botThread_->joinable()) {
        botThread_->join();
    }
    
    // Clean up
    bot_.reset();
    botThread_.reset();
    
    connected_.store(false);
    ready_.store(false);
    token_.clear();
    
    logInfo("Disconnected from Discord");
    return true;
}

bool DiscordClientReal::isConnected() const {
    return connected_.load() && ready_.load();
}

std::vector<DiscordMessage> DiscordClientReal::getMessages(const std::string& channelId, int limit) {
    if (!isConnected()) {
        logWarning("Cannot get messages: not connected to Discord");
        return {};
    }
    
    std::vector<DiscordMessage> messages;
    std::promise<void> promise;
    std::future<void> future = promise.get_future();
    
    try {
        dpp::snowflake channel_id_sf = std::stoull(channelId);
        
        bot_->messages_get(channel_id_sf, 0, 0, 0, limit, [&](const dpp::confirmation_callback_t& callback) {
            if (!callback.is_error()) {
                auto message_map = std::get<dpp::message_map>(callback.value);
                for (const auto& [id, msg] : message_map) {
                    messages.push_back(convertMessage(msg));
                }
                logInfo("Retrieved " + std::to_string(messages.size()) + " messages from channel " + channelId);
            } else {
                logError("Failed to get messages: " + callback.get_error().message);
            }
            promise.set_value();
        });
        
        // Wait for the async operation to complete (with timeout)
        if (future.wait_for(std::chrono::seconds(10)) == std::future_status::timeout) {
            logError("Timeout while getting messages from channel " + channelId);
        }
        
    } catch (const std::exception& e) {
        logError("Exception while getting messages: " + std::string(e.what()));
    }
    
    return messages;
}

std::vector<DiscordMessage> DiscordClientReal::getMessagesSince(
    const std::string& channelId, 
    const std::chrono::system_clock::time_point& since) {
    
    if (!isConnected()) {
        logWarning("Cannot get messages: not connected to Discord");
        return {};
    }
    
    // Get all recent messages and filter by timestamp
    auto allMessages = getMessages(channelId, 100);
    
    std::vector<DiscordMessage> filteredMessages;
    for (const auto& msg : allMessages) {
        if (msg.timestamp >= since) {
            filteredMessages.push_back(msg);
        }
    }
    
    return filteredMessages;
}

bool DiscordClientReal::sendMessage(const std::string& channelId, const std::string& content) {
    if (!isConnected()) {
        logWarning("Cannot send message: not connected to Discord");
        return false;
    }
    
    if (content.empty()) {
        logWarning("Cannot send empty message");
        return false;
    }
    
    std::promise<bool> promise;
    std::future<bool> future = promise.get_future();
    
    try {
        dpp::snowflake channel_id_sf = std::stoull(channelId);
        
        bot_->message_create(dpp::message(channel_id_sf, content), [&](const dpp::confirmation_callback_t& callback) {
            if (!callback.is_error()) {
                logSuccess("Message sent to channel " + channelId);
                promise.set_value(true);
            } else {
                logError("Failed to send message: " + callback.get_error().message);
                promise.set_value(false);
            }
        });
        
        // Wait for the async operation to complete (with timeout)
        if (future.wait_for(std::chrono::seconds(10)) == std::future_status::timeout) {
            logError("Timeout while sending message to channel " + channelId);
            return false;
        }
        
        return future.get();
        
    } catch (const std::exception& e) {
        logError("Exception while sending message: " + std::string(e.what()));
        return false;
    }
}

bool DiscordClientReal::deleteMessage(const std::string& channelId, const std::string& messageId) {
    if (!isConnected()) {
        logWarning("Cannot delete message: not connected to Discord");
        return false;
    }
    
    std::promise<bool> promise;
    std::future<bool> future = promise.get_future();
    
    try {
        dpp::snowflake channel_id_sf = std::stoull(channelId);
        dpp::snowflake message_id_sf = std::stoull(messageId);
        
        bot_->message_delete(message_id_sf, channel_id_sf, [&](const dpp::confirmation_callback_t& callback) {
            if (!callback.is_error()) {
                logSuccess("Message deleted from channel " + channelId);
                promise.set_value(true);
            } else {
                logError("Failed to delete message: " + callback.get_error().message);
                promise.set_value(false);
            }
        });
        
        // Wait for the async operation to complete (with timeout)
        if (future.wait_for(std::chrono::seconds(10)) == std::future_status::timeout) {
            logError("Timeout while deleting message from channel " + channelId);
            return false;
        }
        
        return future.get();
        
    } catch (const std::exception& e) {
        logError("Exception while deleting message: " + std::string(e.what()));
        return false;
    }
}

std::vector<DiscordChannel> DiscordClientReal::getChannels(const std::string& guildId) {
    if (!isConnected()) {
        logWarning("Cannot get channels: not connected to Discord");
        return {};
    }
    
    std::vector<DiscordChannel> channels;
    std::promise<void> promise;
    std::future<void> future = promise.get_future();
    
    try {
        dpp::snowflake guild_id_sf = std::stoull(guildId);
        
        bot_->channels_get(guild_id_sf, [&](const dpp::confirmation_callback_t& callback) {
            if (!callback.is_error()) {
                auto channel_map = std::get<dpp::channel_map>(callback.value);
                for (const auto& [id, chan] : channel_map) {
                    channels.push_back(convertChannel(chan));
                }
                logInfo("Retrieved " + std::to_string(channels.size()) + " channels from guild " + guildId);
            } else {
                logError("Failed to get channels: " + callback.get_error().message);
            }
            promise.set_value();
        });
        
        // Wait for the async operation to complete (with timeout)
        if (future.wait_for(std::chrono::seconds(10)) == std::future_status::timeout) {
            logError("Timeout while getting channels from guild " + guildId);
        }
        
    } catch (const std::exception& e) {
        logError("Exception while getting channels: " + std::string(e.what()));
    }
    
    return channels;
}

DiscordChannel DiscordClientReal::getChannel(const std::string& channelId) {
    if (!isConnected()) {
        logWarning("Cannot get channel: not connected to Discord");
        return DiscordChannel();
    }
    
    DiscordChannel channel;
    std::promise<void> promise;
    std::future<void> future = promise.get_future();
    
    try {
        dpp::snowflake channel_id_sf = std::stoull(channelId);
        
        bot_->channel_get(channel_id_sf, [&](const dpp::confirmation_callback_t& callback) {
            if (!callback.is_error()) {
                auto chan = std::get<dpp::channel>(callback.value);
                channel = convertChannel(chan);
                logInfo("Retrieved channel " + channelId);
            } else {
                logError("Failed to get channel: " + callback.get_error().message);
            }
            promise.set_value();
        });
        
        // Wait for the async operation to complete (with timeout)
        if (future.wait_for(std::chrono::seconds(10)) == std::future_status::timeout) {
            logError("Timeout while getting channel " + channelId);
        }
        
    } catch (const std::exception& e) {
        logError("Exception while getting channel: " + std::string(e.what()));
    }
    
    return channel;
}

std::vector<DiscordGuild> DiscordClientReal::getGuilds() {
    if (!isConnected()) {
        logWarning("Cannot get guilds: not connected to Discord");
        return {};
    }
    
    std::vector<DiscordGuild> guilds;
    
    // D++ doesn't have a direct API to get all guilds, so we use the cached guilds
    if (bot_) {
        auto guild_map = bot_->current_user_get_guilds_sync();
        for (const auto& [id, guild] : guild_map) {
            guilds.push_back(convertGuild(guild));
        }
        logInfo("Retrieved " + std::to_string(guilds.size()) + " guilds");
    }
    
    return guilds;
}

DiscordGuild DiscordClientReal::getGuild(const std::string& guildId) {
    if (!isConnected()) {
        logWarning("Cannot get guild: not connected to Discord");
        return DiscordGuild();
    }
    
    DiscordGuild guild;
    std::promise<void> promise;
    std::future<void> future = promise.get_future();
    
    try {
        dpp::snowflake guild_id_sf = std::stoull(guildId);
        
        bot_->guild_get(guild_id_sf, [&](const dpp::confirmation_callback_t& callback) {
            if (!callback.is_error()) {
                auto g = std::get<dpp::guild>(callback.value);
                guild = convertGuild(g);
                logInfo("Retrieved guild " + guildId);
            } else {
                logError("Failed to get guild: " + callback.get_error().message);
            }
            promise.set_value();
        });
        
        // Wait for the async operation to complete (with timeout)
        if (future.wait_for(std::chrono::seconds(10)) == std::future_status::timeout) {
            logError("Timeout while getting guild " + guildId);
        }
        
    } catch (const std::exception& e) {
        logError("Exception while getting guild: " + std::string(e.what()));
    }
    
    return guild;
}

void DiscordClientReal::setMessageHandler(std::function<void(const DiscordMessage&)> handler) {
    std::lock_guard<std::mutex> lock(clientMutex_);
    messageHandler_ = handler;
}

void DiscordClientReal::setChannelHandler(std::function<void(const DiscordChannel&)> handler) {
    std::lock_guard<std::mutex> lock(clientMutex_);
    channelHandler_ = handler;
}

// ==============================================================================
// PRIVATE HELPER METHODS
// ==============================================================================

DiscordMessage DiscordClientReal::convertMessage(const dpp::message& msg) {
    DiscordMessage result;
    result.id = std::to_string(msg.id);
    result.channelId = std::to_string(msg.channel_id);
    result.guildId = std::to_string(msg.guild_id);
    result.authorId = std::to_string(msg.author.id);
    result.authorName = msg.author.username;
    result.content = msg.content;
    result.isBot = msg.author.is_bot();
    result.timestamp = timestampToTimePoint(msg.sent);
    
    // Extract attachments
    for (const auto& attachment : msg.attachments) {
        result.attachments.push_back(attachment.url);
    }
    
    // Extract embeds (simplified)
    for (const auto& embed : msg.embeds) {
        if (!embed.title.empty()) {
            result.embeds.push_back(embed.title);
        }
    }
    
    // Extract reactions (simplified)
    for (const auto& reaction : msg.reactions) {
        result.reactions.push_back(reaction.emoji_name);
    }
    
    return result;
}

DiscordChannel DiscordClientReal::convertChannel(const dpp::channel& chan) {
    DiscordChannel result;
    result.id = std::to_string(chan.id);
    result.name = chan.name;
    result.guildId = std::to_string(chan.guild_id);
    result.topic = chan.topic;
    result.isNsfw = chan.is_nsfw();
    
    // Convert channel type
    switch (chan.get_type()) {
        case dpp::CHANNEL_TEXT:
            result.type = "text";
            break;
        case dpp::CHANNEL_VOICE:
            result.type = "voice";
            break;
        case dpp::CHANNEL_CATEGORY:
            result.type = "category";
            break;
        case dpp::CHANNEL_ANNOUNCEMENT:
            result.type = "announcement";
            break;
        default:
            result.type = "unknown";
            break;
    }
    
    return result;
}

DiscordGuild DiscordClientReal::convertGuild(const dpp::guild& guild) {
    DiscordGuild result;
    result.id = std::to_string(guild.id);
    result.name = guild.name;
    result.description = guild.description;
    
    // Note: channels and members require separate API calls
    // For performance, we don't populate these here
    
    return result;
}

void DiscordClientReal::setupEventHandlers() {
    if (!bot_) return;
    
    // Ready event
    bot_->on_ready([this](const dpp::ready_t& event) {
        onReady(event);
    });
    
    // Message create event
    bot_->on_message_create([this](const dpp::message_create_t& event) {
        onMessageCreate(event);
    });
    
    // Log event
    bot_->on_log([](const dpp::log_t& event) {
        if (event.severity >= dpp::ll_warning) {
            logWarning("D++ log: " + event.message);
        }
    });
}

void DiscordClientReal::onReady(const dpp::ready_t& /* event */) {
    ready_.store(true);
    readyCondition_.notify_all();
    logSuccess("Discord bot is ready");
}

void DiscordClientReal::onMessageCreate(const dpp::message_create_t& event) {
    if (messageHandler_) {
        DiscordMessage msg = convertMessage(event.msg);
        messageHandler_(msg);
    }
}

void DiscordClientReal::runBot() {
    try {
        if (bot_) {
            bot_->start(dpp::st_wait);
        }
    } catch (const std::exception& e) {
        logError("Bot thread exception: " + std::string(e.what()));
        shouldStop_.store(true);
    }
}

} // namespace elizaos

#else // !DISCORD_REAL_IMPLEMENTATION

// ==============================================================================
// MOCK IMPLEMENTATION (Fallback when real implementation is not enabled)
// ==============================================================================

#include "elizaos/discord_summarizer.hpp"
#include "elizaos/agentlogger.hpp"

namespace elizaos {

// Mock implementation - just log warnings
class DiscordClientReal : public DiscordClient {
public:
    DiscordClientReal() {
        AgentLogger logger;
        logger.log("Using MOCK Discord client (real implementation not enabled)", 
                  "discord", "DiscordClient", LogLevel::WARNING);
    }
    
    bool connect(const std::string& /* token */) override {
        AgentLogger logger;
        logger.log("Mock Discord connection", "discord", "DiscordClient", LogLevel::INFO);
        connected_ = true;
        return true;
    }
    
    bool disconnect() override {
        connected_ = false;
        return true;
    }
    
    bool isConnected() const override {
        return connected_;
    }
    
    std::vector<DiscordMessage> getMessages(const std::string& /* channelId */, int limit) override {
        std::vector<DiscordMessage> messages;
        for (int i = 0; i < std::min(limit, 5); ++i) {
            DiscordMessage msg;
            msg.id = "mock_msg_" + std::to_string(i);
            msg.content = "Mock message " + std::to_string(i);
            msg.authorName = "MockUser";
            messages.push_back(msg);
        }
        return messages;
    }
    
    std::vector<DiscordMessage> getMessagesSince(const std::string& channelId, 
                                                 const std::chrono::system_clock::time_point& /* since */) override {
        return getMessages(channelId, 10);
    }
    
    bool sendMessage(const std::string& /* channelId */, const std::string& /* content */) override {
        return true;
    }
    
    bool deleteMessage(const std::string& /* channelId */, const std::string& /* messageId */) override {
        return true;
    }
    
    std::vector<DiscordChannel> getChannels(const std::string& /* guildId */) override {
        return {};
    }
    
    DiscordChannel getChannel(const std::string& /* channelId */) override {
        return DiscordChannel();
    }
    
    std::vector<DiscordGuild> getGuilds() override {
        return {};
    }
    
    DiscordGuild getGuild(const std::string& /* guildId */) override {
        return DiscordGuild();
    }
    
    void setMessageHandler(std::function<void(const DiscordMessage&)> handler) override {
        messageHandler_ = handler;
    }
    
    void setChannelHandler(std::function<void(const DiscordChannel&)> handler) override {
        channelHandler_ = handler;
    }
};

} // namespace elizaos

#endif // DISCORD_REAL_IMPLEMENTATION
