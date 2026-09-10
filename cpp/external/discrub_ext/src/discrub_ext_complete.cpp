#include "elizaos/discrub_ext.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>
#include <map>
#include <set>
#include <sstream>
#include <thread>

namespace elizaos {
namespace {

using Json = nlohmann::json;
constexpr std::size_t kMaximumArchiveBytes = 8U * 1024U * 1024U;
constexpr std::size_t kMaximumCachedMessagesPerChannel = 1000;
constexpr std::size_t kMaximumPaginationPages = 100;

bool validDiscordId(const std::string& value) {
    return value.size() >= 17 && value.size() <= 20 &&
           value != std::string(value.size(), '0') &&
           std::all_of(value.begin(), value.end(), [](unsigned char character) {
               return std::isdigit(character) != 0;
           });
}

bool validOptionalDiscordId(const std::string& value) {
    return value.empty() || validDiscordId(value);
}

bool validReason(const std::string& value, bool allowEmpty = false) {
    return (allowEmpty || !value.empty()) && value.size() <= DISCRUB_MAX_REASON_LENGTH &&
           value.find('\0') == std::string::npos;
}

bool validTimestamp(const std::chrono::system_clock::time_point& timestamp) {
    const auto now = std::chrono::system_clock::now();
    return timestamp.time_since_epoch().count() > 0 &&
           timestamp <= now + std::chrono::minutes(5);
}

std::int64_t toMilliseconds(const std::chrono::system_clock::time_point& value) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               value.time_since_epoch()).count();
}

std::chrono::system_clock::time_point fromMilliseconds(std::int64_t value) {
    return std::chrono::system_clock::time_point(std::chrono::milliseconds(value));
}

DiscordAcknowledgement makeAcknowledgement(DiscordOperationStatus status,
                                            const std::string& operation,
                                            const std::string& resource,
                                            const std::string& detail) {
    DiscordAcknowledgement result;
    result.status = status;
    result.operation = operation;
    result.resourceId = resource;
    result.detail = detail;
    return result;
}

bool messageLess(const DiscordMessage& left, const DiscordMessage& right) {
    if (left.timestamp != right.timestamp) {
        return left.timestamp < right.timestamp;
    }
    return left.id < right.id;
}

bool pathWithin(const std::filesystem::path& root,
                const std::filesystem::path& requested,
                std::filesystem::path& resolved) {
    if (root.empty() || requested.empty()) {
        return false;
    }
    std::error_code error;
    const auto canonicalRoot = std::filesystem::weakly_canonical(root, error);
    if (error) {
        return false;
    }
    const auto candidate = requested.is_absolute() ? requested : canonicalRoot / requested;
    resolved = std::filesystem::weakly_canonical(candidate, error);
    if (error) {
        return false;
    }
    auto rootIterator = canonicalRoot.begin();
    auto candidateIterator = resolved.begin();
    while (rootIterator != canonicalRoot.end() && candidateIterator != resolved.end()) {
        if (*rootIterator != *candidateIterator) {
            return false;
        }
        ++rootIterator;
        ++candidateIterator;
    }
    return rootIterator == canonicalRoot.end();
}

Json messageToJson(const DiscordMessage& message) {
    return Json{{"id", message.id},
                {"channelId", message.channelId},
                {"guildId", message.guildId},
                {"authorId", message.authorId},
                {"authorName", message.authorName},
                {"content", message.content},
                {"timestampMs", toMilliseconds(message.timestamp)},
                {"attachments", message.attachments},
                {"embeds", message.embeds},
                {"reactions", message.reactions},
                {"isBot", message.isBot}};
}

bool jsonToMessage(const Json& json, DiscordMessage& message) {
    try {
        message.id = json.at("id").get<std::string>();
        message.channelId = json.at("channelId").get<std::string>();
        message.guildId = json.value("guildId", std::string{});
        message.authorId = json.at("authorId").get<std::string>();
        message.authorName = json.value("authorName", std::string{});
        message.content = json.at("content").get<std::string>();
        message.timestamp = fromMilliseconds(json.at("timestampMs").get<std::int64_t>());
        message.attachments = json.value("attachments", std::vector<std::string>{});
        message.embeds = json.value("embeds", std::vector<std::string>{});
        message.reactions = json.value("reactions", std::vector<std::string>{});
        message.isBot = json.value("isBot", false);
    } catch (...) {
        return false;
    }
    return validDiscordId(message.id) && validDiscordId(message.channelId) &&
           validOptionalDiscordId(message.guildId) && validDiscordId(message.authorId) &&
           validTimestamp(message.timestamp) &&
           message.content.size() <= DISCRUB_MAX_CONTENT_LENGTH;
}

std::string escapeHtml(const std::string& value) {
    std::string result;
    result.reserve(value.size());
    for (char character : value) {
        switch (character) {
            case '&': result += "&amp;"; break;
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default: result.push_back(character); break;
        }
    }
    return result;
}

} // namespace

ContentCleaner::ContentCleaner() = default;

ContentCleaner::ContentCleaner(
    std::shared_ptr<DiscordDataAdapter> dataAdapter,
    std::shared_ptr<DiscordMutationAdapter> mutationAdapter)
    : dataAdapter_(std::move(dataAdapter)),
      mutationAdapter_(std::move(mutationAdapter)) {}

ContentCleaner::~ContentCleaner() {
    {
        std::lock_guard<std::mutex> lock(cleanerMutex_);
        cleanupRunning_.store(false);
    }
    cleanupCv_.notify_all();
    if (cleanupThread_.joinable()) {
        cleanupThread_.join();
    }
}

void ContentCleaner::setAdapters(
    std::shared_ptr<DiscordDataAdapter> dataAdapter,
    std::shared_ptr<DiscordMutationAdapter> mutationAdapter) {
    std::lock_guard<std::mutex> lock(cleanerMutex_);
    dataAdapter_ = std::move(dataAdapter);
    mutationAdapter_ = std::move(mutationAdapter);
}

void ContentCleaner::setRetryPolicy(int maxAttempts,
                                    std::chrono::milliseconds maximumDelay) {
    if (maxAttempts < 1 || maxAttempts > 5 || maximumDelay.count() < 0 ||
        maximumDelay > std::chrono::seconds(5)) {
        return;
    }
    std::lock_guard<std::mutex> lock(cleanerMutex_);
    maxAttempts_ = maxAttempts;
    maximumRetryDelay_ = maximumDelay;
}

