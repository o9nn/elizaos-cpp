#pragma once

#include "elizaos/discord_summarizer.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <regex>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

constexpr std::size_t DISCRUB_MAX_CONTENT_LENGTH = 4000;
constexpr std::size_t DISCRUB_MAX_REASON_LENGTH = 512;
constexpr std::size_t DISCRUB_MAX_PAGE_SIZE = 100;

enum class FilterAction { NONE, WARN, DELETE, TIMEOUT, KICK, BAN };

enum class DiscordOperationStatus {
    ACKNOWLEDGED,
    LOCAL_ONLY,
    INVALID_INPUT,
    NO_ADAPTER,
    PERMISSION_DENIED,
    RATE_LIMITED,
    NOT_FOUND,
    REMOTE_REJECTED,
    IO_ERROR,
    PARSE_ERROR,
    ADAPTER_ERROR
};

struct DiscordAcknowledgement {
    DiscordOperationStatus status{DiscordOperationStatus::INVALID_INPUT};
    std::string operation;
    std::string resourceId;
    std::string receiptId;
    std::string detail;
    std::chrono::milliseconds retryAfter{0};
    int attempts{0};
    bool acknowledged() const noexcept {
        return status == DiscordOperationStatus::ACKNOWLEDGED;
    }
    bool retryable() const noexcept {
        return status == DiscordOperationStatus::RATE_LIMITED ||
               status == DiscordOperationStatus::ADAPTER_ERROR;
    }
};

struct MessageQuery {
    std::string guildId;
    std::string channelId;
    std::string searchText;
    std::string cursor;
    std::size_t limit{50};
    std::optional<std::chrono::system_clock::time_point> after;
    std::optional<std::chrono::system_clock::time_point> before;
    bool includeBots{true};
};

struct MessagePage {
    DiscordAcknowledgement acknowledgement;
    std::vector<DiscordMessage> messages;
    std::string nextCursor;
    bool hasMore{false};
};

struct ChannelPage {
    DiscordAcknowledgement acknowledgement;
    std::vector<DiscordChannel> channels;
    std::string nextCursor;
    bool hasMore{false};
};

class DiscordDataAdapter {
public:
    virtual ~DiscordDataAdapter() = default;
    virtual MessagePage fetchMessages(const MessageQuery& query) = 0;
    virtual MessagePage searchMessages(const MessageQuery& query) = 0;
    virtual ChannelPage fetchGuildChannels(const std::string& guildId,
                                           const std::string& cursor,
                                           std::size_t limit) = 0;
};

class DiscordMutationAdapter {
public:
    virtual ~DiscordMutationAdapter() = default;
    virtual DiscordAcknowledgement warnUser(const std::string& guildId,
                                             const std::string& userId,
                                             const std::string& channelId,
                                             const std::string& reason) = 0;
    virtual DiscordAcknowledgement timeoutUser(const std::string& guildId,
                                                const std::string& userId,
                                                int minutes,
                                                const std::string& reason) = 0;
    virtual DiscordAcknowledgement kickUser(const std::string& guildId,
                                             const std::string& userId,
                                             const std::string& reason) = 0;
    virtual DiscordAcknowledgement banUser(const std::string& guildId,
                                            const std::string& userId,
                                            const std::string& reason,
                                            int deleteMessageDays) = 0;
    virtual DiscordAcknowledgement deleteMessage(const std::string& channelId,
                                                   const std::string& messageId,
                                                   const std::string& reason) = 0;
    virtual DiscordAcknowledgement bulkDeleteMessages(
        const std::string& channelId,
        const std::vector<std::string>& messageIds,
        const std::string& reason) = 0;
    virtual DiscordAcknowledgement restoreMessages(
        const std::string& channelId,
        const std::vector<DiscordMessage>& messages) = 0;
};

struct ContentFilter {
    std::string name;
    std::string description;
    std::regex pattern;
    FilterAction action{FilterAction::NONE};
    int severity{1};
    bool enabled{true};
    std::string reason;
    ContentFilter() = default;
    ContentFilter(const std::string& filterName, const std::string& patternString,
                  FilterAction filterAction, int filterSeverity = 1)
        : name(filterName), pattern(patternString), action(filterAction),
          severity(filterSeverity) {}
};

