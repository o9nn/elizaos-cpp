#include "elizaos/discord_summarizer.hpp"
#include "elizaos/agentlogger.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <map>
#include <numeric>
#include <sstream>
#include <unordered_set>

namespace elizaos {
namespace {

using Json = nlohmann::json;
using Clock = std::chrono::system_clock;

std::int64_t toMilliseconds(const Clock::time_point& value) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(value.time_since_epoch()).count();
}

Clock::time_point fromMilliseconds(std::int64_t value) {
    return Clock::time_point(std::chrono::milliseconds(value));
}

std::tm utcTime(std::time_t value) {
    std::tm result{};
#if defined(_WIN32)
    gmtime_s(&result, &value);
#else
    gmtime_r(&value, &result);
#endif
    return result;
}

std::string formatTime(const Clock::time_point& value) {
    const auto raw = Clock::to_time_t(value);
    const auto tm = utcTime(raw);
    std::ostringstream output;
    output << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return output.str();
}

std::string htmlEscape(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size());
    for (const char character : value) {
        switch (character) {
            case '&': escaped += "&amp;"; break;
            case '<': escaped += "&lt;"; break;
            case '>': escaped += "&gt;"; break;
            case '\"': escaped += "&quot;"; break;
            case '\'': escaped += "&#39;"; break;
            default: escaped += character; break;
        }
    }
    return escaped;
}

std::string lowercase(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char character) {
        return static_cast<char>(std::tolower(character));
    });
    return value;
}

std::vector<std::string> uniqueNonEmpty(const std::vector<std::string>& values) {
    std::vector<std::string> result;
    std::unordered_set<std::string> seen;
    for (const auto& value : values) {
        if (!value.empty() && seen.insert(value).second) {
            result.push_back(value);
        }
    }
    return result;
}

Json messageToJson(const DiscordMessage& message) {
    return Json{{"attachments", message.attachments},
                {"author_id", message.authorId},
                {"author_name", message.authorName},
                {"channel_id", message.channelId},
                {"content", message.content},
                {"embeds", message.embeds},
                {"guild_id", message.guildId},
                {"id", message.id},
                {"is_bot", message.isBot},
                {"reactions", message.reactions},
                {"timestamp_ms", toMilliseconds(message.timestamp)}};
}

DiscordMessage messageFromJson(const Json& value) {
    DiscordMessage message;
    message.id = value.at("id").get<std::string>();
    message.channelId = value.at("channel_id").get<std::string>();
    message.guildId = value.value("guild_id", std::string{});
    message.authorId = value.value("author_id", std::string{});
    message.authorName = value.value("author_name", std::string{});
    message.content = value.value("content", std::string{});
    message.timestamp = fromMilliseconds(value.at("timestamp_ms").get<std::int64_t>());
    message.attachments = value.value("attachments", std::vector<std::string>{});
    message.embeds = value.value("embeds", std::vector<std::string>{});
    message.reactions = value.value("reactions", std::vector<std::string>{});
    message.isBot = value.value("is_bot", false);
    return message;
}

Json analysisToJson(const MessageAnalysis& analysis) {
    std::map<std::string, double> categories(analysis.categories.begin(), analysis.categories.end());
    return Json{{"categories", categories},
                {"contains_spam", analysis.containsSpam},
                {"keywords", analysis.keywords},
                {"language", analysis.language},
                {"message_id", analysis.messageId},
                {"sentiment", analysis.sentiment},
                {"topics", analysis.topics},
                {"toxicity_level", analysis.toxicityLevel}};
}

MessageAnalysis analysisFromJson(const Json& value) {
    MessageAnalysis analysis;
    analysis.messageId = value.at("message_id").get<std::string>();
    analysis.sentiment = value.value("sentiment", 0.0);
    analysis.topics = value.value("topics", std::vector<std::string>{});
    analysis.keywords = value.value("keywords", std::vector<std::string>{});
    analysis.categories = value.value("categories", std::unordered_map<std::string, double>{});
    analysis.toxicityLevel = value.value("toxicity_level", 0);
    analysis.containsSpam = value.value("contains_spam", false);
    analysis.language = value.value("language", std::string{});
    return analysis;
}

Json summaryToJson(const ChannelSummary& summary) {
    return Json{{"average_sentiment", summary.averageSentiment},
                {"channel_id", summary.channelId},
                {"channel_name", summary.channelName},
                {"main_topics", summary.mainTopics},
                {"most_active_time", summary.mostActiveTime},
                {"period_end_ms", toMilliseconds(summary.periodEnd)},
                {"period_start_ms", toMilliseconds(summary.periodStart)},
                {"top_users", summary.topUsers},
                {"total_messages", summary.totalMessages},
                {"unique_users", summary.uniqueUsers}};
}

ChannelSummary summaryFromJson(const Json& value) {
    ChannelSummary summary;
    summary.channelId = value.at("channel_id").get<std::string>();
    summary.channelName = value.value("channel_name", std::string{});
    summary.periodStart = fromMilliseconds(value.at("period_start_ms").get<std::int64_t>());
    summary.periodEnd = fromMilliseconds(value.at("period_end_ms").get<std::int64_t>());
    summary.totalMessages = value.value("total_messages", 0);
    summary.uniqueUsers = value.value("unique_users", 0);
    summary.topUsers = value.value("top_users", std::vector<std::string>{});
    summary.mainTopics = value.value("main_topics", std::vector<std::string>{});
    summary.averageSentiment = value.value("average_sentiment", 0.0);
    summary.mostActiveTime = value.value("most_active_time", std::string{});
    return summary;
}

std::string join(const std::vector<std::string>& values, const std::string& separator) {
    std::ostringstream output;
    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index != 0) {
            output << separator;
        }
        output << values[index];
    }
    return output.str();
}

std::string encodeConfigValue(const std::string& value) {
    std::ostringstream output;
    output << std::hex << std::uppercase;
    for (const unsigned char character : value) {
        if (std::isalnum(character) || character == '-' || character == '_' || character == '.' || character == ',') {
            output << static_cast<char>(character);
        } else {
            output << '%' << std::setw(2) << std::setfill('0') << static_cast<int>(character);
        }
    }
    return output.str();
}

int hexDigit(char value) {
    if (value >= '0' && value <= '9') return value - '0';
    if (value >= 'a' && value <= 'f') return value - 'a' + 10;
    if (value >= 'A' && value <= 'F') return value - 'A' + 10;
    return -1;
}