bool ContentCleaner::setStorageRoot(const std::string& rootPath) {
    if (rootPath.empty() || rootPath.size() > 4096) {
        return false;
    }
    std::error_code error;
    std::filesystem::create_directories(rootPath, error);
    if (error) {
        return false;
    }
    const auto root = std::filesystem::weakly_canonical(rootPath, error);
    if (error || !std::filesystem::is_directory(root, error)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(cleanerMutex_);
    storageRoot_ = root;
    return true;
}

std::string ContentCleaner::getStorageRoot() const {
    std::lock_guard<std::mutex> lock(cleanerMutex_);
    return storageRoot_.string();
}

MessagePage ContentCleaner::fetchMessages(const MessageQuery& query) {
    if (!validDiscordId(query.channelId) || !validOptionalDiscordId(query.guildId) ||
        query.limit == 0 || query.limit > DISCRUB_MAX_PAGE_SIZE ||
        query.searchText.size() > 256 || query.cursor.size() > 256 ||
        (query.after && query.before && *query.after > *query.before)) {
        MessagePage page;
        page.acknowledgement = makeAcknowledgement(
            DiscordOperationStatus::INVALID_INPUT, "fetch_messages", query.channelId,
            "invalid query ids, bounds, cursor, or time range");
        return page;
    }
    std::shared_ptr<DiscordDataAdapter> adapter;
    {
        std::lock_guard<std::mutex> lock(cleanerMutex_);
        adapter = dataAdapter_;
    }
    if (!adapter) {
        MessagePage page;
        page.acknowledgement = makeAcknowledgement(
            DiscordOperationStatus::NO_ADAPTER, "fetch_messages", query.channelId,
            "remote fetch requires a DiscordDataAdapter");
        return page;
    }
    MessagePage page;
    try {
        page = adapter->fetchMessages(query);
    } catch (...) {
        page.acknowledgement = makeAcknowledgement(
            DiscordOperationStatus::ADAPTER_ERROR, "fetch_messages", query.channelId,
            "data adapter threw an exception");
        return page;
    }
    page.acknowledgement.operation = "fetch_messages";
    page.acknowledgement.resourceId = query.channelId;
    page.acknowledgement.attempts = 1;
    if (page.acknowledgement.acknowledged() && page.acknowledgement.receiptId.empty()) {
        page.acknowledgement.status = DiscordOperationStatus::ADAPTER_ERROR;
        page.acknowledgement.detail = "fetch acknowledgement omitted receipt";
        page.messages.clear();
        page.nextCursor.clear();
        page.hasMore = false;
        return page;
    }
    if (!page.acknowledgement.acknowledged()) {
        page.messages.clear();
        page.nextCursor.clear();
        page.hasMore = false;
        return page;
    }
    if (page.messages.size() > query.limit || page.nextCursor.size() > 256 ||
        (page.hasMore && (page.nextCursor.empty() || page.nextCursor == query.cursor))) {
        page.acknowledgement.status = DiscordOperationStatus::ADAPTER_ERROR;
        page.acknowledgement.detail = "adapter returned invalid pagination metadata";
        page.messages.clear();
        page.nextCursor.clear();
        page.hasMore = false;
        return page;
    }
    std::set<std::string> ids;
    for (const auto& message : page.messages) {
        const bool matchesQuery = message.channelId == query.channelId &&
            (query.guildId.empty() || message.guildId == query.guildId) &&
            (query.includeBots || !message.isBot) &&
            (!query.after || message.timestamp >= *query.after) &&
            (!query.before || message.timestamp <= *query.before);
        if (!matchesQuery || !validDiscordId(message.id) ||
            !validDiscordId(message.authorId) || !validTimestamp(message.timestamp) ||
            message.content.size() > DISCRUB_MAX_CONTENT_LENGTH ||
            !ids.insert(message.id).second) {
            page.acknowledgement.status = DiscordOperationStatus::ADAPTER_ERROR;
            page.acknowledgement.detail = "adapter returned malformed or duplicate message";
            page.messages.clear();
            page.nextCursor.clear();
            page.hasMore = false;
            return page;
        }
    }
    std::sort(page.messages.begin(), page.messages.end(), messageLess);
    return page;
}

MessagePage ContentCleaner::searchMessages(const MessageQuery& query) {
    if (query.searchText.empty() || query.searchText.size() > 256 ||
        !validDiscordId(query.channelId) || !validOptionalDiscordId(query.guildId) ||
        query.limit == 0 || query.limit > DISCRUB_MAX_PAGE_SIZE ||
        query.cursor.size() > 256 ||
        (query.after && query.before && *query.after > *query.before)) {
        MessagePage page;
        page.acknowledgement = makeAcknowledgement(
            DiscordOperationStatus::INVALID_INPUT, "search_messages", query.channelId,
            "invalid search query");
        return page;
    }
    std::shared_ptr<DiscordDataAdapter> adapter;
    {
        std::lock_guard<std::mutex> lock(cleanerMutex_);
        adapter = dataAdapter_;
    }
    if (!adapter) {
        MessagePage page;
        page.acknowledgement = makeAcknowledgement(
            DiscordOperationStatus::NO_ADAPTER, "search_messages", query.channelId,
            "remote search requires a DiscordDataAdapter");
        return page;
    }
    MessagePage page;
    try {
        page = adapter->searchMessages(query);
    } catch (...) {
        page.acknowledgement = makeAcknowledgement(
            DiscordOperationStatus::ADAPTER_ERROR, "search_messages", query.channelId,
            "data adapter threw an exception");
        return page;
    }
    page.acknowledgement.operation = "search_messages";
    page.acknowledgement.resourceId = query.channelId;
    page.acknowledgement.attempts = 1;
    if (!page.acknowledgement.acknowledged() ||
        page.acknowledgement.receiptId.empty() || page.messages.size() > query.limit ||
        page.nextCursor.size() > 256 ||
        (page.hasMore && (page.nextCursor.empty() || page.nextCursor == query.cursor))) {
        if (page.acknowledgement.acknowledged()) {
            page.acknowledgement.status = DiscordOperationStatus::ADAPTER_ERROR;
            page.acknowledgement.detail = "invalid search acknowledgement or pagination";
        }
        page.messages.clear();
        page.nextCursor.clear();
        page.hasMore = false;
        return page;
    }
    const std::string needle = query.searchText;
    std::set<std::string> ids;
    for (const auto& message : page.messages) {
        const bool matches = message.channelId == query.channelId &&
            (query.guildId.empty() || message.guildId == query.guildId) &&
            message.content.find(needle) != std::string::npos &&
            (query.includeBots || !message.isBot) &&
            (!query.after || message.timestamp >= *query.after) &&
            (!query.before || message.timestamp <= *query.before);
        if (!matches || !validDiscordId(message.id) ||
            !validDiscordId(message.authorId) || !validTimestamp(message.timestamp) ||
            message.content.size() > DISCRUB_MAX_CONTENT_LENGTH ||
            !ids.insert(message.id).second) {
            page.acknowledgement.status = DiscordOperationStatus::ADAPTER_ERROR;
            page.acknowledgement.detail = "adapter returned invalid search evidence";
            page.messages.clear();
            page.nextCursor.clear();
            page.hasMore = false;
            return page;
        }
    }
    std::sort(page.messages.begin(), page.messages.end(), messageLess);
    return page;
}

std::vector<DiscordMessage> ContentCleaner::fetchAllMessages(
    const MessageQuery& query, DiscordAcknowledgement& result) {
    std::vector<DiscordMessage> messages;
    std::set<std::string> seenIds;
    std::set<std::string> seenCursors;
    MessageQuery pageQuery = query;
    pageQuery.limit = DISCRUB_MAX_PAGE_SIZE;
    for (std::size_t pageIndex = 0; pageIndex < kMaximumPaginationPages; ++pageIndex) {
        if (!seenCursors.insert(pageQuery.cursor).second) {
            result = makeAcknowledgement(DiscordOperationStatus::ADAPTER_ERROR,
                                         "fetch_messages", query.channelId,
                                         "pagination cursor cycle detected");
            return {};
        }
        auto page = fetchMessages(pageQuery);
        result = page.acknowledgement;
        if (!result.acknowledged()) {
            return {};
        }
        for (const auto& message : page.messages) {
            if (seenIds.insert(message.id).second) {
                messages.push_back(message);
            }
        }
        if (!page.hasMore) {
            std::sort(messages.begin(), messages.end(), messageLess);
            return messages;
        }
        pageQuery.cursor = page.nextCursor;
    }
    result = makeAcknowledgement(DiscordOperationStatus::ADAPTER_ERROR,
                                 "fetch_messages", query.channelId,
                                 "pagination exceeded safety bound");
    return {};
}

DiscordAcknowledgement ContentCleaner::invokeMutation(
    const std::string& operation, const std::string& resourceId,
    const std::function<DiscordAcknowledgement(DiscordMutationAdapter&)>& call) {
    std::shared_ptr<DiscordMutationAdapter> adapter;
    int attempts = 1;
    std::chrono::milliseconds maximumDelay{0};
    {
        std::lock_guard<std::mutex> lock(cleanerMutex_);
        adapter = mutationAdapter_;
        attempts = maxAttempts_;
        maximumDelay = maximumRetryDelay_;
    }
    if (!adapter) {
        return makeAcknowledgement(DiscordOperationStatus::NO_ADAPTER, operation,
                                   resourceId,
                                   "remote mutation requires a DiscordMutationAdapter");
    }
    DiscordAcknowledgement result;
    for (int attempt = 1; attempt <= attempts; ++attempt) {
        try {
            result = call(*adapter);
        } catch (...) {
            result = makeAcknowledgement(DiscordOperationStatus::ADAPTER_ERROR,
                                         operation, resourceId,
                                         "mutation adapter threw an exception");
        }
        result.operation = operation;
        result.resourceId = resourceId;
        result.attempts = attempt;
        if (result.acknowledged() && result.receiptId.empty()) {
            result.status = DiscordOperationStatus::ADAPTER_ERROR;
            result.detail = "acknowledgement omitted receipt";
        }
        if (!result.retryable() || attempt == attempts) {
            return result;
        }
        const auto requested = std::max(result.retryAfter, std::chrono::milliseconds(0));
        std::this_thread::sleep_for(std::min(requested, maximumDelay));
    }
    return result;
}

ContentCleaner::CleanupResult ContentCleaner::cleanChannel(
    const std::string& channelId, const CleanupConfig& config) {
    CleanupResult result;
    if (!validDiscordId(channelId) || config.maxAge.count() < 0 ||
        config.maxDuplicateCount < 1 || config.maxDuplicateCount > 100) {
        result.status = DiscordOperationStatus::INVALID_INPUT;
        result.errors.push_back("invalid channel or cleanup configuration");
        return result;
    }
    if (std::find(config.preserveChannels.begin(), config.preserveChannels.end(),
                  channelId) != config.preserveChannels.end()) {
        result.status = DiscordOperationStatus::LOCAL_ONLY;
        return result;
    }
    MessageQuery query;
    query.channelId = channelId;
    DiscordAcknowledgement fetchResult;
    auto messages = fetchAllMessages(query, fetchResult);
    if (!fetchResult.acknowledged()) {
        result.status = fetchResult.status;
        result.errors.push_back(fetchResult.detail);
        return result;
    }
    result.messagesScanned = static_cast<int>(messages.size());
    std::set<std::string> selectedIds;
    std::map<std::string, int> normalizedCounts;
    for (const auto& message : messages) {
        const bool spam = config.deleteSpam && isSpamMessage(message);
        const bool empty = config.deleteEmpty && isEmptyMessage(message);
        const bool old = config.deleteOldMessages && isOldMessage(message, config.maxAge);
        const std::string normalized = [&message] {
            std::string value;
            for (char rawCharacter : message.content) {
                const auto character = static_cast<unsigned char>(rawCharacter);
                if (std::isspace(character) == 0) {
                    value.push_back(static_cast<char>(std::tolower(character)));
                }
            }
            return value;
        }();
        int duplicateOrdinal = 0;
        if (config.deleteDuplicates && !normalized.empty()) {
            duplicateOrdinal = ++normalizedCounts[normalized];
        }
        const bool duplicate = duplicateOrdinal > config.maxDuplicateCount;
        if (spam || empty || old || duplicate || (config.deleteBot && message.isBot)) {
            selectedIds.insert(message.id);
            if (spam) { ++result.spamRemoved; }
            if (empty) { ++result.emptyRemoved; }
            if (old) { ++result.oldRemoved; }
            if (duplicate) { ++result.duplicatesRemoved; }
        }
    }
    if (selectedIds.empty()) {
        result.status = DiscordOperationStatus::ACKNOWLEDGED;
        result.acknowledgements.push_back(fetchResult);
        return result;
    }
    std::vector<std::string> ids(selectedIds.begin(), selectedIds.end());
    const auto mutation = bulkDeleteMessagesAcknowledged(channelId, ids,
                                                          "bounded content cleanup");
    result.acknowledgements.push_back(mutation);
    result.status = mutation.status;
    if (mutation.acknowledged()) {
        result.messagesDeleted = static_cast<int>(ids.size());
        for (const auto& id : ids) {
            observeMessageDelete(channelId, id);
        }
    } else {
        result.errors.push_back(mutation.detail);
        result.duplicatesRemoved = 0;
        result.spamRemoved = 0;
        result.emptyRemoved = 0;
        result.oldRemoved = 0;
    }
    return result;
}

ContentCleaner::CleanupResult ContentCleaner::cleanGuild(
    const std::string& guildId, const CleanupConfig& config) {
    CleanupResult total;
    if (!validDiscordId(guildId)) {
        total.status = DiscordOperationStatus::INVALID_INPUT;
        total.errors.push_back("invalid guild id");
        return total;
    }
    std::shared_ptr<DiscordDataAdapter> adapter;
    {
        std::lock_guard<std::mutex> lock(cleanerMutex_);
        adapter = dataAdapter_;
    }
    if (!adapter) {
        total.status = DiscordOperationStatus::NO_ADAPTER;
        total.errors.push_back("guild cleanup requires a DiscordDataAdapter");
        return total;
    }
    std::vector<DiscordChannel> channels;
    std::set<std::string> cursors;
    std::string cursor;
    for (std::size_t index = 0; index < kMaximumPaginationPages; ++index) {
        if (!cursors.insert(cursor).second) {
            total.status = DiscordOperationStatus::ADAPTER_ERROR;
            total.errors.push_back("channel pagination cursor cycle detected");
            return total;
        }
        ChannelPage page;
        try {
            page = adapter->fetchGuildChannels(guildId, cursor,
                                               DISCRUB_MAX_PAGE_SIZE);
        } catch (...) {
            total.status = DiscordOperationStatus::ADAPTER_ERROR;
            total.errors.push_back("channel adapter threw an exception");
            return total;
        }
        if (!page.acknowledgement.acknowledged() ||
            page.acknowledgement.receiptId.empty() ||
            page.channels.size() > DISCRUB_MAX_PAGE_SIZE ||
            (page.hasMore && (page.nextCursor.empty() || page.nextCursor == cursor))) {
            total.status = page.acknowledgement.acknowledged()
                ? DiscordOperationStatus::ADAPTER_ERROR
                : page.acknowledgement.status;
            total.errors.push_back("guild channel enumeration not acknowledged");
            return total;
        }
        for (const auto& channel : page.channels) {
            if (!validDiscordId(channel.id) || channel.guildId != guildId) {
                total.status = DiscordOperationStatus::ADAPTER_ERROR;
                total.errors.push_back("malformed guild channel evidence");
                return total;
            }
            channels.push_back(channel);
        }
        if (!page.hasMore) {
            break;
        }
        cursor = page.nextCursor;
    }
    std::sort(channels.begin(), channels.end(),
              [](const DiscordChannel& left, const DiscordChannel& right) {
                  return left.id < right.id;
              });
    total.status = DiscordOperationStatus::ACKNOWLEDGED;
    for (const auto& channel : channels) {
        if (std::find(config.preserveChannels.begin(), config.preserveChannels.end(),
                      channel.id) != config.preserveChannels.end()) {
            continue;
        }
        const auto result = cleanChannel(channel.id, config);
        total.messagesScanned += result.messagesScanned;
        total.messagesDeleted += result.messagesDeleted;
        total.duplicatesRemoved += result.duplicatesRemoved;
        total.spamRemoved += result.spamRemoved;
        total.emptyRemoved += result.emptyRemoved;
        total.oldRemoved += result.oldRemoved;
        total.acknowledgements.insert(total.acknowledgements.end(),
                                      result.acknowledgements.begin(),
                                      result.acknowledgements.end());
        total.errors.insert(total.errors.end(), result.errors.begin(), result.errors.end());
        if (!result.acknowledged()) {
            total.status = result.status;
        }
    }
    return total;
}

std::vector<ContentCleaner::CleanupResult> ContentCleaner::cleanAllChannels(
    const CleanupConfig& config) {
    std::vector<std::string> channels;
    {
        std::lock_guard<std::mutex> lock(cleanerMutex_);
        for (const auto& entry : scheduledCleanups_) {
            channels.push_back(entry.first);
        }
    }
    std::sort(channels.begin(), channels.end());
    std::vector<CleanupResult> results;
    results.reserve(channels.size());
    for (const auto& channel : channels) {
        if (std::find(config.preserveChannels.begin(), config.preserveChannels.end(),
                      channel) == config.preserveChannels.end()) {
            results.push_back(cleanChannel(channel, config));
        }
    }
    return results;
}

void ContentCleaner::scheduleCleanup(const std::string& channelId,
                                     const CleanupConfig& config,
                                     const std::chrono::hours& interval) {
    if (!validDiscordId(channelId) || interval.count() <= 0 ||
        interval > std::chrono::hours(24 * 365) || config.maxAge.count() < 0 ||
        config.maxDuplicateCount < 1 || config.maxDuplicateCount > 100) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(cleanerMutex_);
        scheduledCleanups_[channelId] = config;
        cleanupIntervals_[channelId] = interval;
        nextCleanupTimes_[channelId] = std::chrono::system_clock::now() + interval;
        if (!cleanupRunning_.exchange(true)) {
            cleanupThread_ = std::thread(&ContentCleaner::cleanupLoop, this);
        }
    }
    cleanupCv_.notify_all();
}

