// discord_client_real.cpp - Implementation of real Discord API client using D++

#include "discord_client_real.hpp"

#ifdef DISCORD_REAL_IMPLEMENTATION

#include <chrono>
#include <thread>
#include <iostream>

namespace elizaos {

DiscordClientReal::DiscordClientReal() 
    : connected_(false), ready_(false), shouldStop_(false) {
}

DiscordClientReal::~DiscordClientReal() {
    disconnect();
}

bool DiscordClientReal::connect(const std::string& token) {
    std::lock_guard<std::mutex> lock(clientMutex_);
    
    if (connected_) {
        return true; // Already connected
    }
    
    try {
        // Create D++ bot instance with token
        bot_ = std::make_unique<dpp::cluster>(token, dpp::i_default_intents | dpp::i_message_content);
        
        // Setup event handlers
        setupEventHandlers();
        
        // Start bot in separate thread
        shouldStop_ = false;
        botThread_ = std::make_unique<std::thread>(&DiscordClientReal::runBot, this);
        
        // Wait for ready event (with timeout)
        std::unique_lock<std::mutex> readyLock(clientMutex_);
        auto timeout = std::chrono::seconds(30);
        if (readyCondition_.wait_for(readyLock, timeout, [this]{ return ready_.load(); })) {
            connected_ = true;
            return true;
        } else {
            // Timeout - cleanup and return false
            shouldStop_ = true;
            if (botThread_ && botThread_->joinable()) {
                botThread_->join();
            }
            bot_.reset();
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to connect to Discord: " << e.what() << std::endl;
        bot_.reset();
        return false;
    }
}

bool DiscordClientReal::disconnect() {
    std::lock_guard<std::mutex> lock(clientMutex_);
    
    if (!connected_) {
        return true; // Already disconnected
    }
    
    shouldStop_ = true;
    
    if (bot_) {
        bot_->shutdown();
    }
    
    if (botThread_ && botThread_->joinable()) {
        botThread_->join();
    }
    
    bot_.reset();
    connected_ = false;
    ready_ = false;
    
    return true;
}

bool DiscordClientReal::isConnected() const {
    return connected_.load() && ready_.load();
}

std::vector<DiscordMessage> DiscordClientReal::getMessages(const std::string& channelId, int limit) {
    std::vector<DiscordMessage> messages;
    
    if (!isConnected() || !bot_) {
        return messages;
    }
    
    try {
        // Convert channel ID string to snowflake
        dpp::snowflake channel_sf = std::stoull(channelId);
        
        // Use syncCall to perform synchronous message retrieval
        auto result = syncCall<dpp::message_map>([this, channel_sf, limit](auto callback) {
            bot_->messages_get(channel_sf, 0, 0, 0, limit, callback);
        });
        
        // Convert D++ messages to our format
        for (const auto& [id, msg] : result) {
            messages.push_back(convertMessage(msg));
        }
        
        // Sort by timestamp (oldest first)
        std::sort(messages.begin(), messages.end(), 
                 [](const DiscordMessage& a, const DiscordMessage& b) {
                     return a.timestamp < b.timestamp;
                 });
        
    } catch (const std::exception& e) {
        std::cerr << "Error getting messages: " << e.what() << std::endl;
    }
    
    return messages;
}

std::vector<DiscordMessage> DiscordClientReal::getMessagesSince(
    const std::string& channelId, 
    const std::chrono::system_clock::time_point& since) {
    
    std::vector<DiscordMessage> allMessages = getMessages(channelId, 100);
    std::vector<DiscordMessage> filteredMessages;
    
    for (const auto& msg : allMessages) {
        if (msg.timestamp >= since) {
            filteredMessages.push_back(msg);
        }
    }
    
    return filteredMessages;
}

bool DiscordClientReal::sendMessage(const std::string& channelId, const std::string& content) {
    if (!isConnected() || !bot_) {
        return false;
    }
    
    try {
        dpp::snowflake channel_sf = std::stoull(channelId);
        
        // Create message
        dpp::message msg(channel_sf, content);
        
        // Send message asynchronously
        bot_->message_create(msg);
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error sending message: " << e.what() << std::endl;
        return false;
    }
}

bool DiscordClientReal::deleteMessage(const std::string& channelId, const std::string& messageId) {
    if (!isConnected() || !bot_) {
        return false;
    }
    
    try {
        dpp::snowflake message_sf = std::stoull(messageId);
        
        // Delete message asynchronously
        bot_->message_delete(message_sf, [](const dpp::confirmation_callback_t& callback) {
            if (callback.is_error()) {
                std::cerr << "Error deleting message: " << callback.get_error().message << std::endl;
            }
        });
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error deleting message: " << e.what() << std::endl;
        return false;
    }
}

std::vector<DiscordChannel> DiscordClientReal::getChannels(const std::string& guildId) {
    std::vector<DiscordChannel> channels;
    
    if (!isConnected() || !bot_) {
        return channels;
    }
    
    try {
        dpp::snowflake guild_sf = std::stoull(guildId);
        
        auto result = syncCall<dpp::channel_map>([this, guild_sf](auto callback) {
            bot_->channels_get(guild_sf, callback);
        });
        
        for (const auto& [id, chan] : result) {
            channels.push_back(convertChannel(chan));
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error getting channels: " << e.what() << std::endl;
    }
    
    return channels;
}

DiscordChannel DiscordClientReal::getChannel(const std::string& channelId) {
    if (!isConnected() || !bot_) {
        return DiscordChannel();
    }
    
    try {
        dpp::snowflake channel_sf = std::stoull(channelId);
        
        auto result = syncCall<dpp::channel>([this, channel_sf](auto callback) {
            bot_->channel_get(channel_sf, callback);
        });
        
        return convertChannel(result);
        
    } catch (const std::exception& e) {
        std::cerr << "Error getting channel: " << e.what() << std::endl;
        return DiscordChannel();
    }
}

std::vector<DiscordGuild> DiscordClientReal::getGuilds() {
    std::vector<DiscordGuild> guilds;
    
    if (!isConnected() || !bot_) {
        return guilds;
    }
    
    try {
        auto result = syncCall<dpp::guild_map>([this](auto callback) {
            bot_->current_user_get_guilds(callback);
        });
        
        for (const auto& [id, guild] : result) {
            guilds.push_back(convertGuild(guild));
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error getting guilds: " << e.what() << std::endl;
    }
    
    return guilds;
}

DiscordGuild DiscordClientReal::getGuild(const std::string& guildId) {
    if (!isConnected() || !bot_) {
        return DiscordGuild();
    }
    
    try {
        dpp::snowflake guild_sf = std::stoull(guildId);
        
        auto result = syncCall<dpp::guild>([this, guild_sf](auto callback) {
            bot_->guild_get(guild_sf, callback);
        });
        
        return convertGuild(result);
        
    } catch (const std::exception& e) {
        std::cerr << "Error getting guild: " << e.what() << std::endl;
        return DiscordGuild();
    }
}

void DiscordClientReal::setMessageHandler(std::function<void(const DiscordMessage&)> handler) {
    // Store handler for use in event processing
    // This would be called from onMessageCreate
    (void)handler; // Placeholder - implement event handler storage
}

void DiscordClientReal::setChannelHandler(std::function<void(const DiscordChannel&)> handler) {
    // Store handler for use in event processing
    (void)handler; // Placeholder - implement event handler storage
}

// Private helper methods

DiscordMessage DiscordClientReal::convertMessage(const dpp::message& msg) {
    DiscordMessage result;
    result.id = std::to_string(msg.id);
    result.channelId = std::to_string(msg.channel_id);
    result.authorId = std::to_string(msg.author.id);
    result.authorName = msg.author.username;
    result.content = msg.content;
    result.timestamp = std::chrono::system_clock::from_time_t(msg.sent);
    return result;
}

DiscordChannel DiscordClientReal::convertChannel(const dpp::channel& chan) {
    DiscordChannel result;
    result.id = std::to_string(chan.id);
    result.name = chan.name;
    result.type = static_cast<int>(chan.get_type());
    result.guildId = std::to_string(chan.guild_id);
    return result;
}

DiscordGuild DiscordClientReal::convertGuild(const dpp::guild& guild) {
    DiscordGuild result;
    result.id = std::to_string(guild.id);
    result.name = guild.name;
    return result;
}

template<typename T>
T DiscordClientReal::syncCall(std::function<void(std::function<void(const dpp::confirmation_callback_t&)>)> asyncFunc) {
    std::promise<T> promise;
    auto future = promise.get_future();
    
    asyncFunc([&promise](const dpp::confirmation_callback_t& callback) {
        if (callback.is_error()) {
            promise.set_exception(std::make_exception_ptr(
                std::runtime_error(callback.get_error().message)
            ));
        } else {
            promise.set_value(std::get<T>(callback.value));
        }
    });
    
    return future.get();
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
}

void DiscordClientReal::onReady(const dpp::ready_t& event) {
    (void)event;
    std::cout << "Discord bot ready!" << std::endl;
    
    ready_ = true;
    readyCondition_.notify_all();
}

void DiscordClientReal::onMessageCreate(const dpp::message_create_t& event) {
    // Handle incoming messages
    // This would call registered message handlers
    (void)event;
}

void DiscordClientReal::runBot() {
    if (bot_) {
        try {
            bot_->start(dpp::st_wait);
        } catch (const std::exception& e) {
            std::cerr << "Bot error: " << e.what() << std::endl;
        }
    }
}

} // namespace elizaos

#endif // DISCORD_REAL_IMPLEMENTATION