bool decodeConfigValue(const std::string& value, std::string& decoded) {
    decoded.clear();
    for (std::size_t index = 0; index < value.size(); ++index) {
        if (value[index] != '%') {
            decoded.push_back(value[index]);
            continue;
        }
        if (index + 2 >= value.size()) return false;
        const int high = hexDigit(value[index + 1]);
        const int low = hexDigit(value[index + 2]);
        if (high < 0 || low < 0) return false;
        decoded.push_back(static_cast<char>((high << 4) | low));
        index += 2;
    }
    return true;
}

bool parseNonNegativeInt(const std::string& value, int& result) {
    try {
        std::size_t consumed = 0;
        const long long parsed = std::stoll(value, &consumed);
        if (consumed != value.size() || parsed < 0 || parsed > std::numeric_limits<int>::max()) {
            return false;
        }
        result = static_cast<int>(parsed);
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<std::string> splitCommaSeparated(const std::string& value) {
    std::vector<std::string> result;
    std::istringstream input(value);
    std::string item;
    while (std::getline(input, item, ',')) {
        if (!item.empty()) result.push_back(item);
    }
    return uniqueNonEmpty(result);
}

// Explicit in-process reference client. It performs no network I/O, but every
// successful operation mutates state visible through the normal client API.
class InProcessDiscordClient final : public DiscordClient {
public:
    InProcessDiscordClient() {
        DiscordGuild guild("local-guild", "Local Discord Guild");
        guild.description = "In-process Discord reference guild";
        guilds_.emplace(guild.id, guild);
    }

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
        if (!connected_) return false;
        connected_ = false;
        token_.clear();
        return true;
    }

    bool isConnected() const override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        return connected_;
    }

    std::vector<DiscordMessage> getMessages(const std::string& channelId, int limit) override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        if (!connected_ || channelId.empty() || limit <= 0) return {};
        const auto found = messages_.find(channelId);
        if (found == messages_.end()) return {};
        auto result = found->second;
        std::sort(result.begin(), result.end(), [](const auto& left, const auto& right) {
            if (left.timestamp != right.timestamp) return left.timestamp > right.timestamp;
            return left.id < right.id;
        });
        if (result.size() > static_cast<std::size_t>(limit)) {
            result.resize(static_cast<std::size_t>(limit));
        }
        return result;
    }

    std::vector<DiscordMessage> getMessagesSince(
        const std::string& channelId, const Clock::time_point& since) override {
        auto result = getMessages(channelId, std::numeric_limits<int>::max());
        result.erase(std::remove_if(result.begin(), result.end(), [&](const DiscordMessage& message) {
            return message.timestamp < since;
        }), result.end());
        std::sort(result.begin(), result.end(), [](const auto& left, const auto& right) {
            if (left.timestamp != right.timestamp) return left.timestamp < right.timestamp;
            return left.id < right.id;
        });
        return result;
    }

    bool sendMessage(const std::string& channelId, const std::string& content) override {
        std::function<void(const DiscordMessage&)> messageHandler;
        std::function<void(const DiscordChannel&)> channelHandler;
        DiscordMessage message;
        DiscordChannel createdChannel;
        bool channelCreated = false;
        {
            std::lock_guard<std::mutex> lock(clientMutex_);
            if (!connected_ || channelId.empty() || content.empty()) return false;

            auto channel = channels_.find(channelId);
            if (channel == channels_.end()) {
                createdChannel = DiscordChannel(channelId, channelId, "local-guild");
                createdChannel.type = "text";
                channels_.emplace(channelId, createdChannel);
                guilds_.at("local-guild").channels.push_back(channelId);
                channelCreated = true;
            }

            message.id = "local-message-" + std::to_string(++nextMessageId_);
            message.channelId = channelId;
            message.guildId = channels_.at(channelId).guildId;
            message.authorId = "local-client";
            message.authorName = "ElizaOS";
            message.content = content;
            message.timestamp = Clock::now();
            message.isBot = true;
            messages_[channelId].push_back(message);
            messageHandler = messageHandler_;
            channelHandler = channelHandler_;
        }
        if (channelCreated && channelHandler) channelHandler(createdChannel);
        if (messageHandler) messageHandler(message);
        return true;
    }

    bool deleteMessage(const std::string& channelId, const std::string& messageId) override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        if (!connected_ || channelId.empty() || messageId.empty()) return false;
        const auto found = messages_.find(channelId);
        if (found == messages_.end()) return false;
        auto& messages = found->second;
        const auto originalSize = messages.size();
        messages.erase(std::remove_if(messages.begin(), messages.end(), [&](const DiscordMessage& message) {
            return message.id == messageId;
        }), messages.end());
        return messages.size() != originalSize;
    }

    std::vector<DiscordChannel> getChannels(const std::string& guildId) override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        if (!connected_ || guildId.empty() || guilds_.find(guildId) == guilds_.end()) return {};
        std::vector<DiscordChannel> result;
        for (const auto& [id, channel] : channels_) {
            (void)id;
            if (channel.guildId == guildId) result.push_back(channel);
        }
        std::sort(result.begin(), result.end(), [](const auto& left, const auto& right) {
            return left.id < right.id;
        });
        return result;
    }

    DiscordChannel getChannel(const std::string& channelId) override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        if (!connected_) return {};
        const auto found = channels_.find(channelId);
        return found == channels_.end() ? DiscordChannel{} : found->second;
    }

    std::vector<DiscordGuild> getGuilds() override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        if (!connected_) return {};
        std::vector<DiscordGuild> result;
        for (const auto& [id, guild] : guilds_) {
            (void)id;
            result.push_back(guild);
        }
        std::sort(result.begin(), result.end(), [](const auto& left, const auto& right) {
            return left.id < right.id;
        });
        return result;
    }

    DiscordGuild getGuild(const std::string& guildId) override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        if (!connected_) return {};
        const auto found = guilds_.find(guildId);
        return found == guilds_.end() ? DiscordGuild{} : found->second;
    }

    void setMessageHandler(std::function<void(const DiscordMessage&)> handler) override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        messageHandler_ = std::move(handler);
    }

    void setChannelHandler(std::function<void(const DiscordChannel&)> handler) override {
        std::lock_guard<std::mutex> lock(clientMutex_);
        channelHandler_ = std::move(handler);
    }

private:
    std::unordered_map<std::string, std::vector<DiscordMessage>> messages_;
    std::unordered_map<std::string, DiscordChannel> channels_;
    std::unordered_map<std::string, DiscordGuild> guilds_;
    std::uint64_t nextMessageId_ = 0;
};

} // namespace

std::shared_ptr<DiscordSummarizer> globalDiscordSummarizer = std::make_shared<DiscordSummarizer>();

DiscordClient::DiscordClient() : connected_(false) {}
DiscordClient::~DiscordClient() = default;