void ContentCleaner::cancelScheduledCleanup(const std::string& channelId) {
    if (!validDiscordId(channelId)) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(cleanerMutex_);
        scheduledCleanups_.erase(channelId);
        cleanupIntervals_.erase(channelId);
        nextCleanupTimes_.erase(channelId);
    }
    cleanupCv_.notify_all();
}

std::vector<std::string> ContentCleaner::getScheduledCleanups() const {
    std::lock_guard<std::mutex> lock(cleanerMutex_);
    std::vector<std::string> result;
    for (const auto& entry : scheduledCleanups_) {
        result.push_back(entry.first);
    }
    std::sort(result.begin(), result.end());
    return result;
}

void ContentCleaner::observeMessage(const DiscordMessage& message) {
    if (!validDiscordId(message.id) || !validDiscordId(message.channelId) ||
        !validDiscordId(message.authorId) || !validOptionalDiscordId(message.guildId) ||
        !validTimestamp(message.timestamp) ||
        message.content.size() > DISCRUB_MAX_CONTENT_LENGTH) {
        return;
    }
    std::lock_guard<std::mutex> lock(cleanerMutex_);
    auto& cache = messageCache_[message.channelId];
    const auto iterator = std::find_if(cache.begin(), cache.end(),
                                       [&message](const DiscordMessage& existing) {
                                           return existing.id == message.id;
                                       });
    if (iterator == cache.end()) {
        cache.push_back(message);
    } else {
        *iterator = message;
    }
    std::sort(cache.begin(), cache.end(), messageLess);
    if (cache.size() > kMaximumCachedMessagesPerChannel) {
        cache.erase(cache.begin(),
                    cache.begin() + static_cast<std::ptrdiff_t>(
                        cache.size() - kMaximumCachedMessagesPerChannel));
    }
}

