// discord_summarizer.cpp - INTEGRATED VERSION with Real D++ Library
// This file shows the modifications needed to integrate the real Discord API

#include "elizaos/discord_summarizer.hpp"
#include "elizaos/agentlogger.hpp"

// NEW: Include real D++ implementation
#ifdef DISCORD_REAL_IMPLEMENTATION
#include "discord_client_real.hpp"
#endif

#include <algorithm>
#include <sstream>
#include <fstream>
#include <regex>
#include <cmath>
#include <numeric>

namespace elizaos {

// Global summarizer instance
std::shared_ptr<DiscordSummarizer> globalDiscordSummarizer = std::make_shared<DiscordSummarizer>();

// MODIFIED: Use real or mock client based on compilation flag
#ifndef DISCORD_REAL_IMPLEMENTATION
// Mock Discord client implementation for demonstration (FALLBACK)
class MockDiscordClient : public DiscordClient {
public:
    MockDiscordClient() : DiscordClient() {}
    
    bool connect(const std::string& token) override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        token_ = token;
        connected_ = !token.empty();
        
        if (connected_) {
            logInfo("Connected to Discord with token (MOCK)", "discord_summarizer");
        } else {
            logError("Failed to connect - invalid token", "discord_summarizer");
        }
        
        return connected_;
    }
    
    bool disconnect() override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        connected_ = false;
        token_.clear();
        logInfo("Disconnected from Discord (MOCK)", "discord_summarizer");
        return true;
    }
    
    bool isConnected() const override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        return connected_;
    }
    
    std::vector<DiscordMessage> getMessages(const std::string& channelId, int limit) override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        
        if (!connected_) {
            logWarning("Not connected to Discord", "discord_summarizer");
            return {};
        }
        
        // Mock implementation - generate sample messages
        std::vector<DiscordMessage> messages;
        for (int i = 0; i < std::min(limit, 10); ++i) {
            DiscordMessage msg;
            msg.id = "msg_" + std::to_string(i);
            msg.channelId = channelId;
            msg.authorName = "User" + std::to_string(i % 3 + 1);
            msg.content = "Sample message content " + std::to_string(i);
            msg.timestamp = std::chrono::system_clock::now() - std::chrono::hours(i);
            msg.isBot = (i % 5 == 0);
            messages.push_back(msg);
        }
        
        logInfo("Retrieved " + std::to_string(messages.size()) + " messages from channel " + channelId + " (MOCK)", "discord_summarizer");
        
        return messages;
    }
    
    std::vector<DiscordMessage> getMessagesSince(const std::string& channelId, 
                                                const std::chrono::system_clock::time_point& /* since */) override {
        // Simple mock: return all recent messages
        return getMessages(channelId, 50);
    }
    
    bool sendMessage(const std::string& channelId, const std::string& content) override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        
        if (!connected_) {
            return false;
        }
        
        logInfo("Sent message to channel " + channelId + ": " + content.substr(0, 50) + "... (MOCK)", "discord_summarizer");
        
        return true;
    }
    
    bool deleteMessage(const std::string& channelId, const std::string& messageId) override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        
        if (!connected_) {
            return false;
        }
        
        logInfo("Deleted message " + messageId + " from channel " + channelId + " (MOCK)", "discord_summarizer");
        
        return true;
    }
    
private:
    mutable std::mutex clientMutex_;
    std::string token_;
    bool connected_ = false;
};
#endif

// MODIFIED: DiscordSummarizer constructor - choose real or mock client
DiscordSummarizer::DiscordSummarizer() {
#ifdef DISCORD_REAL_IMPLEMENTATION
    // Use real D++ client
    client_ = std::make_unique<discord_impl::RealDiscordClient>();
    logInfo("DiscordSummarizer initialized with REAL D++ client", "discord_summarizer");
#else
    // Use mock client
    client_ = std::make_unique<MockDiscordClient>();
    logInfo("DiscordSummarizer initialized with MOCK client", "discord_summarizer");
#endif
}

DiscordSummarizer::~DiscordSummarizer() {
    if (client_ && client_->isConnected()) {
        client_->disconnect();
    }
}

// Connect to Discord
bool DiscordSummarizer::connect(const std::string& token) {
    if (!client_) {
        logError("Discord client not initialized", "discord_summarizer");
        return false;
    }
    
    bool success = client_->connect(token);
    
    if (success) {
#ifdef DISCORD_REAL_IMPLEMENTATION
        logInfo("Successfully connected to Discord using REAL API", "discord_summarizer");
#else
        logInfo("Successfully connected to Discord using MOCK API", "discord_summarizer");
#endif
    } else {
        logError("Failed to connect to Discord", "discord_summarizer");
    }
    
    return success;
}

// Disconnect from Discord
bool DiscordSummarizer::disconnect() {
    if (!client_) {
        return false;
    }
    
    return client_->disconnect();
}

// Check if connected
bool DiscordSummarizer::isConnected() const {
    if (!client_) {
        return false;
    }
    
    return client_->isConnected();
}