struct ModerationAction {
    std::string id;
    std::string guildId;
    std::string userId;
    std::string moderatorId;
    std::string channelId;
    std::string messageId;
    FilterAction action{FilterAction::NONE};
    std::string reason;
    std::chrono::system_clock::time_point timestamp{};
    bool appealed{false};
    bool appealReviewed{false};
    bool appealApproved{false};
    std::string appealReason;
    DiscordAcknowledgement acknowledgement;
};

struct UserReputation {
    std::string userId;
    int reputationScore{100};
    int warningCount{0};
    int timeoutCount{0};
    int kickCount{0};
    int banCount{0};
    std::chrono::system_clock::time_point lastIncident{};
    std::vector<std::string> violations;
    bool isTrusted{false};
    UserReputation() = default;
    explicit UserReputation(const std::string& userIdentifier) : userId(userIdentifier) {}
};

struct CleanupConfig {
    bool deleteSpam{true};
    bool deleteBot{false};
    bool deleteDuplicates{true};
    bool deleteEmpty{true};
    bool deleteOldMessages{false};
    std::chrono::hours maxAge{24 * 30};
    int maxDuplicateCount{3};
    std::vector<std::string> preserveChannels;
};

class ContentScanner {
public:
    struct ScanResult {
        bool validInput{true};
        bool violation{false};
        std::vector<std::string> triggeredFilters;
        FilterAction recommendedAction{FilterAction::NONE};
        int totalSeverity{0};
        std::string reason;
        std::vector<std::string> evidence;
    };
    ContentScanner();
    ~ContentScanner();
    void addFilter(const ContentFilter& filter);
    void removeFilter(const std::string& name);
    void updateFilter(const std::string& name, const ContentFilter& filter);
    std::vector<ContentFilter> getFilters() const;
    ScanResult scanMessage(const DiscordMessage& message);
    ScanResult scanContent(const std::string& content);
    std::vector<ScanResult> scanMessages(const std::vector<DiscordMessage>& messages);
    void enableProfanityFilter(bool enable = true);
    void enableSpamFilter(bool enable = true);
    void enablePhishingFilter(bool enable = true);
    void enableInviteFilter(bool enable = true);
    void enableMentionSpamFilter(bool enable = true, int maxMentions = 5);
    void addProfanityWords(const std::vector<std::string>& words);
    void addAllowedDomains(const std::vector<std::string>& domains);
    void addBlockedDomains(const std::vector<std::string>& domains);

private:
    std::vector<ContentFilter> filters_;
    std::unordered_set<std::string> profanityWords_;
    std::unordered_set<std::string> allowedDomains_;
    std::unordered_set<std::string> blockedDomains_;
    bool profanityFilterEnabled_{true};
    bool spamFilterEnabled_{true};
    bool phishingFilterEnabled_{true};
    bool inviteFilterEnabled_{true};
    bool mentionSpamEnabled_{true};
    int maxMentions_{5};
    mutable std::mutex scannerMutex_;
    ScanResult scanLocked(const std::string& content,
                          const DiscordMessage* message) const;
    bool detectProfanityLocked(const std::string& content) const;
    bool detectSpamLocked(const DiscordMessage& message) const;
    bool detectPhishingLocked(const std::string& content) const;
    bool detectInviteLinksLocked(const std::string& content) const;
    bool detectMentionSpamLocked(const DiscordMessage& message) const;
    static std::vector<std::string> extractUrls(const std::string& content);
    static int countMentions(const std::string& content);
};