void ContentCleaner::observeMessageDelete(const std::string& channelId,
                                          const std::string& messageId) {
    if (!validDiscordId(channelId) || !validDiscordId(messageId)) {
        return;
    }
    std::lock_guard<std::mutex> lock(cleanerMutex_);
    const auto cache = messageCache_.find(channelId);
    if (cache == messageCache_.end()) {
        return;
    }
    cache->second.erase(std::remove_if(cache->second.begin(), cache->second.end(),
                                       [&messageId](const DiscordMessage& message) {
                                           return message.id == messageId;
                                       }),
                        cache->second.end());
}

std::vector<std::vector<DiscordMessage>> ContentCleaner::findDuplicateMessages(
    const std::string& channelId) {
    if (!validDiscordId(channelId)) {
        return {};
    }
    std::vector<DiscordMessage> messages;
    {
        std::lock_guard<std::mutex> lock(cleanerMutex_);
        const auto iterator = messageCache_.find(channelId);
        if (iterator != messageCache_.end()) {
            messages = iterator->second;
        }
    }
    std::vector<std::vector<DiscordMessage>> groups;
    std::vector<bool> used(messages.size(), false);
    for (std::size_t first = 0; first < messages.size(); ++first) {
        if (used[first]) {
            continue;
        }
        std::vector<DiscordMessage> group{messages[first]};
        for (std::size_t second = first + 1; second < messages.size(); ++second) {
            if (!used[second] && areDuplicates(messages[first], messages[second])) {
                group.push_back(messages[second]);
                used[second] = true;
            }
        }
        if (group.size() > 1) {
            std::sort(group.begin(), group.end(), messageLess);
            groups.push_back(std::move(group));
        }
    }
    std::sort(groups.begin(), groups.end(),
              [](const auto& left, const auto& right) {
                  return left.front().id < right.front().id;
              });
    return groups;
}

bool ContentCleaner::areDuplicates(const DiscordMessage& first,
                                   const DiscordMessage& second,
                                   double threshold) {
    if (threshold < 0.0 || threshold > 1.0) {
        return false;
    }
    return calculateMessageSimilarity(first, second) >= threshold;
}

DiscordAcknowledgement ContentCleaner::bulkDeleteMessagesAcknowledged(
    const std::string& channelId, const std::vector<std::string>& messageIds,
    const std::string& reason) {
    if (!validDiscordId(channelId) || messageIds.empty() || messageIds.size() > 100 ||
        !validReason(reason, true)) {
        return makeAcknowledgement(DiscordOperationStatus::INVALID_INPUT,
                                   "bulk_delete_messages", channelId,
                                   "invalid channel, id count, or reason");
    }
    std::set<std::string> uniqueIds;
    for (const auto& id : messageIds) {
        if (!validDiscordId(id) || !uniqueIds.insert(id).second) {
            return makeAcknowledgement(DiscordOperationStatus::INVALID_INPUT,
                                       "bulk_delete_messages", channelId,
                                       "message ids must be valid and unique");
        }
    }
    const std::vector<std::string> orderedIds(uniqueIds.begin(), uniqueIds.end());
    return invokeMutation(
        "bulk_delete_messages", channelId,
        [&](DiscordMutationAdapter& adapter) {
            return adapter.bulkDeleteMessages(channelId, orderedIds, reason);
        });
}

DiscordAcknowledgement ContentCleaner::archiveChannelAcknowledged(
    const std::string& channelId, const std::string& archivePath) {
    if (!validDiscordId(channelId) || archivePath.empty() || archivePath.size() > 4096) {
        return makeAcknowledgement(DiscordOperationStatus::INVALID_INPUT,
                                   "archive_channel", channelId,
                                   "invalid channel or path");
    }
    std::filesystem::path root;
    {
        std::lock_guard<std::mutex> lock(cleanerMutex_);
        root = storageRoot_;
    }
    std::filesystem::path resolved;
    if (!pathWithin(root, archivePath, resolved)) {
        return makeAcknowledgement(DiscordOperationStatus::INVALID_INPUT,
                                   "archive_channel", channelId,
                                   "archive path escapes configured storage root");
    }
    MessageQuery query;
    query.channelId = channelId;
    DiscordAcknowledgement fetchResult;
    const auto messages = fetchAllMessages(query, fetchResult);
    if (!fetchResult.acknowledged()) {
        fetchResult.operation = "archive_channel";
        return fetchResult;
    }
    Json document{{"schema", "elizaos.discrub.archive.v1"},
                  {"channelId", channelId},
                  {"sourceReceipt", fetchResult.receiptId},
                  {"messages", Json::array()}};
    for (const auto& message : messages) {
        document["messages"].push_back(messageToJson(message));
    }
    const std::string serialized = document.dump(2);
    if (serialized.size() > kMaximumArchiveBytes) {
        return makeAcknowledgement(DiscordOperationStatus::IO_ERROR,
                                   "archive_channel", channelId,
                                   "archive exceeds size bound");
    }
    std::error_code error;
    std::filesystem::create_directories(resolved.parent_path(), error);
    if (error) {
        return makeAcknowledgement(DiscordOperationStatus::IO_ERROR,
                                   "archive_channel", channelId,
                                   "could not create archive directory");
    }
    std::ofstream output(resolved, std::ios::binary | std::ios::trunc);
    if (!output || !(output << serialized)) {
        return makeAcknowledgement(DiscordOperationStatus::IO_ERROR,
                                   "archive_channel", channelId,
                                   "could not write archive");
    }
    auto result = makeAcknowledgement(DiscordOperationStatus::ACKNOWLEDGED,
                                      "archive_channel", channelId,
                                      "remote evidence archived locally");
    result.receiptId = fetchResult.receiptId;
    result.attempts = 1;
    return result;
}