bool DiscordClient::connect(const std::string&) { return false; }
bool DiscordClient::disconnect() { return false; }
bool DiscordClient::isConnected() const { return false; }
std::vector<DiscordMessage> DiscordClient::getMessages(const std::string&, int) { return {}; }
std::vector<DiscordMessage> DiscordClient::getMessagesSince(const std::string&, const Clock::time_point&) { return {}; }
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

MessageAnalyzer::MessageAnalyzer() : toxicityThreshold_(5) {
    topicCategories_["general"] = {"hello", "hi", "how", "what", "when", "where"};
    topicCategories_["gaming"] = {"game", "gaming", "player", "level", "score"};
    topicCategories_["technology"] = {"ai", "machine", "learning", "programming", "software", "computer"};
}

MessageAnalyzer::~MessageAnalyzer() = default;

MessageAnalysis MessageAnalyzer::analyzeMessage(const DiscordMessage& message) {
    std::lock_guard<std::recursive_mutex> lock(analyzerMutex_);
    MessageAnalysis analysis;
    analysis.messageId = message.id;
    analysis.sentiment = calculateSentiment(message.content);
    analysis.topics = extractTopics(message.content);
    analysis.keywords = extractKeywords(message.content);
    analysis.categories = classifyContent(message.content);
    analysis.toxicityLevel = assessToxicity(message.content);
    analysis.containsSpam = detectSpam(message);
    analysis.language = detectLanguage(message.content);
    return analysis;
}

std::vector<MessageAnalysis> MessageAnalyzer::analyzeMessages(const std::vector<DiscordMessage>& messages) {
    std::vector<MessageAnalysis> analyses;
    analyses.reserve(messages.size());
    for (const auto& message : messages) analyses.push_back(analyzeMessage(message));
    return analyses;
}

double MessageAnalyzer::calculateSentiment(const std::string& content) {
    std::lock_guard<std::recursive_mutex> lock(analyzerMutex_);
    static const std::array<std::string, 8> positiveWords = {
        "good", "great", "awesome", "excellent", "love", "like", "happy", "amazing"};
    static const std::array<std::string, 8> negativeWords = {
        "bad", "terrible", "awful", "hate", "dislike", "sad", "angry", "horrible"};
    const auto tokens = tokenizeText(content);
    int positiveCount = 0;
    int negativeCount = 0;
    for (const auto& token : tokens) {
        positiveCount += std::count(positiveWords.begin(), positiveWords.end(), token);
        negativeCount += std::count(negativeWords.begin(), negativeWords.end(), token);
    }
    const int total = positiveCount + negativeCount;
    return total == 0 ? 0.0 : static_cast<double>(positiveCount - negativeCount) / total;
}

std::string MessageAnalyzer::classifySentiment(double sentimentScore) {
    if (sentimentScore > 0.2) return "positive";
    if (sentimentScore < -0.2) return "negative";
    return "neutral";
}

std::vector<std::string> MessageAnalyzer::extractTopics(const std::string& content) {
    std::lock_guard<std::recursive_mutex> lock(analyzerMutex_);
    std::vector<std::string> topics;
    for (const auto& [category, keywords] : topicCategories_) {
        if (scoreKeywordMatch(content, keywords) > 0.0) topics.push_back(category);
    }
    std::sort(topics.begin(), topics.end());
    return topics;
}

std::vector<std::string> MessageAnalyzer::extractKeywords(const std::string& content) {
    std::lock_guard<std::recursive_mutex> lock(analyzerMutex_);
    auto words = tokenizeText(content);
    words.erase(std::remove_if(words.begin(), words.end(), [](const std::string& word) {
        return word.size() <= 4;
    }), words.end());
    std::sort(words.begin(), words.end());
    words.erase(std::unique(words.begin(), words.end()), words.end());
    return words;
}

std::unordered_map<std::string, double> MessageAnalyzer::classifyContent(const std::string& content) {
    std::lock_guard<std::recursive_mutex> lock(analyzerMutex_);
    std::unordered_map<std::string, double> categories;
    for (const auto& [category, keywords] : topicCategories_) {
        const double score = scoreKeywordMatch(content, keywords);
        if (score > 0.0) categories.emplace(category, score);
    }
    return categories;
}

int MessageAnalyzer::assessToxicity(const std::string& content) {
    std::lock_guard<std::recursive_mutex> lock(analyzerMutex_);
    if (containsProfanity(content)) return std::clamp(toxicityThreshold_ + 2, 0, 10);
    if (content.empty()) return 0;
    const auto capsCount = std::count_if(content.begin(), content.end(), [](unsigned char character) {
        return std::isupper(character) != 0;
    });
    const double capsRatio = static_cast<double>(capsCount) / static_cast<double>(content.size());
    return capsRatio > 0.7 && content.size() > 10 ? std::min(4, toxicityThreshold_) : 1;
}

bool MessageAnalyzer::detectSpam(const DiscordMessage& message) {
    const auto& content = message.content;
    std::size_t maxRepeat = 0;
    for (std::size_t index = 0; index < content.size();) {
        std::size_t next = index + 1;
        while (next < content.size() && content[next] == content[index]) ++next;
        maxRepeat = std::max(maxRepeat, next - index);
        index = next;
    }
    return maxRepeat > 10 || content.size() > 2000 ||
           (message.isBot && lowercase(content).find("http") != std::string::npos);
}

std::string MessageAnalyzer::detectLanguage(const std::string& content) {
    const bool hasNonAscii = std::any_of(content.begin(), content.end(), [](unsigned char character) {
        return character > 127;
    });
    return hasNonAscii ? "unknown" : "en";
}

void MessageAnalyzer::setToxicityThreshold(int threshold) {
    std::lock_guard<std::recursive_mutex> lock(analyzerMutex_);
    toxicityThreshold_ = std::clamp(threshold, 0, 10);
}

void MessageAnalyzer::setSentimentModel(const std::string& modelPath) {
    std::lock_guard<std::recursive_mutex> lock(analyzerMutex_);
    sentimentModelPath_ = modelPath;
}

void MessageAnalyzer::addTopicCategory(const std::string& category, const std::vector<std::string>& keywords) {
    if (category.empty()) return;
    std::lock_guard<std::recursive_mutex> lock(analyzerMutex_);
    topicCategories_[category] = uniqueNonEmpty(keywords);
}

std::vector<std::string> MessageAnalyzer::tokenizeText(const std::string& text) {
    std::vector<std::string> tokens;
    std::string token;
    for (const unsigned char character : text) {
        if (std::isalnum(character) || character >= 128) {
            token.push_back(static_cast<char>(std::tolower(character)));
        } else if (!token.empty()) {
            tokens.push_back(token);
            token.clear();
        }
    }
    if (!token.empty()) tokens.push_back(token);
    return tokens;
}

