#include "elizaos/discord_summarizer.hpp"
#include "elizaos/agentlogger.hpp"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <regex>
#include <cmath>
#include <numeric>
#include <filesystem>
#include <set>
#include <unordered_set>
#include <atomic>
#include <array>
#include <limits>
#include <ctime>

#include <nlohmann/json.hpp>

namespace elizaos {
namespace {

namespace fs = std::filesystem;
using json = nlohmann::json;

std::int64_t discordTimeToMilliseconds(std::chrono::system_clock::time_point value) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(value.time_since_epoch()).count();
}

std::chrono::system_clock::time_point discordTimeFromMilliseconds(std::int64_t value) {
    return std::chrono::system_clock::time_point(std::chrono::milliseconds(value));
}

}  // namespace

// Global summarizer instance
std::shared_ptr<DiscordSummarizer> globalDiscordSummarizer = std::make_shared<DiscordSummarizer>();

// Thread-safe in-process Discord transport. It performs no external network I/O;
// callers can use it deterministically for local ingestion, analysis, and tests.
class InMemoryDiscordClient : public DiscordClient {
public:
    bool connect(const std::string& token) override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        if (token.empty()) {
            connected_ = false;
            token_.clear();
            return false;
        }
        token_ = token;
        connected_ = true;
        return true;
    }

    bool disconnect() override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        const bool wasConnected = connected_;
        connected_ = false;
        token_.clear();
        return wasConnected;
    }

    bool isConnected() const override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        return connected_;
    }

    std::vector<DiscordMessage> getMessages(const std::string& channelId, int limit) override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        if (!connected_ || channelId.empty() || limit <= 0) return {};
        auto it = messages_.find(channelId);
        if (it == messages_.end()) return {};
        auto messages = it->second;
        std::sort(messages.begin(), messages.end(), [](const auto& left, const auto& right) {
            return left.timestamp > right.timestamp;
        });
        if (messages.size() > static_cast<std::size_t>(limit)) messages.resize(limit);
        return messages;
    }

    std::vector<DiscordMessage> getMessagesSince(
        const std::string& channelId,
        const std::chrono::system_clock::time_point& since) override {
        auto messages = getMessages(channelId, std::numeric_limits<int>::max());
        messages.erase(std::remove_if(messages.begin(), messages.end(), [&](const auto& message) {
            return message.timestamp < since;
        }), messages.end());
        return messages;
    }

    bool sendMessage(const std::string& channelId, const std::string& content) override {
        if (channelId.empty() || content.empty()) return false;
        DiscordMessage message;
        std::function<void(const DiscordMessage&)> handler;
        std::function<void(const DiscordChannel&)> channelHandler;
        DiscordChannel createdChannel;
        bool channelCreated = false;
        {
            std::lock_guard<std::mutex> lock(clientMutex_);
            if (!connected_) return false;
            message.id = "local_message_" + std::to_string(messageCounter_.fetch_add(1));
            message.channelId = channelId;
            message.authorId = "local-agent";
            message.authorName = "ElizaOS";
            message.content = content;
            message.timestamp = std::chrono::system_clock::now();
            messages_[channelId].push_back(message);
            if (channels_.count(channelId) == 0) {
                createdChannel.id = channelId;
                createdChannel.name = channelId;
                createdChannel.type = "text";
                channels_[channelId] = createdChannel;
                channelCreated = true;
            }
            handler = messageHandler_;
            channelHandler = channelHandler_;
        }
        if (channelCreated && channelHandler) channelHandler(createdChannel);
        if (handler) handler(message);
        return true;
    }

    bool deleteMessage(const std::string& channelId, const std::string& messageId) override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        if (!connected_ || channelId.empty() || messageId.empty()) return false;
        auto channelIt = messages_.find(channelId);
        if (channelIt == messages_.end()) return false;
        auto& messages = channelIt->second;
        auto it = std::find_if(messages.begin(), messages.end(), [&](const auto& message) {
            return message.id == messageId;
        });
        if (it == messages.end()) return false;
        messages.erase(it);
        return true;
    }

    std::vector<DiscordChannel> getChannels(const std::string& guildId) override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        if (!connected_) return {};
        std::vector<DiscordChannel> channels;
        for (const auto& [_, channel] : channels_) {
            if (guildId.empty() || channel.guildId == guildId) channels.push_back(channel);
        }
        std::sort(channels.begin(), channels.end(), [](const auto& left, const auto& right) {
            return left.id < right.id;
        });
        return channels;
    }

    DiscordChannel getChannel(const std::string& channelId) override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        if (!connected_) return {};
        auto it = channels_.find(channelId);
        return it == channels_.end() ? DiscordChannel{} : it->second;
    }

    std::vector<DiscordGuild> getGuilds() override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        if (!connected_) return {};
        std::vector<DiscordGuild> guilds;
        for (const auto& [_, guild] : guilds_) guilds.push_back(guild);
        std::sort(guilds.begin(), guilds.end(), [](const auto& left, const auto& right) {
            return left.id < right.id;
        });
        return guilds;
    }

    DiscordGuild getGuild(const std::string& guildId) override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        if (!connected_) return {};
        auto it = guilds_.find(guildId);
        return it == guilds_.end() ? DiscordGuild{} : it->second;
    }

private:
    std::unordered_map<std::string, std::vector<DiscordMessage>> messages_;
    std::unordered_map<std::string, DiscordChannel> channels_;
    std::unordered_map<std::string, DiscordGuild> guilds_;
    std::atomic<std::uint64_t> messageCounter_{0};
};

// DiscordClient base implementation: truthful connection and handler state, with
// network operations unsupported unless a derived transport implements them.
DiscordClient::DiscordClient() : connected_(false) {}
DiscordClient::~DiscordClient() = default;