DiscordAcknowledgement ContentCleaner::restoreFromArchiveAcknowledged(
    const std::string& channelId, const std::string& archivePath) {
    if (!validDiscordId(channelId) || archivePath.empty() || archivePath.size() > 4096) {
        return makeAcknowledgement(DiscordOperationStatus::INVALID_INPUT,
                                   "restore_archive", channelId,
                                   "invalid channel or path");
    }
    std::filesystem::path root;
    {
        std::lock_guard<std::mutex> lock(cleanerMutex_);
        root = storageRoot_;
    }
    std::filesystem::path resolved;
    if (!pathWithin(root, archivePath, resolved)) {
        return makeAcknowledgement(DiscordOperationStatus::INVALID_INPUT,
                                   "restore_archive", channelId,
                                   "archive path escapes configured storage root");
    }
    std::error_code error;
    const auto size = std::filesystem::file_size(resolved, error);
    if (error || size > kMaximumArchiveBytes) {
        return makeAcknowledgement(DiscordOperationStatus::IO_ERROR,
                                   "restore_archive", channelId,
                                   "archive is missing or exceeds size bound");
    }
    std::ifstream input(resolved, std::ios::binary);
    const std::string serialized((std::istreambuf_iterator<char>(input)),
                                 std::istreambuf_iterator<char>());
    Json document;
    try {
        document = Json::parse(serialized);
    } catch (...) {
        return makeAcknowledgement(DiscordOperationStatus::PARSE_ERROR,
                                   "restore_archive", channelId,
                                   "archive is not valid JSON");
    }
    if (document.value("schema", std::string{}) != "elizaos.discrub.archive.v1" ||
        document.value("channelId", std::string{}) != channelId ||
        !document.contains("messages") || !document["messages"].is_array() ||
        document["messages"].size() > kMaximumCachedMessagesPerChannel) {
        return makeAcknowledgement(DiscordOperationStatus::PARSE_ERROR,
                                   "restore_archive", channelId,
                                   "archive schema or channel does not match");
    }
    std::vector<DiscordMessage> messages;
    std::set<std::string> ids;
    for (const auto& item : document["messages"]) {
        DiscordMessage message;
        if (!jsonToMessage(item, message) || message.channelId != channelId ||
            !ids.insert(message.id).second) {
            return makeAcknowledgement(DiscordOperationStatus::PARSE_ERROR,
                                       "restore_archive", channelId,
                                       "archive contains malformed messages");
        }
        messages.push_back(std::move(message));
    }
    return invokeMutation(
        "restore_archive", channelId,
        [&](DiscordMutationAdapter& adapter) {
            return adapter.restoreMessages(channelId, messages);
        });
}

bool ContentCleaner::bulkDeleteMessages(
    const std::string& channelId, const std::vector<std::string>& messageIds) {
    return bulkDeleteMessagesAcknowledged(channelId, messageIds).acknowledged();
}

bool ContentCleaner::archiveChannel(const std::string& channelId,
                                    const std::string& archivePath) {
    return archiveChannelAcknowledged(channelId, archivePath).acknowledged();
}

bool ContentCleaner::restoreFromArchive(const std::string& channelId,
                                        const std::string& archivePath) {
    return restoreFromArchiveAcknowledged(channelId, archivePath).acknowledged();
}

bool ContentCleaner::isSpamMessage(const DiscordMessage& message) const {
    std::size_t run = 1;
    for (std::size_t index = 1; index < message.content.size(); ++index) {
        run = message.content[index] == message.content[index - 1] ? run + 1 : 1;
        if (run > 5 && std::isspace(static_cast<unsigned char>(message.content[index])) == 0) {
            return true;
        }
    }
    std::string lower = message.content;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char character) {
                       return static_cast<char>(std::tolower(character));
                   });
    static const std::vector<std::string> phrases{
        "free nitro", "click here to claim", "limited time offer", "win a prize"};
    return std::any_of(phrases.begin(), phrases.end(),
                       [&lower](const std::string& phrase) {
                           return lower.find(phrase) != std::string::npos;
                       });
}

bool ContentCleaner::isEmptyMessage(const DiscordMessage& message) {
    return std::all_of(message.content.begin(), message.content.end(),
                       [](unsigned char character) {
                           return std::isspace(character) != 0;
                       }) && message.attachments.empty() && message.embeds.empty();
}

bool ContentCleaner::isOldMessage(const DiscordMessage& message,
                                  const std::chrono::hours& maxAge) {
    return validTimestamp(message.timestamp) &&
           std::chrono::system_clock::now() - message.timestamp > maxAge;
}

double ContentCleaner::calculateMessageSimilarity(
    const DiscordMessage& first, const DiscordMessage& second) {
    if (first.content == second.content) {
        return 1.0;
    }
    if (first.content.empty() || second.content.empty()) {
        return 0.0;
    }
    const auto trigrams = [](const std::string& content) {
        std::set<std::string> values;
        if (content.size() < 3) {
            values.insert(content);
        } else {
            for (std::size_t index = 0; index + 3 <= content.size(); ++index) {
                values.insert(content.substr(index, 3));
            }
        }
        return values;
    };
    const auto left = trigrams(first.content);
    const auto right = trigrams(second.content);
    std::vector<std::string> intersection;
    std::set_intersection(left.begin(), left.end(), right.begin(), right.end(),
                          std::back_inserter(intersection));
    const std::size_t unionSize = left.size() + right.size() - intersection.size();
    return unionSize == 0 ? 1.0
                          : static_cast<double>(intersection.size()) /
                                static_cast<double>(unionSize);
}

void ContentCleaner::cleanupLoop() {
    std::unique_lock<std::mutex> lock(cleanerMutex_);
    while (cleanupRunning_.load()) {
        if (nextCleanupTimes_.empty()) {
            cleanupCv_.wait(lock, [this] {
                return !cleanupRunning_.load() || !nextCleanupTimes_.empty();
            });
            continue;
        }
        const auto next = std::min_element(
            nextCleanupTimes_.begin(), nextCleanupTimes_.end(),
            [](const auto& left, const auto& right) {
                return left.second < right.second;
            });
        const auto wake = next->second;
        cleanupCv_.wait_until(lock, wake);
        if (!cleanupRunning_.load()) {
            break;
        }
        const auto now = std::chrono::system_clock::now();
        std::vector<std::pair<std::string, CleanupConfig>> due;
        for (auto& entry : nextCleanupTimes_) {
            if (entry.second <= now) {
                const auto config = scheduledCleanups_.find(entry.first);
                const auto interval = cleanupIntervals_.find(entry.first);
                if (config != scheduledCleanups_.end() &&
                    interval != cleanupIntervals_.end()) {
                    due.emplace_back(entry.first, config->second);
                    entry.second = now + interval->second;
                }
            }
        }
        lock.unlock();
        for (const auto& item : due) {
            try {
                cleanChannel(item.first, item.second);
            } catch (...) {
                // A scheduled run is isolated; the owned worker remains live.
            }
        }
        lock.lock();
    }
}

ModerationAnalytics::ModerationAnalytics() = default;
ModerationAnalytics::~ModerationAnalytics() = default;