double MessageAnalyzer::scoreKeywordMatch(const std::string& text, const std::vector<std::string>& keywords) {
    const auto tokens = tokenizeText(text);
    if (tokens.empty()) return 0.0;
    std::unordered_set<std::string> tokenSet(tokens.begin(), tokens.end());
    int matches = 0;
    for (const auto& keyword : keywords) {
        const auto keywordTokens = tokenizeText(keyword);
        if (keywordTokens.empty()) continue;
        const bool allPresent = std::all_of(keywordTokens.begin(), keywordTokens.end(), [&](const auto& token) {
            return tokenSet.find(token) != tokenSet.end();
        });
        if (allPresent) ++matches;
    }
    return static_cast<double>(matches) / static_cast<double>(tokens.size());
}

bool MessageAnalyzer::containsProfanity(const std::string& content) {
    static const std::unordered_set<std::string> profanity = {"damn", "hell", "crap"};
    const auto tokens = tokenizeText(content);
    return std::any_of(tokens.begin(), tokens.end(), [](const std::string& token) {
        return profanity.find(token) != profanity.end();
    });
}

ChannelSummarizer::ChannelSummarizer()
    : topUsersLimit_(5), topTopicsLimit_(5), minimumMessages_(10) {}
ChannelSummarizer::~ChannelSummarizer() = default;

void ChannelSummarizer::setDataProviders(MessageProvider messages,
                                         AnalysisProvider analyses,
                                         ChannelProvider channels,
                                         GuildChannelsProvider guildChannels) {
    std::lock_guard<std::mutex> lock(summarizerMutex_);
    messageProvider_ = std::move(messages);
    analysisProvider_ = std::move(analyses);
    channelProvider_ = std::move(channels);
    guildChannelsProvider_ = std::move(guildChannels);
}

ChannelSummary ChannelSummarizer::summarizeChannel(
    const std::string& channelId, const Clock::time_point& startTime, const Clock::time_point& endTime) {
    MessageProvider messageProvider;
    AnalysisProvider analysisProvider;
    ChannelProvider channelProvider;
    int topUsersLimit = 0;
    int topTopicsLimit = 0;
    int minimumMessages = 0;
    {
        std::lock_guard<std::mutex> lock(summarizerMutex_);
        messageProvider = messageProvider_;
        analysisProvider = analysisProvider_;
        channelProvider = channelProvider_;
        topUsersLimit = topUsersLimit_;
        topTopicsLimit = topTopicsLimit_;
        minimumMessages = minimumMessages_;
    }

    ChannelSummary summary;
    summary.channelId = channelId;
    summary.periodStart = startTime;
    summary.periodEnd = endTime;
    if (channelId.empty() || endTime < startTime) return summary;

    if (channelProvider) {
        const auto channel = channelProvider(channelId);
        summary.channelName = channel.name;
    }
    if (summary.channelName.empty()) summary.channelName = channelId;

    auto messages = messageProvider ? messageProvider(channelId) : std::vector<DiscordMessage>{};
    messages.erase(std::remove_if(messages.begin(), messages.end(), [&](const DiscordMessage& message) {
        return message.channelId != channelId || message.timestamp < startTime || message.timestamp > endTime;
    }), messages.end());
    std::sort(messages.begin(), messages.end(), [](const auto& left, const auto& right) {
        if (left.timestamp != right.timestamp) return left.timestamp < right.timestamp;
        return left.id < right.id;
    });

    summary.totalMessages = static_cast<int>(messages.size());
    std::unordered_set<std::string> users;
    for (const auto& message : messages) {
        const auto identity = !message.authorId.empty() ? message.authorId : message.authorName;
        if (!identity.empty()) users.insert(identity);
    }
    summary.uniqueUsers = static_cast<int>(users.size());
    if (summary.totalMessages < minimumMessages) return summary;

    std::vector<MessageAnalysis> analyses;
    analyses.reserve(messages.size());
    if (analysisProvider) {
        for (const auto& message : messages) {
            auto analysis = analysisProvider(message.id);
            if (analysis.messageId == message.id) analyses.push_back(std::move(analysis));
        }
    }
    summary.topUsers = findTopUsers(messages, topUsersLimit);
    summary.mainTopics = findMainTopics(analyses, topTopicsLimit);
    summary.averageSentiment = calculateAverageSentiment(analyses);
    summary.mostActiveTime = findMostActiveTime(messages);
    return summary;
}

ChannelSummary ChannelSummarizer::summarizeChannelDaily(const std::string& channelId) {
    const auto end = Clock::now();
    return summarizeChannel(channelId, end - std::chrono::hours(24), end);
}

ChannelSummary ChannelSummarizer::summarizeChannelWeekly(const std::string& channelId) {
    const auto end = Clock::now();
    return summarizeChannel(channelId, end - std::chrono::hours(24 * 7), end);
}

std::vector<ChannelSummary> ChannelSummarizer::summarizeAllChannels(
    const std::string& guildId, const Clock::time_point& startTime, const Clock::time_point& endTime) {
    GuildChannelsProvider provider;
    {
        std::lock_guard<std::mutex> lock(summarizerMutex_);
        provider = guildChannelsProvider_;
    }
    if (!provider || guildId.empty() || endTime < startTime) return {};
    auto channels = provider(guildId);
    std::sort(channels.begin(), channels.end(), [](const auto& left, const auto& right) {
        return left.id < right.id;
    });
    std::vector<ChannelSummary> summaries;
    summaries.reserve(channels.size());
    for (const auto& channel : channels) {
        if (!channel.id.empty()) summaries.push_back(summarizeChannel(channel.id, startTime, endTime));
    }
    return summaries;
}

std::string ChannelSummarizer::generateTextReport(const ChannelSummary& summary) {
    std::ostringstream output;
    output << "Channel: " << summary.channelName << " (" << summary.channelId << ")\n"
           << "Period: " << formatTime(summary.periodStart) << " to " << formatTime(summary.periodEnd) << "\n"
           << "Messages: " << summary.totalMessages << "\n"
           << "Unique users: " << summary.uniqueUsers << "\n"
           << "Top users: " << join(summary.topUsers, ", ") << "\n"
           << "Main topics: " << join(summary.mainTopics, ", ") << "\n"
           << "Average sentiment: " << std::fixed << std::setprecision(3) << summary.averageSentiment << "\n"
           << "Most active time: " << summary.mostActiveTime;
    return output.str();
}