class AutoModerator {
public:
    struct ModerationResult {
        ContentScanner::ScanResult classification;
        std::optional<ModerationAction> action;
        DiscordAcknowledgement acknowledgement;
    };
    using Clock = std::function<std::chrono::system_clock::time_point()>;
    using ActionObserver = std::function<void(const ModerationAction&)>;
    AutoModerator();
    explicit AutoModerator(std::shared_ptr<DiscordMutationAdapter> adapter);
    ~AutoModerator();
    void setMutationAdapter(std::shared_ptr<DiscordMutationAdapter> adapter);
    void setRetryPolicy(int maxAttempts, std::chrono::milliseconds maximumDelay);
    void setClock(Clock clock);
    void setActionObserver(ActionObserver observer);
    ModerationResult processMessageAcknowledged(const DiscordMessage& message);
    ModerationResult processEditAcknowledged(const DiscordMessage& oldMessage,
                                              const DiscordMessage& newMessage);
    bool processMessage(const DiscordMessage& message);
    bool processEdit(const DiscordMessage& oldMessage, const DiscordMessage& newMessage);
    bool reviewUser(const std::string& userId);
    ContentScanner& getScanner() { return scanner_; }
    DiscordAcknowledgement executeActionAcknowledged(const ModerationAction& action);
    DiscordAcknowledgement warnUserAcknowledged(const std::string& guildId,
                                                 const std::string& userId,
                                                 const std::string& reason,
                                                 const std::string& channelId = "");
    DiscordAcknowledgement timeoutUserAcknowledged(const std::string& guildId,
                                                    const std::string& userId,
                                                    int minutes,
                                                    const std::string& reason);
    DiscordAcknowledgement kickUserAcknowledged(const std::string& guildId,
                                                 const std::string& userId,
                                                 const std::string& reason);
    DiscordAcknowledgement banUserAcknowledged(const std::string& guildId,
                                                const std::string& userId,
                                                const std::string& reason,
                                                int deleteMessageDays = 0);
    DiscordAcknowledgement deleteMessageAcknowledged(const std::string& channelId,
                                                       const std::string& messageId,
                                                       const std::string& reason = "");
    bool executeAction(const ModerationAction& action);
    bool warnUser(const std::string& userId, const std::string& reason,
                  const std::string& channelId = "");
    bool timeoutUser(const std::string& userId, int minutes, const std::string& reason);
    bool kickUser(const std::string& userId, const std::string& reason);
    bool banUser(const std::string& userId, const std::string& reason,
                 int deleteMessageDays = 0);
    bool deleteMessage(const std::string& channelId, const std::string& messageId,
                       const std::string& reason = "");
    void setGuildContext(const std::string& guildId);
    void updateUserReputation(const std::string& userId, int change,
                              const std::string& reason);
    UserReputation getUserReputation(const std::string& userId);
    void setTrustedUser(const std::string& userId, bool trusted);
    void setStrictMode(bool strict);
    void setAutoEscalation(bool enable);
    void setReputationThreshold(int threshold);
    void setActionCooldown(int seconds);
    std::vector<ModerationAction> getUserActions(const std::string& userId);
    std::vector<ModerationAction> getChannelActions(const std::string& channelId);
    std::vector<ModerationAction> getRecentActions(int hours = 24);
    bool submitAppeal(const std::string& actionId, const std::string& reason);
    bool reviewAppeal(const std::string& actionId, bool approved,
                      const std::string& moderatorId);
    std::vector<ModerationAction> getPendingAppeals();

private:
    ContentScanner scanner_;
    std::unordered_map<std::string, UserReputation> userReputations_;
    std::unordered_map<std::string, ModerationAction> actionHistory_;
    std::shared_ptr<DiscordMutationAdapter> mutationAdapter_;
    std::string guildContext_;
    bool strictMode_{false};
    bool autoEscalation_{true};
    int reputationThreshold_{50};
    int actionCooldownSeconds_{300};
    int maxAttempts_{3};
    std::chrono::milliseconds maximumRetryDelay_{100};
    Clock clock_;
    ActionObserver actionObserver_;
    std::atomic<std::uint64_t> nextActionId_{1};
    mutable std::mutex moderatorMutex_;
    FilterAction determineAction(const ContentScanner::ScanResult& scanResult,
                                 const UserReputation& reputation) const;
    bool shouldEscalate(const UserReputation& reputation) const;
    bool isOnCooldownLocked(const std::string& userId,
                            std::chrono::system_clock::time_point now) const;
    DiscordAcknowledgement invokeMutation(
        const std::string& operation,
        const std::string& resourceId,
        const std::function<DiscordAcknowledgement(DiscordMutationAdapter&)>& call);
    ModerationAction makeAction(FilterAction action, const std::string& guildId,
                                const std::string& userId,
                                const std::string& channelId,
                                const std::string& messageId,
                                const std::string& reason);
    void commitAcknowledgedAction(ModerationAction action);
};