void ModerationAnalytics::recordAction(const ModerationAction& action) {
    if (!action.acknowledgement.acknowledged() || action.id.empty() ||
        action.id.size() > 64 || !validTimestamp(action.timestamp) ||
        !validOptionalDiscordId(action.guildId) ||
        !validOptionalDiscordId(action.userId) ||
        !validOptionalDiscordId(action.channelId) ||
        !validOptionalDiscordId(action.messageId) || !validReason(action.reason, true)) {
        return;
    }
    std::lock_guard<std::mutex> lock(analyticsMutex_);
    const auto existing = std::find_if(actions_.begin(), actions_.end(),
                                       [&action](const ModerationAction& value) {
                                           return value.id == action.id;
                                       });
    if (existing == actions_.end()) {
        actions_.push_back(action);
        std::sort(actions_.begin(), actions_.end(),
                  [](const ModerationAction& left, const ModerationAction& right) {
                      return left.timestamp == right.timestamp ? left.id < right.id
                                                              : left.timestamp < right.timestamp;
                  });
    }
}

bool ModerationAnalytics::setStorageRoot(const std::string& rootPath) {
    if (rootPath.empty() || rootPath.size() > 4096) {
        return false;
    }
    std::error_code error;
    std::filesystem::create_directories(rootPath, error);
    const auto root = std::filesystem::weakly_canonical(rootPath, error);
    if (error || !std::filesystem::is_directory(root, error)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(analyticsMutex_);
    storageRoot_ = root;
    return true;
}

std::vector<ModerationAction> ModerationAnalytics::getActionsInPeriod(
    const std::chrono::system_clock::time_point& start,
    const std::chrono::system_clock::time_point& end) const {
    std::vector<ModerationAction> result;
    for (const auto& action : actions_) {
        if (action.timestamp >= start && action.timestamp <= end) {
            result.push_back(action);
        }
    }
    return result;
}

ModerationAnalytics::ModerationReport ModerationAnalytics::generateReport(
    const std::chrono::system_clock::time_point& startTime,
    const std::chrono::system_clock::time_point& endTime) {
    ModerationReport report;
    report.periodStart = startTime;
    report.periodEnd = endTime;
    if (startTime > endTime || endTime > std::chrono::system_clock::now() +
                                             std::chrono::minutes(5)) {
        return report;
    }
    std::lock_guard<std::mutex> lock(analyticsMutex_);
    const auto actions = getActionsInPeriod(startTime, endTime);
    for (const auto& action : actions) {
        ++report.totalActions;
        switch (action.action) {
            case FilterAction::WARN: ++report.warningsIssued; break;
            case FilterAction::DELETE: ++report.messagesDeleted; break;
            case FilterAction::TIMEOUT: ++report.timeoutsIssued; break;
            case FilterAction::KICK: ++report.kicksIssued; break;
            case FilterAction::BAN: ++report.bansIssued; break;
            case FilterAction::NONE: break;
        }
        if (!action.channelId.empty()) {
            ++report.violationsByChannel[action.channelId];
        }
    }
    report.topViolators = findTopViolators(actions, 5);
    report.commonViolations = findCommonViolations(actions, 5);
    return report;
}

ModerationAnalytics::ModerationReport ModerationAnalytics::generateDailyReport() {
    const auto now = std::chrono::system_clock::now();
    return generateReport(now - std::chrono::hours(24), now);
}

ModerationAnalytics::ModerationReport ModerationAnalytics::generateWeeklyReport() {
    const auto now = std::chrono::system_clock::now();
    return generateReport(now - std::chrono::hours(24 * 7), now);
}

ModerationAnalytics::ModerationReport ModerationAnalytics::generateMonthlyReport() {
    const auto now = std::chrono::system_clock::now();
    return generateReport(now - std::chrono::hours(24 * 30), now);
}

std::vector<double> ModerationAnalytics::getViolationTrends(int days) {
    if (days < 1 || days > 366) {
        return {};
    }
    const auto now = std::chrono::system_clock::now();
    std::vector<double> trends;
    trends.reserve(static_cast<std::size_t>(days));
    for (int offset = days; offset > 0; --offset) {
        const auto report = generateReport(now - std::chrono::hours(24 * offset),
                                           now - std::chrono::hours(24 * (offset - 1)));
        trends.push_back(static_cast<double>(report.totalActions));
    }
    return trends;
}

std::vector<std::string> ModerationAnalytics::getTopViolationTypes(int limit) {
    if (limit < 1 || limit > 100) {
        return {};
    }
    const auto now = std::chrono::system_clock::now();
    std::lock_guard<std::mutex> lock(analyticsMutex_);
    return findCommonViolations(
        getActionsInPeriod(now - std::chrono::hours(24 * 30), now), limit);
}

std::unordered_map<std::string, double>
ModerationAnalytics::getChannelRiskScores() {
    const auto now = std::chrono::system_clock::now();
    std::lock_guard<std::mutex> lock(analyticsMutex_);
    const auto actions = getActionsInPeriod(now - std::chrono::hours(24 * 7), now);
    std::unordered_map<std::string, double> scores;
    for (const auto& action : actions) {
        if (!action.channelId.empty()) {
            scores[action.channelId] += 1.0;
        }
    }
    if (!actions.empty()) {
        for (auto& score : scores) {
            score.second /= static_cast<double>(actions.size());
        }
    }
    return scores;
}

std::vector<std::string> ModerationAnalytics::findTopViolators(
    const std::vector<ModerationAction>& actions, int limit) {
    std::map<std::string, int> counts;
    for (const auto& action : actions) {
        if (!action.userId.empty()) {
            ++counts[action.userId];
        }
    }
    std::vector<std::pair<std::string, int>> ordered(counts.begin(), counts.end());
    std::sort(ordered.begin(), ordered.end(), [](const auto& left, const auto& right) {
        return left.second == right.second ? left.first < right.first
                                          : left.second > right.second;
    });
    std::vector<std::string> result;
    for (std::size_t index = 0;
         index < ordered.size() && index < static_cast<std::size_t>(std::max(0, limit));
         ++index) {
        result.push_back(ordered[index].first);
    }
    return result;
}

std::vector<std::string> ModerationAnalytics::findCommonViolations(
    const std::vector<ModerationAction>& actions, int limit) {
    std::map<std::string, int> counts;
    for (const auto& action : actions) {
        if (!action.reason.empty()) {
            ++counts[action.reason];
        }
    }
    std::vector<std::pair<std::string, int>> ordered(counts.begin(), counts.end());
    std::sort(ordered.begin(), ordered.end(), [](const auto& left, const auto& right) {
        return left.second == right.second ? left.first < right.first
                                          : left.second > right.second;
    });
    std::vector<std::string> result;
    for (std::size_t index = 0;
         index < ordered.size() && index < static_cast<std::size_t>(std::max(0, limit));
         ++index) {
        result.push_back(ordered[index].first);
    }
    return result;
}

std::string ModerationAnalytics::exportReportAsJson(
    const ModerationReport& report) {
    std::map<std::string, int> orderedChannels(report.violationsByChannel.begin(),
                                               report.violationsByChannel.end());
    Json document{{"schema", "elizaos.discrub.report.v1"},
                  {"periodStartMs", toMilliseconds(report.periodStart)},
                  {"periodEndMs", toMilliseconds(report.periodEnd)},
                  {"totalActions", report.totalActions},
                  {"warningsIssued", report.warningsIssued},
                  {"timeoutsIssued", report.timeoutsIssued},
                  {"kicksIssued", report.kicksIssued},
                  {"bansIssued", report.bansIssued},
                  {"messagesDeleted", report.messagesDeleted},
                  {"topViolators", report.topViolators},
                  {"commonViolations", report.commonViolations},
                  {"violationsByChannel", orderedChannels},
                  {"averageResponseTime", report.averageResponseTime}};
    return document.dump(2);
}

std::string ModerationAnalytics::exportReportAsHtml(
    const ModerationReport& report) {
    std::map<std::string, int> orderedChannels(report.violationsByChannel.begin(),
                                               report.violationsByChannel.end());
    std::ostringstream output;
    output << "<!doctype html><html><head><meta charset=\"utf-8\"><title>Moderation "
              "Report</title></head><body><h1>Moderation Report</h1><dl>"
           << "<dt>Total actions</dt><dd>" << report.totalActions << "</dd>"
           << "<dt>Warnings</dt><dd>" << report.warningsIssued << "</dd>"
           << "<dt>Timeouts</dt><dd>" << report.timeoutsIssued << "</dd>"
           << "<dt>Kicks</dt><dd>" << report.kicksIssued << "</dd>"
           << "<dt>Bans</dt><dd>" << report.bansIssued << "</dd>"
           << "<dt>Messages deleted</dt><dd>" << report.messagesDeleted
           << "</dd></dl><ul>";
    for (const auto& entry : orderedChannels) {
        output << "<li>" << escapeHtml(entry.first) << ": " << entry.second << "</li>";
    }
    output << "</ul><h2>Common violations</h2><ul>";
    for (const auto& reason : report.commonViolations) {
        output << "<li>" << escapeHtml(reason) << "</li>";
    }
    output << "</ul></body></html>";
    return output.str();
}

DiscordAcknowledgement ModerationAnalytics::exportReportToFileAcknowledged(
    const ModerationReport& report, const std::string& filePath) {
    if (filePath.empty() || filePath.size() > 4096 ||
        report.periodStart > report.periodEnd) {
        return makeAcknowledgement(DiscordOperationStatus::INVALID_INPUT,
                                   "export_report", filePath,
                                   "invalid report period or path");
    }
    std::filesystem::path root;
    {
        std::lock_guard<std::mutex> lock(analyticsMutex_);
        root = storageRoot_;
    }
    std::filesystem::path resolved;
    if (!pathWithin(root, filePath, resolved)) {
        return makeAcknowledgement(DiscordOperationStatus::INVALID_INPUT,
                                   "export_report", filePath,
                                   "report path escapes configured storage root");
    }
    const bool html = resolved.extension() == ".html";
    const bool json = resolved.extension() == ".json";
    if (!html && !json) {
        return makeAcknowledgement(DiscordOperationStatus::INVALID_INPUT,
                                   "export_report", filePath,
                                   "report extension must be .json or .html");
    }
    const std::string content = html ? exportReportAsHtml(report)
                                     : exportReportAsJson(report);
    std::error_code error;
    std::filesystem::create_directories(resolved.parent_path(), error);
    if (error) {
        return makeAcknowledgement(DiscordOperationStatus::IO_ERROR,
                                   "export_report", filePath,
                                   "could not create report directory");
    }
    std::ofstream output(resolved, std::ios::binary | std::ios::trunc);
    if (!output || !(output << content)) {
        return makeAcknowledgement(DiscordOperationStatus::IO_ERROR,
                                   "export_report", filePath,
                                   "could not write report");
    }
    auto result = makeAcknowledgement(DiscordOperationStatus::ACKNOWLEDGED,
                                      "export_report", filePath,
                                      "local report export completed");
    result.receiptId = "local-file:" + resolved.filename().string();
    result.attempts = 1;
    return result;
}

bool ModerationAnalytics::exportReportToFile(
    const ModerationReport& report, const std::string& filePath) {
    return exportReportToFileAcknowledged(report, filePath).acknowledged();
}

DiscrubExtension::DiscrubExtension() {
    moderator_.setActionObserver([this](const ModerationAction& action) {
        analytics_.recordAction(action);
        std::function<void(const ModerationAction&)> handler;
        {
            std::lock_guard<std::mutex> lock(configMutex_);
            handler = actionHandler_;
        }
        if (handler) {
            try {
                handler(action);
            } catch (...) {
                // Handler exceptions are isolated from acknowledged moderation.
            }
        }
    });
}

DiscrubExtension::~DiscrubExtension() {
    stopMonitoring();
    moderator_.setActionObserver({});
}

bool DiscrubExtension::initializeWithDiscord(std::shared_ptr<DiscordClient> client) {
    std::lock_guard<std::mutex> lock(configMutex_);
    discordClient_ = std::move(client);
    // A legacy DiscordClient has untyped bool mutations and therefore cannot be
    // treated as an evidence-bearing adapter.
    return discordClient_ != nullptr;
}

bool DiscrubExtension::initializeWithAdapters(
    std::shared_ptr<DiscordDataAdapter> dataAdapter,
    std::shared_ptr<DiscordMutationAdapter> mutationAdapter) {
    {
        std::lock_guard<std::mutex> lock(configMutex_);
        dataAdapter_ = dataAdapter;
        mutationAdapter_ = mutationAdapter;
    }
    cleaner_.setAdapters(std::move(dataAdapter), mutationAdapter);
    moderator_.setMutationAdapter(std::move(mutationAdapter));
    return dataAdapter_ != nullptr || mutationAdapter_ != nullptr;
}

void DiscrubExtension::startMonitoring(const std::vector<std::string>& channelIds) {
    if (channelIds.empty() || channelIds.size() > 1000 ||
        std::any_of(channelIds.begin(), channelIds.end(),
                    [](const std::string& id) { return !validDiscordId(id); })) {
        return;
    }
    std::vector<std::string> ordered = channelIds;
    std::sort(ordered.begin(), ordered.end());
    ordered.erase(std::unique(ordered.begin(), ordered.end()), ordered.end());
    std::lock_guard<std::mutex> lock(configMutex_);
    monitoredChannels_ = std::move(ordered);
    monitoring_.store(true);
}

void DiscrubExtension::stopMonitoring() {
    monitoring_.store(false);
    std::lock_guard<std::mutex> lock(configMutex_);
    monitoredChannels_.clear();
}

bool DiscrubExtension::isMonitoring() const {
    return monitoring_.load();
}

AutoModerator::ModerationResult
DiscrubExtension::processIncomingMessageAcknowledged(
    const DiscordMessage& message) {
    cleaner_.observeMessage(message);
    auto result = moderator_.processMessageAcknowledged(message);
    if (result.classification.validInput && result.classification.violation) {
        std::function<void(const DiscordMessage&, const ContentScanner::ScanResult&)> handler;
        {
            std::lock_guard<std::mutex> lock(configMutex_);
            handler = violationHandler_;
        }
        if (handler) {
            try {
                handler(message, result.classification);
            } catch (...) {
                // Classification evidence remains valid if a user callback fails.
            }
        }
    }
    return result;
}

void DiscrubExtension::processIncomingMessage(const DiscordMessage& message) {
    (void)processIncomingMessageAcknowledged(message);
}

void DiscrubExtension::processMessageEdit(const DiscordMessage& oldMessage,
                                          const DiscordMessage& newMessage) {
    if (oldMessage.id != newMessage.id || oldMessage.channelId != newMessage.channelId ||
        oldMessage.authorId != newMessage.authorId ||
        newMessage.timestamp < oldMessage.timestamp) {
        return;
    }
    cleaner_.observeMessage(newMessage);
    const auto result = moderator_.processEditAcknowledged(oldMessage, newMessage);
    if (result.classification.validInput && result.classification.violation) {
        std::function<void(const DiscordMessage&, const ContentScanner::ScanResult&)> handler;
        {
            std::lock_guard<std::mutex> lock(configMutex_);
            handler = violationHandler_;
        }
        if (handler) {
            try {
                handler(newMessage, result.classification);
            } catch (...) {
            }
        }
    }
}

void DiscrubExtension::processMessageDelete(const std::string& channelId,
                                            const std::string& messageId) {
    cleaner_.observeMessageDelete(channelId, messageId);
}

std::future<ContentCleaner::CleanupResult>
DiscrubExtension::scheduleBatchCleanup(const std::string& channelId,
                                       const CleanupConfig& config) {
    return std::async(std::launch::deferred, [this, channelId, config] {
        const auto result = cleaner_.cleanChannel(channelId, config);
        std::function<void(const ContentCleaner::CleanupResult&)> handler;
        {
            std::lock_guard<std::mutex> lock(configMutex_);
            handler = cleanupHandler_;
        }
        if (handler) {
            try {
                handler(result);
            } catch (...) {
            }
        }
        return result;
    });
}

std::future<ModerationAnalytics::ModerationReport>
DiscrubExtension::generateReport(
    const std::chrono::system_clock::time_point& startTime,
    const std::chrono::system_clock::time_point& endTime) {
    return std::async(std::launch::deferred, [this, startTime, endTime] {
        return analytics_.generateReport(startTime, endTime);
    });
}

bool DiscrubExtension::setStorageRoot(const std::string& rootPath) {
    if (!cleaner_.setStorageRoot(rootPath) || !analytics_.setStorageRoot(rootPath)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(configMutex_);
    storageRoot_ = std::filesystem::weakly_canonical(rootPath);
    return true;
}

DiscordAcknowledgement DiscrubExtension::loadConfigurationAcknowledged(
    const std::string& configPath) {
    std::filesystem::path root;
    {
        std::lock_guard<std::mutex> lock(configMutex_);
        root = storageRoot_;
    }
    std::filesystem::path resolved;
    if (!pathWithin(root, configPath, resolved)) {
        return makeAcknowledgement(DiscordOperationStatus::INVALID_INPUT,
                                   "load_configuration", configPath,
                                   "configuration path escapes storage root");
    }
    std::error_code error;
    if (std::filesystem::file_size(resolved, error) > 64U * 1024U || error) {
        return makeAcknowledgement(DiscordOperationStatus::IO_ERROR,
                                   "load_configuration", configPath,
                                   "configuration is missing or too large");
    }
    std::ifstream input(resolved, std::ios::binary);
    const std::string serialized((std::istreambuf_iterator<char>(input)),
                                 std::istreambuf_iterator<char>());
    Json document;
    try {
        document = Json::parse(serialized);
    } catch (...) {
        return makeAcknowledgement(DiscordOperationStatus::PARSE_ERROR,
                                   "load_configuration", configPath,
                                   "configuration is not valid JSON");
    }
    if (document.value("schema", std::string{}) != "elizaos.discrub.config.v1" ||
        !document.contains("strictMode") || !document["strictMode"].is_boolean() ||
        !document.contains("autoEscalation") ||
        !document["autoEscalation"].is_boolean() ||
        !document.contains("reputationThreshold") ||
        !document["reputationThreshold"].is_number_integer() ||
        !document.contains("actionCooldownSeconds") ||
        !document["actionCooldownSeconds"].is_number_integer()) {
        return makeAcknowledgement(DiscordOperationStatus::PARSE_ERROR,
                                   "load_configuration", configPath,
                                   "configuration schema is invalid");
    }
    const int threshold = document["reputationThreshold"].get<int>();
    const int cooldown = document["actionCooldownSeconds"].get<int>();
    if (threshold < -1000 || threshold > 1000 || cooldown < 0 || cooldown > 86400) {
        return makeAcknowledgement(DiscordOperationStatus::PARSE_ERROR,
                                   "load_configuration", configPath,
                                   "configuration values are outside bounds");
    }
    moderator_.setStrictMode(document["strictMode"].get<bool>());
    moderator_.setAutoEscalation(document["autoEscalation"].get<bool>());
    moderator_.setReputationThreshold(threshold);
    moderator_.setActionCooldown(cooldown);
    {
        std::lock_guard<std::mutex> lock(configMutex_);
        config_["strictMode"] = document["strictMode"].get<bool>() ? "true" : "false";
        config_["autoEscalation"] =
            document["autoEscalation"].get<bool>() ? "true" : "false";
        config_["reputationThreshold"] = std::to_string(threshold);
        config_["actionCooldownSeconds"] = std::to_string(cooldown);
    }
    auto result = makeAcknowledgement(DiscordOperationStatus::ACKNOWLEDGED,
                                      "load_configuration", configPath,
                                      "local configuration loaded");
    result.receiptId = "local-file:" + resolved.filename().string();
    result.attempts = 1;
    return result;
}

DiscordAcknowledgement DiscrubExtension::saveConfigurationAcknowledged(
    const std::string& configPath) {
    std::filesystem::path root;
    std::unordered_map<std::string, std::string> config;
    {
        std::lock_guard<std::mutex> lock(configMutex_);
        root = storageRoot_;
        config = config_;
    }
    std::filesystem::path resolved;
    if (!pathWithin(root, configPath, resolved)) {
        return makeAcknowledgement(DiscordOperationStatus::INVALID_INPUT,
                                   "save_configuration", configPath,
                                   "configuration path escapes storage root");
    }
    const auto booleanValue = [&config](const std::string& key, bool fallback) {
        const auto iterator = config.find(key);
        return iterator == config.end() ? fallback : iterator->second == "true";
    };
    const auto integerValue = [&config](const std::string& key, int fallback) {
        const auto iterator = config.find(key);
        if (iterator == config.end()) {
            return fallback;
        }
        try {
            return std::stoi(iterator->second);
        } catch (...) {
            return fallback;
        }
    };
    Json document{{"schema", "elizaos.discrub.config.v1"},
                  {"strictMode", booleanValue("strictMode", false)},
                  {"autoEscalation", booleanValue("autoEscalation", true)},
                  {"reputationThreshold", integerValue("reputationThreshold", 50)},
                  {"actionCooldownSeconds", integerValue("actionCooldownSeconds", 300)}};
    std::error_code error;
    std::filesystem::create_directories(resolved.parent_path(), error);
    std::ofstream output(resolved, std::ios::binary | std::ios::trunc);
    if (error || !output || !(output << document.dump(2))) {
        return makeAcknowledgement(DiscordOperationStatus::IO_ERROR,
                                   "save_configuration", configPath,
                                   "could not save configuration");
    }
    auto result = makeAcknowledgement(DiscordOperationStatus::ACKNOWLEDGED,
                                      "save_configuration", configPath,
                                      "local configuration saved");
    result.receiptId = "local-file:" + resolved.filename().string();
    result.attempts = 1;
    return result;
}

void DiscrubExtension::loadConfiguration(const std::string& configPath) {
    (void)loadConfigurationAcknowledged(configPath);
}

void DiscrubExtension::saveConfiguration(const std::string& configPath) {
    (void)saveConfigurationAcknowledged(configPath);
}

void DiscrubExtension::setDefaultModerationSettings() {
    moderator_.getScanner().enableProfanityFilter(true);
    moderator_.getScanner().enableSpamFilter(true);
    moderator_.getScanner().enablePhishingFilter(true);
    moderator_.getScanner().enableInviteFilter(true);
    moderator_.getScanner().enableMentionSpamFilter(true, 5);
    moderator_.setStrictMode
(false);
    moderator_.setAutoEscalation(true);
    moderator_.setReputationThreshold(50);
    moderator_.setActionCooldown(300);
    std::lock_guard<std::mutex> lock(configMutex_);
    config_["strictMode"] = "false";
    config_["autoEscalation"] = "true";
    config_["reputationThreshold"] = "50";
    config_["actionCooldownSeconds"] = "300";
}

void DiscrubExtension::setViolationHandler(
    std::function<void(const DiscordMessage&, const ContentScanner::ScanResult&)> handler) {
    std::lock_guard<std::mutex> lock(configMutex_);
    violationHandler_ = std::move(handler);
}

void DiscrubExtension::setActionHandler(
    std::function<void(const ModerationAction&)> handler) {
    std::lock_guard<std::mutex> lock(configMutex_);
    actionHandler_ = std::move(handler);
}

void DiscrubExtension::setCleanupHandler(
    std::function<void(const ContentCleaner::CleanupResult&)> handler) {
    std::lock_guard<std::mutex> lock(configMutex_);
    cleanupHandler_ = std::move(handler);
}

void DiscrubExtension::handleViolation(
    const DiscordMessage& message, const ContentScanner::ScanResult& result) {
    if (!result.validInput || !result.violation) {
        return;
    }
    std::function<void(const DiscordMessage&, const ContentScanner::ScanResult&)> handler;
    {
        std::lock_guard<std::mutex> lock(configMutex_);
        handler = violationHandler_;
    }
    if (handler) {
        try {
            handler(message, result);
        } catch (...) {
        }
    }
}

std::shared_ptr<DiscrubExtension> globalDiscrubExtension =
    std::make_shared<DiscrubExtension>();

} // namespace elizaos