std::string ChannelSummarizer::generateJsonReport(const ChannelSummary& summary) {
    Json report{{"averageSentiment", summary.averageSentiment},
                {"channelId", summary.channelId},
                {"channelName", summary.channelName},
                {"mainTopics", summary.mainTopics},
                {"mostActiveTime", summary.mostActiveTime},
                {"periodEnd", formatTime(summary.periodEnd)},
                {"periodStart", formatTime(summary.periodStart)},
                {"topUsers", summary.topUsers},
                {"totalMessages", summary.totalMessages},
                {"uniqueUsers", summary.uniqueUsers}};
    return report.dump();
}

std::string ChannelSummarizer::generateHtmlReport(const ChannelSummary& summary) {
    std::ostringstream output;
    output << "<!doctype html><html><head><meta charset=\"utf-8\"><title>Discord channel summary</title></head><body>"
           << "<h1>" << htmlEscape(summary.channelName) << "</h1>"
           << "<p>Channel: " << htmlEscape(summary.channelId) << "</p>"
           << "<p>Period: " << htmlEscape(formatTime(summary.periodStart)) << " to "
           << htmlEscape(formatTime(summary.periodEnd)) << "</p>"
           << "<p>Messages: " << summary.totalMessages << "</p>"
           << "<p>Unique users: " << summary.uniqueUsers << "</p>"
           << "<p>Top users: " << htmlEscape(join(summary.topUsers, ", ")) << "</p>"
           << "<p>Main topics: " << htmlEscape(join(summary.mainTopics, ", ")) << "</p>"
           << "<p>Average sentiment: " << std::fixed << std::setprecision(3) << summary.averageSentiment << "</p>"
           << "<p>Most active time: " << htmlEscape(summary.mostActiveTime) << "</p></body></html>";
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
    if (limit <= 0) return {};
    std::unordered_map<std::string, int> counts;
    for (const auto& message : messages) {
        const auto name = !message.authorName.empty() ? message.authorName : message.authorId;
        if (!name.empty()) ++counts[name];
    }
    std::vector<std::pair<std::string, int>> ranked(counts.begin(), counts.end());
    std::sort(ranked.begin(), ranked.end(), [](const auto& left, const auto& right) {
        if (left.second != right.second) return left.second > right.second;
        return left.first < right.first;
    });
    std::vector<std::string> result;
    for (const auto& [user, count] : ranked) {
        (void)count;
        if (result.size() >= static_cast<std::size_t>(limit)) break;
        result.push_back(user);
    }
    return result;
}

std::vector<std::string> ChannelSummarizer::findMainTopics(
    const std::vector<MessageAnalysis>& analyses, int limit) {
    if (limit <= 0) return {};
    std::unordered_map<std::string, int> counts;
    for (const auto& analysis : analyses) {
        std::unordered_set<std::string> perMessage;
        for (const auto& topic : analysis.topics) {
            if (!topic.empty()) perMessage.insert(topic);
        }
        for (const auto& topic : perMessage) ++counts[topic];
    }
    std::vector<std::pair<std::string, int>> ranked(counts.begin(), counts.end());
    std::sort(ranked.begin(), ranked.end(), [](const auto& left, const auto& right) {
        if (left.second != right.second) return left.second > right.second;
        return left.first < right.first;
    });
    std::vector<std::string> result;
    for (const auto& [topic, count] : ranked) {
        (void)count;
        if (result.size() >= static_cast<std::size_t>(limit)) break;
        result.push_back(topic);
    }
    return result;
}

double ChannelSummarizer::calculateAverageSentiment(const std::vector<MessageAnalysis>& analyses) {
    if (analyses.empty()) return 0.0;
    const double total = std::accumulate(analyses.begin(), analyses.end(), 0.0,
        [](double value, const MessageAnalysis& analysis) { return value + analysis.sentiment; });
    return total / static_cast<double>(analyses.size());
}

std::string ChannelSummarizer::findMostActiveTime(const std::vector<DiscordMessage>& messages) {
    if (messages.empty()) return {};
    std::array<int, 24> counts{};
    for (const auto& message : messages) {
        const auto tm = utcTime(Clock::to_time_t(message.timestamp));
        ++counts[static_cast<std::size_t>(tm.tm_hour)];
    }
    const auto found = std::max_element(counts.begin(), counts.end());
    const int hour = static_cast<int>(std::distance(counts.begin(), found));
    std::ostringstream output;
    output << std::setfill('0') << std::setw(2) << hour << ":00-"
           << std::setw(2) << ((hour + 1) % 24) << ":00 UTC";
    return output.str();
}

DiscordDataManager::DiscordDataManager() : maxCacheSize_(1000), persistenceEnabled_(false) {}
DiscordDataManager::~DiscordDataManager() = default;

bool DiscordDataManager::storeMessage(const DiscordMessage& message) {
    if (message.id.empty() || message.channelId.empty()) return false;
    return storeMessages({message});
}

bool DiscordDataManager::storeMessages(const std::vector<DiscordMessage>& messages) {
    if (messages.empty() || std::any_of(messages.begin(), messages.end(), [](const DiscordMessage& message) {
        return message.id.empty() || message.channelId.empty();
    })) return false;
    std::lock_guard<std::mutex> lock(dataMutex_);
    if (maxCacheSize_ == 0) return false;
    const auto previousCache = messageCache_;
    const auto previousOrder = messageOrder_;
    for (const auto& message : messages) {
        messageCache_[message.id] = message;
        messageOrder_.erase(std::remove(messageOrder_.begin(), messageOrder_.end(), message.id), messageOrder_.end());
        messageOrder_.push_back(message.id);
    }
    enforceBoundsLocked();
    if (!persistLocked()) {
        messageCache_ = previousCache;
        messageOrder_ = previousOrder;
        return false;
    }
    return true;
}

std::vector<DiscordMessage> DiscordDataManager::retrieveMessages(const std::string& channelId, int limit) {
    if (channelId.empty() || limit <= 0) return {};
    std::lock_guard<std::mutex> lock(dataMutex_);
    std::vector<DiscordMessage> result;
    for (const auto& [id, message] : messageCache_) {
        (void)id;
        if (message.channelId == channelId) result.push_back(message);
    }
    std::sort(result.begin(), result.end(), [](const auto& left, const auto& right) {
        if (left.timestamp != right.timestamp) return left.timestamp > right.timestamp;
        return left.id < right.id;
    });
    if (result.size() > static_cast<std::size_t>(limit)) result.resize(static_cast<std::size_t>(limit));
    return result;
}

bool DiscordDataManager::storeAnalysis(const MessageAnalysis& analysis) {
    if (analysis.messageId.empty()) return false;
    return storeAnalyses({analysis});
}