class ContentCleaner {
public:
    struct CleanupResult {
        int messagesScanned{0};
        int messagesDeleted{0};
        int duplicatesRemoved{0};
        int spamRemoved{0};
        int emptyRemoved{0};
        int oldRemoved{0};
        DiscordOperationStatus status{DiscordOperationStatus::INVALID_INPUT};
        std::vector<DiscordAcknowledgement> acknowledgements;
        std::vector<std::string> errors;
        bool acknowledged() const noexcept {
            return status == DiscordOperationStatus::ACKNOWLEDGED;
        }
    };
    ContentCleaner();
    ContentCleaner(std::shared_ptr<DiscordDataAdapter> dataAdapter,
                   std::shared_ptr<DiscordMutationAdapter> mutationAdapter);
    ~ContentCleaner();
    void setAdapters(std::shared_ptr<DiscordDataAdapter> dataAdapter,
                     std::shared_ptr<DiscordMutationAdapter> mutationAdapter);
    void setRetryPolicy(int maxAttempts, std::chrono::milliseconds maximumDelay);
    bool setStorageRoot(const std::string& rootPath);
    std::string getStorageRoot() const;
    MessagePage fetchMessages(const MessageQuery& query);
    MessagePage searchMessages(const MessageQuery& query);
    CleanupResult cleanChannel(const std::string& channelId, const CleanupConfig& config);
    CleanupResult cleanGuild(const std::string& guildId, const CleanupConfig& config);
    std::vector<CleanupResult> cleanAllChannels(const CleanupConfig& config);
    void scheduleCleanup(const std::string& channelId, const CleanupConfig& config,
                         const std::chrono::hours& interval);
    void cancelScheduledCleanup(const std::string& channelId);
    std::vector<std::string> getScheduledCleanups() const;
    void observeMessage(const DiscordMessage& message);
    void observeMessageDelete(const std::string& channelId, const std::string& messageId);
    std::vector<std::vector<DiscordMessage>> findDuplicateMessages(
        const std::string& channelId);
    bool areDuplicates(const DiscordMessage& first, const DiscordMessage& second,
                       double threshold = 0.8);
    DiscordAcknowledgement bulkDeleteMessagesAcknowledged(
        const std::string& channelId, const std::vector<std::string>& messageIds,
        const std::string& reason = "cleanup");
    DiscordAcknowledgement archiveChannelAcknowledged(const std::string& channelId,
                                                       const std::string& archivePath);
    DiscordAcknowledgement restoreFromArchiveAcknowledged(
        const std::string& channelId, const std::string& archivePath);
    bool bulkDeleteMessages(const std::string& channelId,
                            const std::vector<std::string>& messageIds);
    bool archiveChannel(const std::string& channelId, const std::string& archivePath);
    bool restoreFromArchive(const std::string& channelId, const std::string& archivePath);

private:
    std::unordered_map<std::string, CleanupConfig> scheduledCleanups_;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> nextCleanupTimes_;
    std::unordered_map<std::string, std::chrono::hours> cleanupIntervals_;
    std::unordered_map<std::string, std::vector<DiscordMessage>> messageCache_;
    std::shared_ptr<DiscordDataAdapter> dataAdapter_;
    std::shared_ptr<DiscordMutationAdapter> mutationAdapter_;
    std::filesystem::path storageRoot_;
    std::thread cleanupThread_;
    std::atomic<bool> cleanupRunning_{false};
    std::condition_variable cleanupCv_;
    int maxAttempts_{3};
    std::chrono::milliseconds maximumRetryDelay_{100};
    mutable std::mutex cleanerMutex_;
    std::vector<DiscordMessage> fetchAllMessages(const MessageQuery& query,
                                                 DiscordAcknowledgement& result);
    bool isSpamMessage(const DiscordMessage& message) const;
    static bool isEmptyMessage(const DiscordMessage& message);
    static bool isOldMessage(const DiscordMessage& message,
                             const std::chrono::hours& maxAge);
    static double calculateMessageSimilarity(const DiscordMessage& first,
                                             const DiscordMessage& second);
    DiscordAcknowledgement invokeMutation(
        const std::string& operation,
        const std::string& resourceId,
        const std::function<DiscordAcknowledgement(DiscordMutationAdapter&)>& call);
    void cleanupLoop();
};