bool DiscordClient::connect(const std::string& token) {
    std::lock_guard<std::mutex> lock(clientMutex_);
    connected_ = !token.empty();
    token_ = connected_ ? token : std::string();
    return connected_;
}
bool DiscordClient::disconnect() {
    std::lock_guard<std::mutex> lock(clientMutex_);
    const bool wasConnected = connected_;
    connected_ = false;
    token_.clear();
    return wasConnected;
}
bool DiscordClient::isConnected() const {
    std::lock_guard<std::mutex> lock(clientMutex_);
    return connected_;
}
std::vector<DiscordMessage> DiscordClient::getMessages(const std::string&, int) { return {}; }
std::vector<DiscordMessage> DiscordClient::getMessagesSince(
    const std::string&, const std::chrono::system_clock::time_point&) { return {}; }
bool DiscordClient::sendMessage(const std::string&, const std::string&) { return false; }
bool DiscordClient::deleteMessage(const std::string&, const std::string&) { return false; }
std::vector<DiscordChannel> DiscordClient::getChannels(const std::string&) { return {}; }
DiscordChannel DiscordClient::getChannel(const std::string&) { return {}; }
std::vector<DiscordGuild> DiscordClient::getGuilds() { return {}; }
DiscordGuild DiscordClient::getGuild(const std::string&) { return {}; }
void DiscordClient::setMessageHandler(std::function<void(const DiscordMessage&)> handler) {
    std::lock_guard<std::mutex> lock(clientMutex_);
    messageHandler_ = std::move(handler);
}
void DiscordClient::setChannelHandler(std::function<void(const DiscordChannel&)> handler) {
    std::lock_guard<std::mutex> lock(clientMutex_);
    channelHandler_ = std::move(handler);
}

// MessageAnalyzer implementation
MessageAnalyzer::MessageAnalyzer() : toxicityThreshold_(5) {
    // Initialize topic categories with sample keywords
    topicCategories_["technology"] = {"AI", "machine learning", "programming", "software", "computer"};
    topicCategories_["gaming"] = {"game", "gaming", "player", "level", "score"};
    topicCategories_["general"] = {"hello", "hi", "how", "what", "when", "where"};
}

MessageAnalyzer::~MessageAnalyzer() {}

MessageAnalysis MessageAnalyzer::analyzeMessage(const DiscordMessage& message) {
    std::lock_guard<std::mutex> lock(analyzerMutex_);
    
    MessageAnalysis analysis;
    analysis.messageId = message.id;
    
    // Sentiment analysis (simple keyword-based)
    analysis.sentiment = calculateSentiment(message.content);
    
    // Topic extraction
    analysis.topics = extractTopics(message.content);
    
    // Keyword extraction
    analysis.keywords = extractKeywords(message.content);
    
    // Content classification
    analysis.categories = classifyContent(message.content);
    
    // Toxicity assessment
    analysis.toxicityLevel = assessToxicity(message.content);
    
    // Spam detection
    analysis.containsSpam = detectSpam(message);
    
    // Language detection (simple English detection)
    analysis.language = detectLanguage(message.content);
    
    logInfo("Analyzed message " + message.id + " - sentiment: " + std::to_string(analysis.sentiment), "discord_summarizer");
    
    return analysis;
}

std::vector<MessageAnalysis> MessageAnalyzer::analyzeMessages(const std::vector<DiscordMessage>& messages) {
    std::vector<MessageAnalysis> analyses;
    
    for (const auto& message : messages) {
        analyses.push_back(analyzeMessage(message));
    }
    
    logInfo("Analyzed " + std::to_string(analyses.size()) + " messages", "discord_summarizer");
    
    return analyses;
}