bool DiscordDataManager::storeAnalyses(const std::vector<MessageAnalysis>& analyses) {
    if (analyses.empty() || std::any_of(analyses.begin(), analyses.end(), [](const MessageAnalysis& analysis) {
        return analysis.messageId.empty();
    })) return false;
    std::lock_guard<std::mutex> lock(dataMutex_);
    if (maxCacheSize_ == 0) return false;
    const auto previousCache = analysisCache_;
    const auto previousOrder = analysisOrder_;
    for (const auto& analysis : analyses) {
        analysisCache_[analysis.messageId] = analysis;
        analysisOrder_.erase(std::remove(analysisOrder_.begin(), analysisOrder_.end(), analysis.messageId), analysisOrder_.end());
        analysisOrder_.push_back(analysis.messageId);
    }
    enforceBoundsLocked();
    if (!persistLocked()) {
        analysisCache_ = previousCache;
        analysisOrder_ = previousOrder;
        return false;
    }
    return true;
}

MessageAnalysis DiscordDataManager::retrieveAnalysis(const std::string& messageId) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    const auto found = analysisCache_.find(messageId);
    return found == analysisCache_.end() ? MessageAnalysis{} : found->second;
}

bool DiscordDataManager::storeSummary(const ChannelSummary& summary) {
    if (summary.channelId.empty() || summary.periodEnd < summary.periodStart) return false;
    std::lock_guard<std::mutex> lock(dataMutex_);
    if (maxCacheSize_ == 0) return false;
    const auto previousCache = summaryCache_;
    const auto previousOrder = summaryOrder_;
    summaryCache_[summary.channelId].push_back(summary);
    summaryOrder_.push_back(summary.channelId);
    enforceBoundsLocked();
    if (!persistLocked()) {
        summaryCache_ = previousCache;
        summaryOrder_ = previousOrder;
        return false;
    }
    return true;
}

std::vector<ChannelSummary> DiscordDataManager::retrieveSummaries(const std::string& channelId) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    const auto found = summaryCache_.find(channelId);
    if (found == summaryCache_.end()) return {};
    auto result = found->second;
    std::stable_sort(result.begin(), result.end(), [](const auto& left, const auto& right) {
        if (left.periodEnd != right.periodEnd) return left.periodEnd > right.periodEnd;
        return left.periodStart > right.periodStart;
    });
    return result;
}

void DiscordDataManager::setCacheSize(std::size_t maxEntries) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    const auto oldSize = maxCacheSize_;
    const auto oldMessages = messageCache_;
    const auto oldAnalyses = analysisCache_;
    const auto oldSummaries = summaryCache_;
    const auto oldMessageOrder = messageOrder_;
    const auto oldAnalysisOrder = analysisOrder_;
    const auto oldSummaryOrder = summaryOrder_;
    maxCacheSize_ = maxEntries;
    enforceBoundsLocked();
    if (!persistLocked()) {
        maxCacheSize_ = oldSize;
        messageCache_ = oldMessages;
        analysisCache_ = oldAnalyses;
        summaryCache_ = oldSummaries;
        messageOrder_ = oldMessageOrder;
        analysisOrder_ = oldAnalysisOrder;
        summaryOrder_ = oldSummaryOrder;
    }
}

void DiscordDataManager::clearCache() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    const auto oldMessages = messageCache_;
    const auto oldAnalyses = analysisCache_;
    const auto oldSummaries = summaryCache_;
    const auto oldMessageOrder = messageOrder_;
    const auto oldAnalysisOrder = analysisOrder_;
    const auto oldSummaryOrder = summaryOrder_;
    messageCache_.clear();
    analysisCache_.clear();
    summaryCache_.clear();
    messageOrder_.clear();
    analysisOrder_.clear();
    summaryOrder_.clear();
    if (!persistLocked()) {
        messageCache_ = oldMessages;
        analysisCache_ = oldAnalyses;
        summaryCache_ = oldSummaries;
        messageOrder_ = oldMessageOrder;
        analysisOrder_ = oldAnalysisOrder;
        summaryOrder_ = oldSummaryOrder;
    }
}

void DiscordDataManager::enablePersistence(const std::string& dataPath) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    if (dataPath.empty()) {
        persistenceEnabled_ = false;
        persistencePath_.clear();
        return;
    }
    std::filesystem::path path(dataPath);
    std::error_code error;
    if ((std::filesystem::exists(path, error) && std::filesystem::is_directory(path, error)) || !path.has_extension()) {
        path /= "discord_summarizer.json";
    }
    persistencePath_ = path.lexically_normal().string();
    persistenceEnabled_ = true;
    const auto data = loadFromFile(persistencePath_);
    if (!data.empty() && !loadSnapshotLocked(data)) {
        logWarning("Ignoring invalid Discord persistence snapshot: " + persistencePath_, "discord_summarizer");
    }
    if (data.empty()) persistLocked();
}

bool DiscordDataManager::saveToFile(const std::string& filePath, const std::string& data) {
    if (filePath.empty()) return false;
    const std::filesystem::path target(filePath);
    const std::filesystem::path temporary = target.string() + ".tmp";
    std::error_code error;
    if (!target.parent_path().empty()) {
        std::filesystem::create_directories(target.parent_path(), error);
        if (error) return false;
    }
    {
        std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
        if (!output) return false;
        output.write(data.data(), static_cast<std::streamsize>(data.size()));
        output.flush();
        if (!output) {
            output.close();
            std::filesystem::remove(temporary, error);
            return false;
        }
    }
    std::filesystem::rename(temporary, target, error);
    if (!error) return true;
    std::filesystem::remove(target, error);
    error.clear();
    std::filesystem::rename(temporary, target, error);
    if (error) {
        std::filesystem::remove(temporary, error);
        return false;
    }
    return true;
}

std::string DiscordDataManager::loadFromFile(const std::string& filePath) {
    std::ifstream input(filePath, std::ios::binary);
    if (!input) return {};
    std::ostringstream data;
    data << input.rdbuf();
    return input.bad() ? std::string{} : data.str();
}

bool DiscordDataManager::persistLocked() {
    return !persistenceEnabled_ || saveToFile(persistencePath_, serializeLocked());
}

std::string DiscordDataManager::serializeLocked() const {
    Json root{{"analyses", Json::array()},
              {"max_cache_size", maxCacheSize_},
              {"messages", Json::array()},
              {"summaries", Json::array()},
              {"version", 1}};
    for (const auto& id : messageOrder_) {
        const auto found = messageCache_.find(id);
        if (found != messageCache_.end()) root["messages"].push_back(messageToJson(found->second));
    }
    for (const auto& id : analysisOrder_) {
        const auto found = analysisCache_.find(id);
        if (found != analysisCache_.end()) root["analyses"].push_back(analysisToJson(found->second));
    }
    std::unordered_map<std::string, std::size_t> summaryIndexes;
    for (const auto& channelId : summaryOrder_) {
        const auto found = summaryCache_.find(channelId);
        if (found == summaryCache_.end()) continue;
        const auto index = summaryIndexes[channelId]++;
        if (index < found->second.size()) root["summaries"].push_back(summaryToJson(found->second[index]));
    }
    return root.dump(2) + "\n";
}