class ModerationAnalytics {
public:
    struct ModerationReport {
        std::chrono::system_clock::time_point periodStart{};
        std::chrono::system_clock::time_point periodEnd{};
        int totalActions{0};
        int warningsIssued{0};
        int timeoutsIssued{0};
        int kicksIssued{0};
        int bansIssued{0};
        int messagesDeleted{0};
        std::vector<std::string> topViolators;
        std::vector<std::string> commonViolations;
        std::unordered_map<std::string, int> violationsByChannel;
        double averageResponseTime{0.0};
    };
    ModerationAnalytics();
    ~ModerationAnalytics();
    void recordAction(const ModerationAction& action);
    bool setStorageRoot(const std::string& rootPath);
    ModerationReport generateReport(
        const std::chrono::system_clock::time_point& startTime,
        const std::chrono::system_clock::time_point& endTime);
    ModerationReport generateDailyReport();
    ModerationReport generateWeeklyReport();
    ModerationReport generateMonthlyReport();
    std::vector<double> getViolationTrends(int days = 30);
    std::vector<std::string> getTopViolationTypes(int limit = 10);
    std::unordered_map<std::string, double> getChannelRiskScores();
    std::string exportReportAsJson(const ModerationReport& report);
    std::string exportReportAsHtml(const ModerationReport& report);
    DiscordAcknowledgement exportReportToFileAcknowledged(
        const ModerationReport& report, const std::string& filePath);
    bool exportReportToFile(const ModerationReport& report,
                            const std::string& filePath);

private:
    std::vector<ModerationAction> actions_;
    std::filesystem::path storageRoot_;
    mutable std::mutex analyticsMutex_;
    std::vector<ModerationAction> getActionsInPeriod(
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) const;
    static std::vector<std::string> findTopViolators(
        const std::vector<ModerationAction>& actions, int limit = 5);
    static std::vector<std::string> findCommonViolations(
        const std::vector<ModerationAction>& actions, int limit = 5);
};

class DiscrubExtension {
public:
    DiscrubExtension();
    ~DiscrubExtension();
    ContentScanner& getScanner() { return moderator_.getScanner(); }
    AutoModerator& getModerator() { return moderator_; }
    ContentCleaner& getCleaner() { return cleaner_; }
    ModerationAnalytics& getAnalytics() { return analytics_; }
    bool initializeWithDiscord(std::shared_ptr<DiscordClient> client);
    bool initializeWithAdapters(std::shared_ptr<DiscordDataAdapter> dataAdapter,
                                std::shared_ptr<DiscordMutationAdapter> mutationAdapter);
    void startMonitoring(const std::vector<std::string>& channelIds);
    void stopMonitoring();
    bool isMonitoring() const;
    AutoModerator::ModerationResult processIncomingMessageAcknowledged(
        const DiscordMessage& message);
    void processIncomingMessage(const DiscordMessage& message);
    void processMessageEdit(const DiscordMessage& oldMessage,
                            const DiscordMessage& newMessage);
    void processMessageDelete(const std::string& channelId,
                              const std::string& messageId);
    std::future<ContentCleaner::CleanupResult> scheduleBatchCleanup(
        const std::string& channelId, const CleanupConfig& config);
    std::future<ModerationAnalytics::ModerationReport> generateReport(
        const std::chrono::system_clock::time_point& startTime,
        const std::chrono::system_clock::time_point& endTime);
    bool setStorageRoot(const std::string& rootPath);
    DiscordAcknowledgement loadConfigurationAcknowledged(const std::string& configPath);
    DiscordAcknowledgement saveConfigurationAcknowledged(const std::string& configPath);
    void loadConfiguration(const std::string& configPath);
    void saveConfiguration(const std::string& configPath);
    void setDefaultModerationSettings();
    void setViolationHandler(
        std::function<void(const DiscordMessage&, const ContentScanner::ScanResult&)> handler);
    void setActionHandler(std::function<void(const ModerationAction&)> handler);
    void setCleanupHandler(
        std::function<void(const ContentCleaner::CleanupResult&)> handler);

private:
    AutoModerator moderator_;
    ContentCleaner cleaner_;
    ModerationAnalytics analytics_;
    std::shared_ptr<DiscordClient> discordClient_;
    std::shared_ptr<DiscordDataAdapter> dataAdapter_;
    std::shared_ptr<DiscordMutationAdapter> mutationAdapter_;
    std::vector<std::string> monitoredChannels_;
    std::atomic<bool> monitoring_{false};
    std::function<void(const DiscordMessage&, const ContentScanner::ScanResult&)>
        violationHandler_;
    std::function<void(const ModerationAction&)> actionHandler_;
    std::function<void(const ContentCleaner::CleanupResult&)> cleanupHandler_;
    std::unordered_map<std::string, std::string> config_;
    std::filesystem::path storageRoot_;
    mutable std::mutex configMutex_;
    void handleViolation(const DiscordMessage& message,
                         const ContentScanner::ScanResult& result);
};

extern std::shared_ptr<DiscrubExtension> globalDiscrubExtension;

} // namespace elizaos