double MessageAnalyzer::calculateSentiment(const std::string& content) {
    // Simple sentiment analysis based on positive/negative keywords
    std::vector<std::string> positiveWords = {"good", "great", "awesome", "excellent", "love", "like", "happy", "amazing"};
    std::vector<std::string> negativeWords = {"bad", "terrible", "awful", "hate", "dislike", "sad", "angry", "horrible"};
    
    std::string lowerContent = content;
    std::transform(lowerContent.begin(), lowerContent.end(), lowerContent.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    std::transform(lowerContent.begin(), lowerContent.end(), lowerContent.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    
    int positiveCount = 0;
    int negativeCount = 0;
    
    for (const auto& word : positiveWords) {
        if (lowerContent.find(word) != std::string::npos) {
            positiveCount++;
        }
    }
    
    for (const auto& word : negativeWords) {
        if (lowerContent.find(word) != std::string::npos) {
            negativeCount++;
        }
    }
    
    if (positiveCount + negativeCount == 0) {
        return 0.0; // Neutral
    }
    
    return (static_cast<double>(positiveCount - negativeCount) / (positiveCount + negativeCount));
}

std::string MessageAnalyzer::classifySentiment(double sentimentScore) {
    if (sentimentScore > 0.2) return "positive";
    if (sentimentScore < -0.2) return "negative";
    return "neutral";
}

std::vector<std::string> MessageAnalyzer::extractTopics(const std::string& content) {
    std::vector<std::string> topics;
    
    for (const auto& [category, keywords] : topicCategories_) {
        if (scoreKeywordMatch(content, keywords) > 0.0) {
            topics.push_back(category);
        }
    }
    
    return topics;
}

std::vector<std::string> MessageAnalyzer::extractKeywords(const std::string& content) {
    // Simple keyword extraction: words longer than 4 characters
    std::vector<std::string> keywords;
    std::istringstream iss(content);
    std::string word;
    
    while (iss >> word) {
        // Remove punctuation
        word.erase(std::remove_if(word.begin(), word.end(), ::ispunct), word.end());
        
        if (word.length() > 4) {
            std::transform(word.begin(), word.end(), word.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
            keywords.push_back(word);
        }
    }
    
    // Remove duplicates
    std::sort(keywords.begin(), keywords.end());
    keywords.erase(std::unique(keywords.begin(), keywords.end()), keywords.end());
    
    return keywords;
}

std::unordered_map<std::string, double> MessageAnalyzer::classifyContent(const std::string& content) {
    std::unordered_map<std::string, double> categories;
    
    for (const auto& [category, keywords] : topicCategories_) {
        double score = scoreKeywordMatch(content, keywords);
        if (score > 0.0) {
            categories[category] = score;
        }
    }
    
    return categories;
}

int MessageAnalyzer::assessToxicity(const std::string& content) {
    // Simple toxicity assessment based on profanity detection
    if (containsProfanity(content)) {
        return 7; // High toxicity
    }
    
    // Check for aggressive patterns
    size_t capsCount = std::count_if(content.begin(), content.end(), ::isupper);
    double capsRatio = static_cast<double>(capsCount) / content.length();
    
    if (capsRatio > 0.7 && content.length() > 10) {
        return 4; // Moderate toxicity (shouting)
    }
    
    return 1; // Low toxicity
}

bool MessageAnalyzer::detectSpam(const DiscordMessage& message) {
    // Simple spam detection heuristics
    const std::string& content = message.content;
    
    // Check for excessive repetition
    size_t maxRepeat = 0;
    for (size_t i = 0; i < content.length(); ++i) {
        size_t count = 1;
        while (i + count < content.length() && content[i] == content[i + count]) {
            count++;
        }
        maxRepeat = std::max(maxRepeat, count);
    }
    
    if (maxRepeat > 10) {
        return true;
    }
    
    // Check for excessive length
    if (content.length() > 2000) {
        return true;
    }
    
    // Check for bot messages with certain patterns
    if (message.isBot && content.find("http") != std::string::npos) {
        return true;
    }
    
    return false;
}

std::string MessageAnalyzer::detectLanguage(const std::string& content) {
    // Very simple language detection - just check for ASCII characters
    bool hasNonAscii = std::any_of(content.begin(), content.end(), 
        [](unsigned char c) { return c > 127; });
    
    if (hasNonAscii) {
        return "unknown";
    }
    
    return "en"; // Assume English for ASCII text
}

void MessageAnalyzer::setToxicityThreshold(int threshold) {
    toxicityThreshold_ = threshold;
}

void MessageAnalyzer::setSentimentModel(const std::string& modelPath) {
    sentimentModelPath_ = modelPath;
    logInfo("Set sentiment model path: " + modelPath, "discord_summarizer");
}

void MessageAnalyzer::addTopicCategory(const std::string& category, const std::vector<std::string>& keywords) {
    std::lock_guard<std::mutex> lock(analyzerMutex_);
    topicCategories_[category] = keywords;
    logInfo("Added topic category: " + category, "discord_summarizer");
}

std::vector<std::string> MessageAnalyzer::tokenizeText(const std::string& text) {
    std::vector<std::string> tokens;
    std::istringstream iss(text);
    std::string token;
    
    while (iss >> token) {
        // Remove punctuation
        token.erase(std::remove_if(token.begin(), token.end(), ::ispunct), token.end());
        if (!token.empty()) {
            std::transform(token.begin(), token.end(), token.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            std::transform(token.begin(), token.end(), token.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

double MessageAnalyzer::scoreKeywordMatch(const std::string& text, const std::vector<std::string>& keywords) {
    auto tokens = tokenizeText(text);
    
    if (tokens.empty()) {
        return 0.0;
    }
    
    int matches = 0;
    for (const auto& keyword : keywords) {
        std::string lowerKeyword = keyword;
        std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
        
        if (std::find(tokens.begin(), tokens.end(), lowerKeyword) != tokens.end()) {
            matches++;
        }
    }
    
    return static_cast<double>(matches) / tokens.size();
}

bool MessageAnalyzer::containsProfanity(const std::string& content) {
    // Simple profanity detection - check against known words
    std::vector<std::string> profanityList = {"damn", "hell", "crap"}; // Mild examples only
    
    std::string lowerContent = content;
    std::transform(lowerContent.begin(), lowerContent.end(), lowerContent.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    std::transform(lowerContent.begin(), lowerContent.end(), lowerContent.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    
    for (const auto& word : profanityList) {
        if (lowerContent.find(word) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

namespace {

json discordMessageToJson(const DiscordMessage& message) {
    return {{"id", message.id}, {"channel_id", message.channelId}, {"guild_id", message.guildId},
            {"author_id", message.authorId}, {"author_name", message.authorName},
            {"content", message.content}, {"timestamp_ms", discordTimeToMilliseconds(message.timestamp)},
            {"attachments", message.attachments}, {"embeds", message.embeds},
            {"reactions", message.reactions}, {"is_bot", message.isBot}};
}

DiscordMessage discordMessageFromJson(const json& value) {
    DiscordMessage message;
    message.id = value.at("id").get<std::string>();
    message.channelId = value.at("channel_id").get<std::string>();
    message.guildId = value.at("guild_id").get<std::string>();
    message.authorId = value.at("author_id").get<std::string>();
    message.authorName = value.at("author_name").get<std::string>();
    message.content = value.at("content").get<std::string>();
    message.timestamp = discordTimeFromMilliseconds(value.at("timestamp_ms").get<std::int64_t>());
    message.attachments = value.at("attachments").get<std::vector<std::string>>();
    message.embeds = value.at("embeds").get<std::vector<std::string>>();
    message.reactions = value.at("reactions").get<std::vector<std::string>>();
    message.isBot = value.at("is_bot").get<bool>();
    if (message.id.empty() || message.channelId.empty()) {
        throw std::invalid_argument("persisted Discord message has empty identity");
    }
    return message;
}

json messageAnalysisToJson(const MessageAnalysis& analysis) {
    return {{"message_id", analysis.messageId}, {"sentiment", analysis.sentiment},
            {"topics", analysis.topics}, {"keywords", analysis.keywords},
            {"categories", analysis.categories}, {"toxicity_level", analysis.toxicityLevel},
            {"contains_spam", analysis.containsSpam}, {"language", analysis.language}};
}

MessageAnalysis messageAnalysisFromJson(const json& value) {
    MessageAnalysis analysis;
    analysis.messageId = value.at("message_id").get<std::string>();
    analysis.sentiment = value.at("sentiment").get<double>();
    analysis.topics = value.at("topics").get<std::vector<std::string>>();
    analysis.keywords = value.at("keywords").get<std::vector<std::string>>();
    analysis.categories = value.at("categories").get<std::unordered_map<std::string, double>>();
    analysis.toxicityLevel = value.at("toxicity_level").get<int>();
    analysis.containsSpam = value.at("contains_spam").get<bool>();
    analysis.language = value.at("language").get<std::string>();
    if (analysis.messageId.empty() || !std::isfinite(analysis.sentiment) ||
        analysis.sentiment < -1.0 || analysis.sentiment > 1.0 ||
        analysis.toxicityLevel < 0 || analysis.toxicityLevel > 10) {
        throw std::invalid_argument("invalid persisted Discord analysis");
    }
    return analysis;
}

json channelSummaryToJson(const ChannelSummary& summary) {
    return {{"channel_id", summary.channelId}, {"channel_name", summary.channelName},
            {"period_start_ms", discordTimeToMilliseconds(summary.periodStart)},
            {"period_end_ms", discordTimeToMilliseconds(summary.periodEnd)},
            {"total_messages", summary.totalMessages}, {"unique_users", summary.uniqueUsers},
            {"top_users", summary.topUsers}, {"main_topics", summary.mainTopics},
            {"average_sentiment", summary.averageSentiment},
            {"most_active_time", summary.mostActiveTime}};
}

ChannelSummary channelSummaryFromJson(const json& value) {
    ChannelSummary summary;
    summary.channelId = value.at("channel_id").get<std::string>();
    summary.channelName = value.at("channel_name").get<std::string>();
    summary.periodStart = discordTimeFromMilliseconds(value.at("period_start_ms").get<std::int64_t>());
    summary.periodEnd = discordTimeFromMilliseconds(value.at("period_end_ms").get<std::int64_t>());
    summary.totalMessages = value.at("total_messages").get<int>();
    summary.uniqueUsers = value.at("unique_users").get<int>();
    summary.topUsers = value.at("top_users").get<std::vector<std::string>>();
    summary.mainTopics = value.at("main_topics").get<std::vector<std::string>>();
    summary.averageSentiment = value.at("average_sentiment").get<double>();
    summary.mostActiveTime = value.at("most_active_time").get<std::string>();
    if (summary.channelId.empty() || summary.periodEnd < summary.periodStart ||
        summary.totalMessages < 0 || summary.uniqueUsers < 0 ||
        !std::isfinite(summary.averageSentiment) || summary.averageSentiment < -1.0 ||
        summary.averageSentiment > 1.0) {
        throw std::invalid_argument("invalid persisted Discord summary");
    }
    return summary;
}

}  // namespace

// ChannelSummarizer implementation
ChannelSummarizer::ChannelSummarizer()
    : topUsersLimit_(5), topTopicsLimit_(5), minimumMessages_(1),
      dataManager_(nullptr), analyzer_(nullptr) {}
ChannelSummarizer::~ChannelSummarizer() = default;

void ChannelSummarizer::setDataSources(DiscordDataManager* dataManager, MessageAnalyzer* analyzer) {
    std::lock_guard<std::mutex> lock(summarizerMutex_);
    dataManager_ = dataManager;
    analyzer_ = analyzer;
}

ChannelSummary ChannelSummarizer::summarizeChannel(
    const std::string& channelId,
    const std::chrono::system_clock::time_point& startTime,
    const std::chrono::system_clock::time_point& endTime) {
    ChannelSummary summary;
    summary.channelId = channelId;
    summary.channelName = channelId;
    summary.periodStart = startTime;
    summary.periodEnd = endTime;
    if (channelId.empty() || endTime < startTime) return summary;

    DiscordDataManager* dataManager = nullptr;
    MessageAnalyzer* analyzer = nullptr;
    int topUsersLimit = 0;
    int topTopicsLimit = 0;
    int minimumMessages = 0;
    {
        std::lock_guard<std::mutex> lock(summarizerMutex_);
        dataManager = dataManager_;
        analyzer = analyzer_;
        topUsersLimit = topUsersLimit_;
        topTopicsLimit = topTopicsLimit_;
        minimumMessages = minimumMessages_;
    }
    if (!dataManager || !analyzer) return summary;

    auto messages = dataManager->retrieveMessages(channelId, std::numeric_limits<int>::max());
    messages.erase(std::remove_if(messages.begin(), messages.end(), [&](const auto& message) {
        return message.timestamp < startTime || message.timestamp > endTime;
    }), messages.end());
    summary.totalMessages = static_cast<int>(messages.size());
    std::unordered_set<std::string> authors;
    for (const auto& message : messages) {
        authors.insert(message.authorId.empty() ? message.authorName : message.authorId);
    }
    summary.uniqueUsers = static_cast<int>(authors.size());
    if (summary.totalMessages < minimumMessages) {
        dataManager->storeSummary(summary);
        return summary;
    }

    std::vector<MessageAnalysis> analyses;
    analyses.reserve(messages.size());
    for (const auto& message : messages) {
        auto analysis = dataManager->retrieveAnalysis(message.id);
        if (analysis.messageId.empty()) {
            analysis = analyzer->analyzeMessage(message);
            dataManager->storeAnalysis(analysis);
        }
        analyses.push_back(std::move(analysis));
    }
    summary.topUsers = findTopUsers(messages, topUsersLimit);
    summary.mainTopics = findMainTopics(analyses, topTopicsLimit);
    summary.averageSentiment = calculateAverageSentiment(analyses);
    summary.mostActiveTime = findMostActiveTime(messages);
    dataManager->storeSummary(summary);
    return summary;
}

ChannelSummary ChannelSummarizer::summarizeChannelDaily(const std::string& channelId) {
    const auto end = std::chrono::system_clock::now();
    return summarizeChannel(channelId, end - std::chrono::hours(24), end);
}

ChannelSummary ChannelSummarizer::summarizeChannelWeekly(const std::string& channelId) {
    const auto end = std::chrono::system_clock::now();
    return summarizeChannel(channelId, end - std::chrono::hours(24 * 7), end);
}

std::vector<ChannelSummary> ChannelSummarizer::summarizeAllChannels(
    const std::string& guildId,
    const std::chrono::system_clock::time_point& startTime,
    const std::chrono::system_clock::time_point& endTime) {
    (void)guildId;
    DiscordDataManager* dataManager = nullptr;
    {
        std::lock_guard<std::mutex> lock(summarizerMutex_);
        dataManager = dataManager_;
    }
    if (!dataManager) return {};
    std::vector<ChannelSummary> summaries;
    for (const auto& channelId : dataManager->getCachedChannelIds()) {
        summaries.push_back(summarizeChannel(channelId, startTime, endTime));
    }
    return summaries;
}

std::string ChannelSummarizer::generateTextReport(const ChannelSummary& summary) {
    std::ostringstream output;
    output << "Channel: " << summary.channelName << " (" << summary.channelId << ")\n"
           << "Messages: " << summary.totalMessages << "\n"
           << "Unique users: " << summary.uniqueUsers << "\n"
           << "Average sentiment: " << summary.averageSentiment << "\n"
           << "Most active hour: " << summary.mostActiveTime << "\n";
    return output.str();
}

std::string ChannelSummarizer::generateJsonReport(const ChannelSummary& summary) {
    return channelSummaryToJson(summary).dump(2);
}

std::string ChannelSummarizer::generateHtmlReport(const ChannelSummary& summary) {
    auto escape = [](std::string value) {
        const std::vector<std::pair<std::string, std::string>> replacements = {
            {"&", "&amp;"}, {"<", "&lt;"}, {">", "&gt;"}, {"\"", "&quot;"}
        };
        for (const auto& [from, to] : replacements) {
            std::size_t position = 0;
            while ((position = value.find(from, position)) != std::string::npos) {
                value.replace(position, from.size(), to);
                position += to.size();
            }
        }
        return value;
    };
    std::ostringstream output;
    output << "<!doctype html><html><body><h1>" << escape(summary.channelName)
           << "</h1><dl><dt>Messages</dt><dd>" << summary.totalMessages
           << "</dd><dt>Unique users</dt><dd>" << summary.uniqueUsers
           << "</dd><dt>Average sentiment</dt><dd>" << summary.averageSentiment
           << "</dd></dl></body></html>";
    return output.str();
}

void ChannelSummarizer::setTopUsersLimit(int limit) {
    std::lock_guard<std::mutex> lock(summarizerMutex_);
    topUsersLimit_ = std::max(0, limit);
}
void ChannelSummarizer::setTopTopicsLimit(int limit) {
    std::lock_guard<std::mutex> lock(summarizerMutex_);
    topTopicsLimit_ = std::max(0, limit);
}
void ChannelSummarizer::setMinimumMessages(int minimum) {
    std::lock_guard<std::mutex> lock(summarizerMutex_);
    minimumMessages_ = std::max(0, minimum);
}

std::vector<std::string> ChannelSummarizer::findTopUsers(
    const std::vector<DiscordMessage>& messages, int limit) {
    std::unordered_map<std::string, int> counts;
    for (const auto& message : messages) {
        const std::string author = message.authorName.empty() ? message.authorId : message.authorName;
        if (!author.empty()) ++counts[author];
    }
    std::vector<std::pair<std::string, int>> ranked(counts.begin(), counts.end());
    std::sort(ranked.begin(), ranked.end(), [](const auto& left, const auto& right) {
        return left.second != right.second ? left.second > right.second : left.first < right.first;
    });
    std::vector<std::string> result;
    for (int index = 0; index < limit && index < static_cast<int>(ranked.size()); ++index) {
        result.push_back(ranked[index].first);
    }
    return result;
}

std::vector<std::string> ChannelSummarizer::findMainTopics(
    const std::vector<MessageAnalysis>& analyses, int limit) {
    std::unordered_map<std::string, int> counts;
    for (const auto& analysis : analyses) {
        for (const auto& topic : analysis.topics) ++counts[topic];
    }
    std::vector<std::pair<std::string, int>> ranked(counts.begin(), counts.end());
    std::sort(ranked.begin(), ranked.end(), [](const auto& left, const auto& right) {
        return left.second != right.second ? left.second > right.second : left.first < right.first;
    });
    std::vector<std::string> result;
    for (int index = 0; index < limit && index < static_cast<int>(ranked.size()); ++index) {
        result.push_back(ranked[index].first);
    }
    return result;
}

double ChannelSummarizer::calculateAverageSentiment(const std::vector<MessageAnalysis>& analyses) {
    if (analyses.empty()) return 0.0;
    const double total = std::accumulate(analyses.begin(), analyses.end(), 0.0,
        [](double value, const auto& analysis) { return value + analysis.sentiment; });
    return total / static_cast<double>(analyses.size());
}

std::string ChannelSummarizer::findMostActiveTime(const std::vector<DiscordMessage>& messages) {
    if (messages.empty()) return {};
    std::array<int, 24> hours{};
    for (const auto& message : messages) {
        const std::time_t timestamp = std::chrono::system_clock::to_time_t(message.timestamp);
        std::tm utc{};
#ifdef _WIN32
        gmtime_s(&utc, &timestamp);
#else
        gmtime_r(&timestamp, &utc);
#endif
        ++hours[static_cast<std::size_t>(utc.tm_hour)];
    }
    const auto best = std::distance(hours.begin(), std::max_element(hours.begin(), hours.end()));
    std::ostringstream result;
    result << std::setfill('0') << std::setw(2) << best << ":00-"
           << std::setfill('0') << std::setw(2) << ((best + 1) % 24) << ":00 UTC";
    return result.str();
}

// DiscordDataManager implementation
DiscordDataManager::DiscordDataManager() : maxCacheSize_(1000), persistenceEnabled_(false) {}
DiscordDataManager::~DiscordDataManager() = default;

bool DiscordDataManager::storeMessage(const DiscordMessage& message) {
    if (message.id.empty() || message.channelId.empty()) return false;
    std::lock_guard<std::mutex> lock(dataMutex_);
    messageCache_[message.id] = message;
    enforceCacheLimitUnlocked();
    return !persistenceEnabled_ || persistUnlocked();
}

bool DiscordDataManager::storeMessages(const std::vector<DiscordMessage>& messages) {
    for (const auto& message : messages) {
        if (message.id.empty() || message.channelId.empty()) return false;
    }
    std::lock_guard<std::mutex> lock(dataMutex_);
    for (const auto& message : messages) messageCache_[message.id] = message;
    enforceCacheLimitUnlocked();
    return !persistenceEnabled_ || persistUnlocked();
}

std::vector<DiscordMessage> DiscordDataManager::retrieveMessages(
    const std::string& channelId, int limit) {
    if (channelId.empty() || limit <= 0) return {};
    std::lock_guard<std::mutex> lock(dataMutex_);
    std::vector<DiscordMessage> messages;
    for (const auto& [_, message] : messageCache_) {
        if (message.channelId == channelId) messages.push_back(message);
    }
    std::sort(messages.begin(), messages.end(), [](const auto& left, const auto& right) {
        return left.timestamp > right.timestamp;
    });
    if (messages.size() > static_cast<std::size_t>(limit)) messages.resize(limit);
    return messages;
}

bool DiscordDataManager::storeAnalysis(const MessageAnalysis& analysis) {
    if (analysis.messageId.empty()) return false;
    std::lock_guard<std::mutex> lock(dataMutex_);
    analysisCache_[analysis.messageId] = analysis;
    enforceCacheLimitUnlocked();
    return !persistenceEnabled_ || persistUnlocked();
}

bool DiscordDataManager::storeAnalyses(const std::vector<MessageAnalysis>& analyses) {
    for (const auto& analysis : analyses) if (analysis.messageId.empty()) return false;
    std::lock_guard<std::mutex> lock(dataMutex_);
    for (const auto& analysis : analyses) analysisCache_[analysis.messageId] = analysis;
    enforceCacheLimitUnlocked();
    return !persistenceEnabled_ || persistUnlocked();
}

MessageAnalysis DiscordDataManager::retrieveAnalysis(const std::string& messageId) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    auto it = analysisCache_.find(messageId);
    return it == analysisCache_.end() ? MessageAnalysis{} : it->second;
}

bool DiscordDataManager::storeSummary(const ChannelSummary& summary) {
    if (summary.channelId.empty() || summary.periodEnd < summary.periodStart) return false;
    std::lock_guard<std::mutex> lock(dataMutex_);
    auto& summaries = summaryCache_[summary.channelId];
    auto existing = std::find_if(summaries.begin(), summaries.end(), [&](const auto& value) {
        return value.periodStart == summary.periodStart && value.periodEnd == summary.periodEnd;
    });
    if (existing == summaries.end()) summaries.push_back(summary); else *existing = summary;
    if (summaries.size() > maxCacheSize_) summaries.erase(summaries.begin(),
        summaries.begin() + static_cast<std::ptrdiff_t>(summaries.size() - maxCacheSize_));
    return !persistenceEnabled_ || persistUnlocked();
}

std::vector<ChannelSummary> DiscordDataManager::retrieveSummaries(const std::string& channelId) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    auto it = summaryCache_.find(channelId);
    if (it == summaryCache_.end()) return {};
    auto summaries = it->second;
    std::sort(summaries.begin(), summaries.end(), [](const auto& left, const auto& right) {
        return left.periodEnd > right.periodEnd;
    });
    return summaries;
}

void DiscordDataManager::setCacheSize(size_t maxEntries) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    maxCacheSize_ = std::max<std::size_t>(1, maxEntries);
    enforceCacheLimitUnlocked();
    if (persistenceEnabled_) persistUnlocked();
}

void DiscordDataManager::clearCache() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    messageCache_.clear();
    analysisCache_.clear();
    summaryCache_.clear();
    if (persistenceEnabled_) persistUnlocked();
}

void DiscordDataManager::enablePersistence(const std::string& dataPath) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    if (dataPath.empty()) {
        persistenceEnabled_ = false;
        persistencePath_.clear();
        return;
    }
    fs::path path(dataPath);
    if (path.extension() != ".json") path /= "discord_cache.json";
    persistencePath_ = path.string();
    persistenceEnabled_ = true;
    std::error_code ec;
    if (fs::is_regular_file(path, ec)) loadPersistenceUnlocked();
    else persistUnlocked();
}

std::vector<std::string> DiscordDataManager::getCachedChannelIds() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    std::set<std::string> ids;
    for (const auto& [_, message] : messageCache_) ids.insert(message.channelId);
    for (const auto& [channelId, _] : summaryCache_) ids.insert(channelId);
    return {ids.begin(), ids.end()};
}

void DiscordDataManager::enforceCacheLimitUnlocked() {
    while (messageCache_.size() > maxCacheSize_) {
        auto oldest = std::min_element(messageCache_.begin(), messageCache_.end(),
            [](const auto& left, const auto& right) {
                return left.second.timestamp < right.second.timestamp;
            });
        if (oldest == messageCache_.end()) break;
        analysisCache_.erase(oldest->first);
        messageCache_.erase(oldest);
    }
    while (analysisCache_.size() > maxCacheSize_) analysisCache_.erase(analysisCache_.begin());
}

bool DiscordDataManager::persistUnlocked() {
    json messages = json::array();
    for (const auto& [_, message] : messageCache_) messages.push_back(discordMessageToJson(message));
    json analyses = json::array();
    for (const auto& [_, analysis] : analysisCache_) analyses.push_back(messageAnalysisToJson(analysis));
    json summaries = json::array();
    for (const auto& [_, values] : summaryCache_) {
        for (const auto& summary : values) summaries.push_back(channelSummaryToJson(summary));
    }
    const json document = {{"schema", "elizaos.discord_cache"}, {"version", 1},
                           {"messages", std::move(messages)},
                           {"analyses", std::move(analyses)},
                           {"summaries", std::move(summaries)}};
    return saveToFile(persistencePath_, document.dump(2));
}

bool DiscordDataManager::loadPersistenceUnlocked() {
    try {
        const std::string data = loadFromFile(persistencePath_);
        if (data.empty()) return false;
        const json document = json::parse(data);
        if (document.value("schema", "") != "elizaos.discord_cache" ||
            document.value("version", 0) != 1) return false;
        std::unordered_map<std::string, DiscordMessage> messages;
        std::unordered_map<std::string, MessageAnalysis> analyses;
        std::unordered_map<std::string, std::vector<ChannelSummary>> summaries;
        for (const auto& value : document.at("messages")) {
            auto message = discordMessageFromJson(value);
            if (!messages.emplace(message.id, message).second) return false;
        }
        for (const auto& value : document.at("analyses")) {
            auto analysis = messageAnalysisFromJson(value);
            if (!analyses.emplace(analysis.messageId, analysis).second) return false;
        }
        for (const auto& value : document.at("summaries")) {
            auto summary = channelSummaryFromJson(value);
            summaries[summary.channelId].push_back(std::move(summary));
        }
        messageCache_.swap(messages);
        analysisCache_.swap(analyses);
        summaryCache_.swap(summaries);
        enforceCacheLimitUnlocked();
        return true;
    } catch (...) {
        return false;
    }
}

bool DiscordDataManager::saveToFile(const std::string& filePath, const std::string& data) {
    if (filePath.empty()) return false;
    try {
        const fs::path destination(filePath);
        std::error_code ec;
        if (destination.has_parent_path()) {
            fs::create_directories(destination.parent_path(), ec);
            if (ec) return false;
        }
        fs::path temporary = destination;
        temporary += ".tmp";
        {
            std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
            if (!output) return false;
            output << data << '\n';
            if (!output.good()) return false;
        }
        fs::rename(temporary, destination, ec);
        if (ec) {
            fs::remove(destination, ec);
            ec.clear();
            fs::rename(temporary, destination, ec);
        }
        if (ec) fs::remove(temporary, ec);
        return !ec;
    } catch (...) {
        return false;
    }
}

std::string DiscordDataManager::loadFromFile(const std::string& filePath) {
    std::ifstream input(filePath, std::ios::binary);
    if (!input) return {};
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return (!input.good() && !input.eof()) ? std::string() : buffer.str();
}

// DiscordSummarizer implementation
DiscordSummarizer::DiscordSummarizer() : monitoring_(false) {
    client_ = std::make_unique<InMemoryDiscordClient>();
    summarizer_.setDataSources(&dataManager_, &analyzer_);
    config_ = {{"cache_size", "1000"}, {"top_users_limit", "5"},
               {"top_topics_limit", "5"}, {"minimum_messages", "1"},
               {"poll_interval_ms", "250"}, {"persistence_path", ""}};
}

DiscordSummarizer::~DiscordSummarizer() {
    stopMonitoring();
    if (client_) client_->disconnect();
}

bool DiscordSummarizer::initializeWithToken(const std::string& token) {
    if (!client_) return false;
    const bool connected = client_->connect(token);
    if (!connected) return false;
    client_->setMessageHandler([this](const DiscordMessage& message) {
        processNewMessage(message);
    });
    return true;
}

std::future<ChannelSummary> DiscordSummarizer::generateChannelSummary(
    const std::string& channelId,
    const std::chrono::system_clock::time_point& startTime,
    const std::chrono::system_clock::time_point& endTime) {
    return std::async(std::launch::async, [this, channelId, startTime, endTime]() {
        if (channelId.empty() || endTime < startTime) {
            ChannelSummary invalid;
            invalid.channelId = channelId;
            invalid.periodStart = startTime;
            invalid.periodEnd = endTime;
            return invalid;
        }
        if (client_ && client_->isConnected()) {
            auto messages = client_->getMessagesSince(channelId, startTime);
            messages.erase(std::remove_if(messages.begin(), messages.end(), [&](const auto& message) {
                return message.timestamp > endTime;
            }), messages.end());
            dataManager_.storeMessages(messages);
        }
        auto summary = summarizer_.summarizeChannel(channelId, startTime, endTime);
        if (client_) {
            const auto channel = client_->getChannel(channelId);
            if (!channel.name.empty()) summary.channelName = channel.name;
        }
        dataManager_.storeSummary(summary);
        return summary;
    });
}

std::future<std::vector<ChannelSummary>> DiscordSummarizer::generateGuildSummary(
    const std::string& guildId,
    const std::chrono::system_clock::time_point& startTime,
    const std::chrono::system_clock::time_point& endTime) {
    return std::async(std::launch::async, [this, guildId, startTime, endTime]() {
        std::vector<std::string> channelIds;
        if (client_ && client_->isConnected()) {
            for (const auto& channel : client_->getChannels(guildId)) {
                if (!channel.id.empty()) channelIds.push_back(channel.id);
            }
        }
        if (channelIds.empty()) channelIds = dataManager_.getCachedChannelIds();
        std::sort(channelIds.begin(), channelIds.end());
        channelIds.erase(std::unique(channelIds.begin(), channelIds.end()), channelIds.end());
        std::vector<ChannelSummary> summaries;
        summaries.reserve(channelIds.size());
        for (const auto& channelId : channelIds) {
            summaries.push_back(generateChannelSummary(channelId, startTime, endTime).get());
        }
        return summaries;
    });
}

void DiscordSummarizer::startMonitoring(const std::vector<std::string>& channelIds) {
    if (channelIds.empty() || !client_ || !client_->isConnected()) return;
    if (monitoring_.exchange(true)) return;
    {
        std::lock_guard<std::mutex> lock(configMutex_);
        monitoredChannels_ = channelIds;
    }
    if (monitoringThread_.joinable()) monitoringThread_.join();
    monitoringThread_ = std::thread(&DiscordSummarizer::monitoringLoop, this);
}

void DiscordSummarizer::stopMonitoring() {
    monitoring_ = false;
    if (monitoringThread_.joinable() &&
        monitoringThread_.get_id() != std::this_thread::get_id()) {
        monitoringThread_.join();
    }
}

bool DiscordSummarizer::isMonitoring() const {
    return monitoring_.load();
}

void DiscordSummarizer::loadConfiguration(const std::string& configPath) {
    try {
        std::ifstream input(configPath);
        if (!input) return;
        const json document = json::parse(input);
        if (document.value("schema", "") != "elizaos.discord_summarizer_config" ||
            document.value("version", 0) != 1 || !document.at("settings").is_object()) return;
        std::unordered_map<std::string, std::string> loaded;
        for (auto it = document.at("settings").begin(); it != document.at("settings").end(); ++it) {
            if (!it.value().is_string()) return;
            loaded[it.key()] = it.value().get<std::string>();
        }
        const int cacheSize = std::stoi(loaded.at("cache_size"));
        const int topUsers = std::stoi(loaded.at("top_users_limit"));
        const int topTopics = std::stoi(loaded.at("top_topics_limit"));
        const int minimumMessages = std::stoi(loaded.at("minimum_messages"));
        const int pollInterval = std::stoi(loaded.at("poll_interval_ms"));
        if (cacheSize <= 0 || topUsers < 0 || topTopics < 0 || minimumMessages < 0 ||
            pollInterval <= 0) return;
        {
            std::lock_guard<std::mutex> lock(configMutex_);
            config_ = loaded;
        }
        dataManager_.setCacheSize(static_cast<std::size_t>(cacheSize));
        summarizer_.setTopUsersLimit(topUsers);
        summarizer_.setTopTopicsLimit(topTopics);
        summarizer_.setMinimumMessages(minimumMessages);
        auto pathIt = loaded.find("persistence_path");
        if (pathIt != loaded.end() && !pathIt->second.empty()) {
            dataManager_.enablePersistence(pathIt->second);
        }
    } catch (...) {
        return;
    }
}

void DiscordSummarizer::saveConfiguration(const std::string& configPath) {
    if (configPath.empty()) return;
    try {
        std::unordered_map<std::string, std::string> snapshot;
        {
            std::lock_guard<std::mutex> lock(configMutex_);
            snapshot = config_;
        }
        const json document = {{"schema", "elizaos.discord_summarizer_config"},
                               {"version", 1}, {"settings", snapshot}};
        const fs::path destination(configPath);
        std::error_code ec;
        if (destination.has_parent_path()) fs::create_directories(destination.parent_path(), ec);
        if (ec) return;
        fs::path temporary = destination;
        temporary += ".tmp";
        {
            std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
            if (!output) return;
            output << document.dump(2) << '\n';
            if (!output.good()) return;
        }
        fs::rename(temporary, destination, ec);
        if (ec) {
            fs::remove(destination, ec);
            ec.clear();
            fs::rename(temporary, destination, ec);
        }
        if (ec) fs::remove(temporary, ec);
    } catch (...) {
        return;
    }
}

void DiscordSummarizer::monitoringLoop() {
    auto lastPoll = std::chrono::system_clock::now() - std::chrono::seconds(1);
    while (monitoring_) {
        std::vector<std::string> channels;
        int intervalMs = 250;
        {
            std::lock_guard<std::mutex> lock(configMutex_);
            channels = monitoredChannels_;
            auto it = config_.find("poll_interval_ms");
            if (it != config_.end()) {
                try { intervalMs = std::max(1, std::stoi(it->second)); } catch (...) {}
            }
        }
        const auto pollStarted = std::chrono::system_clock::now();
        for (const auto& channelId : channels) {
            for (const auto& message : client_->getMessagesSince(channelId, lastPoll)) {
                processNewMessage(message);
            }
        }
        lastPoll = pollStarted;
        std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
    }
}

void DiscordSummarizer::processNewMessage(const DiscordMessage& message) {
    if (!dataManager_.storeMessage(message)) return;
    const auto analysis = analyzer_.analyzeMessage(message);
    dataManager_.storeAnalysis(analysis);
}

} // namespace elizaos