// Get messages from a channel
std::vector<DiscordMessage> DiscordSummarizer::getMessages(const std::string& channelId, int limit) {
    if (!client_) {
        logError("Discord client not initialized", "discord_summarizer");
        return {};
    }
    
    if (!client_->isConnected()) {
        logWarning("Not connected to Discord", "discord_summarizer");
        return {};
    }
    
    auto messages = client_->getMessages(channelId, limit);
    
#ifdef DISCORD_REAL_IMPLEMENTATION
    logInfo("Retrieved " + std::to_string(messages.size()) + " REAL messages from channel " + channelId, "discord_summarizer");
#else
    logInfo("Retrieved " + std::to_string(messages.size()) + " MOCK messages from channel " + channelId, "discord_summarizer");
#endif
    
    return messages;
}

// Get messages since a specific time
std::vector<DiscordMessage> DiscordSummarizer::getMessagesSince(const std::string& channelId, 
                                                               const std::chrono::system_clock::time_point& since) {
    if (!client_) {
        logError("Discord client not initialized", "discord_summarizer");
        return {};
    }
    
    if (!client_->isConnected()) {
        logWarning("Not connected to Discord", "discord_summarizer");
        return {};
    }
    
    return client_->getMessagesSince(channelId, since);
}

// Send a message to a channel
bool DiscordSummarizer::sendMessage(const std::string& channelId, const std::string& content) {
    if (!client_) {
        logError("Discord client not initialized", "discord_summarizer");
        return false;
    }
    
    if (!client_->isConnected()) {
        logWarning("Not connected to Discord", "discord_summarizer");
        return false;
    }
    
    bool success = client_->sendMessage(channelId, content);
    
#ifdef DISCORD_REAL_IMPLEMENTATION
    if (success) {
        logInfo("Sent REAL message to channel " + channelId, "discord_summarizer");
    }
#endif
    
    return success;
}

// Delete a message
bool DiscordSummarizer::deleteMessage(const std::string& channelId, const std::string& messageId) {
    if (!client_) {
        logError("Discord client not initialized", "discord_summarizer");
        return false;
    }
    
    if (!client_->isConnected()) {
        logWarning("Not connected to Discord", "discord_summarizer");
        return false;
    }
    
    return client_->deleteMessage(channelId, messageId);
}

// Summarize messages (unchanged - works with both real and mock messages)
std::string DiscordSummarizer::summarizeMessages(const std::vector<DiscordMessage>& messages, 
                                                const SummaryOptions& options) {
    if (messages.empty()) {
        return "No messages to summarize.";
    }
    
    std::ostringstream summary;
    
    // Filter messages based on options
    std::vector<DiscordMessage> filteredMessages;
    for (const auto& msg : messages) {
        if (options.excludeBots && msg.isBot) {
            continue;
        }
        
        if (options.minLength > 0 && msg.content.length() < static_cast<size_t>(options.minLength)) {
            continue;
        }
        
        filteredMessages.push_back(msg);
    }
    
    if (filteredMessages.empty()) {
        return "No messages match the filter criteria.";
    }
    
    // Sort by timestamp if requested
    if (options.sortByTime) {
        std::sort(filteredMessages.begin(), filteredMessages.end(),
                 [](const DiscordMessage& a, const DiscordMessage& b) {
                     return a.timestamp < b.timestamp;
                 });
    }
    
    // Generate summary based on format
    if (options.format == SummaryFormat::BULLET_POINTS) {
        summary << "Message Summary (" << filteredMessages.size() << " messages):\n\n";
        
        for (const auto& msg : filteredMessages) {
            summary << "• [" << msg.authorName << "] " << msg.content << "\n";
        }
    } else if (options.format == SummaryFormat::CONDENSED) {
        summary << "Condensed Summary:\n\n";
        
        // Group by author
        std::map<std::string, std::vector<std::string>> messagesByAuthor;
        for (const auto& msg : filteredMessages) {
            messagesByAuthor[msg.authorName].push_back(msg.content);
        }
        
        for (const auto& [author, contents] : messagesByAuthor) {
            summary << author << " (" << contents.size() << " messages): ";
            
            if (contents.size() <= 3) {
                for (size_t i = 0; i < contents.size(); ++i) {
                    summary << contents[i];
                    if (i < contents.size() - 1) summary << "; ";
                }
            } else {
                summary << contents[0] << "; ... (" << (contents.size() - 2) << " more); " << contents.back();
            }
            
            summary << "\n";
        }
    } else {  // FULL_TEXT
        summary << "Full Message Log:\n\n";
        
        for (const auto& msg : filteredMessages) {
            auto time_t = std::chrono::system_clock::to_time_t(msg.timestamp);
            summary << "[" << std::ctime(&time_t);
            summary.seekp(-1, std::ios_base::end);  // Remove newline from ctime
            summary << "] " << msg.authorName << ": " << msg.content << "\n";
        }
    }
    
    logInfo("Generated summary for " + std::to_string(filteredMessages.size()) + " messages", "discord_summarizer");
    
    return summary.str();
}

// ... rest of the implementation remains the same (statistics, export, etc.) ...

} // namespace elizaos