bool DiscordDataManager::loadSnapshotLocked(const std::string& data) {
    try {
        const auto root = Json::parse(data);
        if (root.value("version", 0) != 1 || !root.at("messages").is_array() ||
            !root.at("analyses").is_array() || !root.at("summaries").is_array()) return false;

        std::unordered_map<std::string, DiscordMessage> messages;
        std::unordered_map<std::string, MessageAnalysis> analyses;
        std::unordered_map<std::string, std::vector<ChannelSummary>> summaries;
        std::deque<std::string> messageOrder;
        std::deque<std::string> analysisOrder;
        std::deque<std::string> summaryOrder;

        for (const auto& value : root.at("messages")) {
            auto message = messageFromJson(value);
            if (message.id.empty() || message.channelId.empty()) return false;
            messages[message.id] = message;
            messageOrder.erase(std::remove(messageOrder.begin(), messageOrder.end(), message.id), messageOrder.end());
            messageOrder.push_back(message.id);
        }
        for (const auto& value : root.at("analyses")) {
            auto analysis = analysisFromJson(value);
            if (analysis.messageId.empty()) return false;
            analyses[analysis.messageId] = analysis;
            analysisOrder.erase(std::remove(analysisOrder.begin(), analysisOrder.end(), analysis.messageId), analysisOrder.end());
            analysisOrder.push_back(analysis.messageId);
        }
        for (const auto& value : root.at("summaries")) {
            auto summary = summaryFromJson(value);
            if (summary.channelId.empty() || summary.periodEnd < summary.periodStart) return false;
            summaries[summary.channelId].push_back(summary);
            summaryOrder.push_back(summary.channelId);
        }

        const auto persistedSize = root.value("max_cache_size", maxCacheSize_);
        maxCacheSize_ = persistedSize;
        messageCache_ = std::move(messages);
        analysisCache_ = std::move(analyses);
        summaryCache_ = std::move(summaries);
        messageOrder_ = std::move(messageOrder);
        analysisOrder_ = std::move(analysisOrder);
        summaryOrder_ = std::move(summaryOrder);
        enforceBoundsLocked();
        return true;
    } catch (...) {
        return false;
    }
}

void DiscordDataManager::enforceBoundsLocked() {
    while (messageOrder_.size() > maxCacheSize_) {
        messageCache_.erase(messageOrder_.front());
        messageOrder_.pop_front();
    }
    while (analysisOrder_.size() > maxCacheSize_) {
        analysisCache_.erase(analysisOrder_.front());
        analysisOrder_.pop_front();
    }
    while (summaryOrder_.size() > maxCacheSize_) {
        const auto channelId = summaryOrder_.front();
        summaryOrder_.pop_front();
        auto found = summaryCache_.find(channelId);
        if (found == summaryCache_.end()) continue;
        if (!found->second.empty()) found->second.erase(found->second.begin());
        if (found->second.empty()) summaryCache_.erase(found);
    }
}

DiscordSummarizer::DiscordSummarizer()
    : client_(std::make_unique<InProcessDiscordClient>()),
      monitoring_(false),
      monitoringInterval_(std::chrono::milliseconds(50)) {
    config_["cache_size"] = "1000";
    config_["minimum_messages"] = "10";
    config_["monitoring_interval_ms"] = "50";
    config_["persistence_path"] = "";
    config_["top_topics_limit"] = "5";
    config_["top_users_limit"] = "5";
    summarizer_.setDataProviders(
        [this](const std::string& channelId) { return dataManager_.retrieveMessages(channelId, std::numeric_limits<int>::max()); },
        [this](const std::string& messageId) { return dataManager_.retrieveAnalysis(messageId); },
        [this](const std::string& channelId) { return client_->getChannel(channelId); },
        [this](const std::string& guildId) { return client_->getChannels(guildId); });
}

DiscordSummarizer::~DiscordSummarizer() {
    stopMonitoring();
    if (client_) client_->setMessageHandler({});
}

bool DiscordSummarizer::initializeWithToken(const std::string& token) {
    if (!client_->connect(token)) return false;
    client_->setMessageHandler([this](const DiscordMessage& message) { processNewMessage(message); });
    return true;
}

std::future<ChannelSummary> DiscordSummarizer::generateChannelSummary(
    const std::string& channelId, const Clock::time_point& startTime, const Clock::time_point& endTime) {
    ChannelSummary summary;
    if (!channelId.empty() && endTime >= startTime) {
        const auto
 fetched = client_->getMessages(channelId, std::numeric_limits<int>::max());
        for (const auto& message : fetched) processNewMessage(message);
        summary = summarizer_.summarizeChannel(channelId, startTime, endTime);
        dataManager_.storeSummary(summary);
    }
    return std::async(std::launch::deferred, [summary]() { return summary; });
}

std::future<std::vector<ChannelSummary>> DiscordSummarizer::generateGuildSummary(
    const std::string& guildId, const Clock::time_point& startTime, const Clock::time_point& endTime) {
    std::vector<ChannelSummary> summaries;
    if (!guildId.empty() && endTime >= startTime) {
        auto channels = client_->getChannels(guildId);
        std::sort(channels.begin(), channels.end(), [](const auto& left, const auto& right) {
            return left.id < right.id;
        });
        for (const auto& channel : channels) {
            const auto fetched = client_->getMessages(channel.id, std::numeric_limits<int>::max());
            for (const auto& message : fetched) processNewMessage(message);
            auto summary = summarizer_.summarizeChannel(channel.id, startTime, endTime);
            dataManager_.storeSummary(summary);
            summaries.push_back(std::move(summary));
        }
    }
    return std::async(std::launch::deferred, [summaries]() { return summaries; });
}

void DiscordSummarizer::startMonitoring(const std::vector<std::string>& channelIds) {
    std::lock_guard<std::mutex> lifecycleLock(monitoringLifecycleMutex_);
    if (monitoring_.load() || !client_->isConnected()) return;
    auto channels = uniqueNonEmpty(channelIds);
    if (channels.empty()) return;
    if (monitoringThread_.joinable()) monitoringThread_.join();
    {
        std::lock_guard<std::mutex> configLock(configMutex_);
        monitoredChannels_ = channels;
        monitoringCursors_.clear();
        monitoringCursorMessageIds_.clear();
        for (const auto& channelId : channels) monitoringCursors_[channelId] = Clock::time_point::min();
    }
    monitoring_.store(true);
    monitoringThread_ = std::thread(&DiscordSummarizer::monitoringLoop, this);
}

void DiscordSummarizer::stopMonitoring() {
    std::lock_guard<std::mutex> lifecycleLock(monitoringLifecycleMutex_);
    monitoring_.store(false);
    monitoringCv_.notify_all();
    if (monitoringThread_.joinable()) monitoringThread_.join();
}

bool DiscordSummarizer::isMonitoring() const {
    return monitoring_.load();
}

void DiscordSummarizer::loadConfiguration(const std::string& configPath) {
    std::ifstream input(configPath, std::ios::binary);
    if (!input) return;

    std::unordered_map<std::string, std::string> parsed;
    std::vector<std::string> channels;
    std::string line;
    bool versionSeen = false;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line.front() == '#') continue;
        const auto separator = line.find('=');
        if (separator == std::string::npos) return;
        const auto key = line.substr(0, separator);
        std::string value;
        if (!decodeConfigValue(line.substr(separator + 1), value)) return;
        if (key == "version") {
            if (value != "1") return;
            versionSeen = true;
        } else if (key == "monitored_channels") {
            channels = splitCommaSeparated(value);
        } else if (!key.empty()) {
            parsed[key] = value;
        } else {
            return;
        }
    }
    if (!versionSeen || input.bad()) return;

    int topUsers = 5;
    int topTopics = 5;
    int minimumMessages = 10;
    int cacheSize = 1000;
    int monitoringInterval = 50;
    const auto parseSetting = [&](const char* key, int fallback, int& output) {
        const auto found = parsed.find(key);
        if (found == parsed.end()) {
            output = fallback;
            return true;
        }
        return parseNonNegativeInt(found->second, output);
    };
    if (!parseSetting("top_users_limit", 5, topUsers) ||
        !parseSetting("top_topics_limit", 5, topTopics) ||
        !parseSetting("minimum_messages", 10, minimumMessages) ||
        !parseSetting("cache_size", 1000, cacheSize) ||
        !parseSetting("monitoring_interval_ms", 50, monitoringInterval) ||
        monitoringInterval == 0) return;

    {
        std::lock_guard<std::mutex> lock(configMutex_);
        config_ = parsed;
        config_["top_users_limit"] = std::to_string(topUsers);
        config_["top_topics_limit"] = std::to_string(topTopics);
        config_["minimum_messages"] = std::to_string(minimumMessages);
        config_["cache_size"] = std::to_string(cacheSize);
        config_["monitoring_interval_ms"] = std::to_string(monitoringInterval);
        if (config_.find("persistence_path") == config_.end()) config_["persistence_path"] = "";
        monitoredChannels_ = channels;
        monitoringInterval_ = std::chrono::milliseconds(monitoringInterval);
    }
    summarizer_.setTopUsersLimit(topUsers);
    summarizer_.setTopTopicsLimit(topTopics);
    summarizer_.setMinimumMessages(minimumMessages);
    dataManager_.enablePersistence(parsed.count("persistence_path") ? parsed.at("persistence_path") : "");
    dataManager_.setCacheSize(static_cast<std::size_t>(cacheSize));
}

void DiscordSummarizer::saveConfiguration(const std::string& configPath) {
    if (configPath.empty()) return;
    std::map<std::string, std::string> snapshot;
    std::vector<std::string> channels;
    {
        std::lock_guard<std::mutex> lock(configMutex_);
        snapshot.insert(config_.begin(), config_.end());
        channels = monitoredChannels_;
        snapshot["monitoring_interval_ms"] = std::to_string(monitoringInterval_.count());
    }
    channels = uniqueNonEmpty(channels);
    std::sort(channels.begin(), channels.end());

    std::ostringstream output;
    output << "version=1\n";
    for (const auto& [key, value] : snapshot) output << key << '=' << encodeConfigValue(value) << '\n';
    output << "monitored_channels=" << encodeConfigValue(join(channels, ",")) << '\n';

    const std::filesystem::path target(configPath);
    const auto temporary = target.string() + ".tmp";
    std::error_code error;
    if (!target.parent_path().empty()) {
        std::filesystem::create_directories(target.parent_path(), error);
        if (error) return;
    }
    {
        std::ofstream file(temporary, std::ios::binary | std::ios::trunc);
        if (!file) return;
        file << output.str();
        file.flush();
        if (!file) {
            file.close();
            std::filesystem::remove(temporary, error);
            return;
        }
    }
    std::filesystem::rename(temporary, target, error);
    if (error) {
        std::filesystem::remove(target, error);
        error.clear();
        std::filesystem::rename(temporary, target, error);
        if (error) std::filesystem::remove(temporary, error);
    }
}

void DiscordSummarizer::monitoringLoop() {
    while (monitoring_.load()) {
        std::vector<std::string> channels;
        std::chrono::milliseconds interval;
        {
            std::lock_guard<std::mutex> lock(configMutex_);
            channels = monitoredChannels_;
            interval = monitoringInterval_;
        }
        for (const auto& channelId : channels) {
            if (!monitoring_.load()) break;
            Clock::time_point cursor;
            {
                std::lock_guard<std::mutex> lock(configMutex_);
                cursor = monitoringCursors_[channelId];
            }
            const auto messages = client_->getMessagesSince(channelId, cursor);
            for (const auto& message : messages) {
                bool alreadyProcessed = false;
                {
                    std::lock_guard<std::mutex> lock(configMutex_);
                    const auto current = monitoringCursors_[channelId];
                    alreadyProcessed = message.timestamp == current &&
                        monitoringCursorMessageIds_[channelId].find(message.id) != monitoringCursorMessageIds_[channelId].end();
                }
                if (alreadyProcessed) continue;
                processNewMessage(message);
                std::lock_guard<std::mutex> lock(configMutex_);
                auto& current = monitoringCursors_[channelId];
                auto& processedIds = monitoringCursorMessageIds_[channelId];
                if (message.timestamp > current) {
                    current = message.timestamp;
                    processedIds.clear();
                }
                if (message.timestamp == current) processedIds.insert(message.id);
            }
        }
        std::unique_lock<std::mutex> waitLock(monitoringWaitMutex_);
        monitoringCv_.wait_for(waitLock, interval, [this]() { return !monitoring_.load(); });
    }
}

void DiscordSummarizer::processNewMessage(const DiscordMessage& message) {
    if (!dataManager_.storeMessage(message)) return;
    const auto analysis = analyzer_.analyzeMessage(message);
    dataManager_.storeAnalysis(analysis);
}

} // namespace elizaos
