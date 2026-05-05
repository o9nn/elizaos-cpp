#include "elizaos/discrub_ext.hpp"
#include "elizaos/agentlogger.hpp"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <random>
#include <chrono>
#include <cmath>
#include <thread>
#include <iomanip>
#include <future>
#include <unordered_set>

namespace elizaos {

// ============================================================
// Internal helpers
// ============================================================

static std::string generateActionId() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<uint64_t> dis;
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    std::ostringstream oss;
    oss << std::hex << now << "_" << dis(gen);
    return oss.str();
}

static std::string filterActionToString(FilterAction action) {
    switch (action) {
        case FilterAction::NONE:    return "NONE";
        case FilterAction::WARN:    return "WARN";
        case FilterAction::DELETE:  return "DELETE";
        case FilterAction::TIMEOUT: return "TIMEOUT";
        case FilterAction::KICK:    return "KICK";
        case FilterAction::BAN:     return "BAN";
        default:                    return "UNKNOWN";
    }
}

// ============================================================
// AutoModerator implementation
// ============================================================

bool AutoModerator::processMessage(const DiscordMessage& message) {
    // Scan outside of lock (ContentScanner has its own mutex)
    auto scanResult = scanner_.scanMessage(message);
    if (!scanResult.violation) {
        return true;
    }

    UserReputation rep;
    bool isTrusted = false;
    bool onCooldown = false;
    FilterAction action;

    {
        std::lock_guard<std::mutex> lock(moderatorMutex_);
        auto& repRef = userReputations_[message.authorId];
        repRef.userId = message.authorId;
        rep = repRef;
        isTrusted = rep.isTrusted;
        onCooldown = isOnCooldown(message.authorId);
        action = determineAction(scanResult, rep);
    }

    if (isTrusted) {
        logInfo("Trusted user " + message.authorId + " exempted from moderation", "auto_moderator");
        return true;
    }

    if (onCooldown) {
        logInfo("User " + message.authorId + " is on moderation cooldown", "auto_moderator");
        return false;
    }

    ModerationAction modAction;
    modAction.id = generateActionId();
    modAction.userId = message.authorId;
    modAction.channelId = message.channelId;
    modAction.messageId = message.id;
    modAction.action = action;
    modAction.reason = scanResult.reason;

    {
        std::lock_guard<std::mutex> lock(moderatorMutex_);
        logAction(modAction);
    }

    return executeAction(modAction);
}

bool AutoModerator::processEdit(const DiscordMessage& /* oldMessage */, const DiscordMessage& newMessage) {
    return processMessage(newMessage);
}

bool AutoModerator::reviewUser(const std::string& userId) {
    std::lock_guard<std::mutex> lock(moderatorMutex_);

    auto it = userReputations_.find(userId);
    if (it == userReputations_.end()) {
        logInfo("No reputation data for user: " + userId, "auto_moderator");
        return true;
    }

    const auto& rep = it->second;
    if (autoEscalation_ && shouldEscalate(rep)) {
        logWarning("User " + userId + " flagged for review escalation (score: " +
                   std::to_string(rep.reputationScore) + ")", "auto_moderator");
        return false;
    }

    return true;
}

bool AutoModerator::executeAction(const ModerationAction& action) {
    switch (action.action) {
        case FilterAction::NONE:
            return true;
        case FilterAction::WARN:
            return warnUser(action.userId, action.reason, action.channelId);
        case FilterAction::DELETE:
            return deleteMessage(action.channelId, action.messageId, action.reason);
        case FilterAction::TIMEOUT:
            return timeoutUser(action.userId, 10, action.reason);
        case FilterAction::KICK:
            return kickUser(action.userId, action.reason);
        case FilterAction::BAN:
            return banUser(action.userId, action.reason);
        default:
            return false;
    }
}

bool AutoModerator::warnUser(const std::string& userId, const std::string& reason,
                              const std::string& channelId) {
    std::lock_guard<std::mutex> lock(moderatorMutex_);

    auto& rep = userReputations_[userId];
    rep.userId = userId;
    rep.warningCount++;
    rep.reputationScore = std::max(-1000, rep.reputationScore - 5);
    rep.lastIncident = std::chrono::system_clock::now();
    rep.violations.push_back("Warning: " + reason);

    ModerationAction action;
    action.id = generateActionId();
    action.userId = userId;
    action.channelId = channelId;
    action.action = FilterAction::WARN;
    action.reason = reason;
    actionHistory_[action.id] = action;

    logWarning("Warning issued to user " + userId + ": " + reason, "auto_moderator");
    return true;
}

bool AutoModerator::timeoutUser(const std::string& userId, int minutes, const std::string& reason) {
    std::lock_guard<std::mutex> lock(moderatorMutex_);

    auto& rep = userReputations_[userId];
    rep.userId = userId;
    rep.timeoutCount++;
    rep.reputationScore = std::max(-1000, rep.reputationScore - 15);
    rep.lastIncident = std::chrono::system_clock::now();
    rep.violations.push_back("Timeout (" + std::to_string(minutes) + "m): " + reason);

    ModerationAction action;
    action.id = generateActionId();
    action.userId = userId;
    action.action = FilterAction::TIMEOUT;
    action.reason = reason + " (duration: " + std::to_string(minutes) + " min)";
    actionHistory_[action.id] = action;

    logWarning("Timeout issued to user " + userId + " for " + std::to_string(minutes) +
               " minutes: " + reason, "auto_moderator");
    return true;
}

bool AutoModerator::kickUser(const std::string& userId, const std::string& reason) {
    std::lock_guard<std::mutex> lock(moderatorMutex_);

    auto& rep = userReputations_[userId];
    rep.userId = userId;
    rep.kickCount++;
    rep.reputationScore = std::max(-1000, rep.reputationScore - 25);
    rep.lastIncident = std::chrono::system_clock::now();
    rep.violations.push_back("Kick: " + reason);

    ModerationAction action;
    action.id = generateActionId();
    action.userId = userId;
    action.action = FilterAction::KICK;
    action.reason = reason;
    actionHistory_[action.id] = action;

    logWarning("Kick issued to user " + userId + ": " + reason, "auto_moderator");
    return true;
}

bool AutoModerator::banUser(const std::string& userId, const std::string& reason,
                             int deleteMessageDays) {
    std::lock_guard<std::mutex> lock(moderatorMutex_);

    auto& rep = userReputations_[userId];
    rep.userId = userId;
    rep.banCount++;
    rep.reputationScore = std::max(-1000, rep.reputationScore - 100);
    rep.lastIncident = std::chrono::system_clock::now();
    rep.violations.push_back("Ban: " + reason);

    ModerationAction action;
    action.id = generateActionId();
    action.userId = userId;
    action.action = FilterAction::BAN;
    action.reason = reason + " (delete_days: " + std::to_string(deleteMessageDays) + ")";
    actionHistory_[action.id] = action;

    logWarning("Ban issued to user " + userId + ": " + reason, "auto_moderator");
    return true;
}

bool AutoModerator::deleteMessage(const std::string& channelId, const std::string& messageId,
                                   const std::string& reason) {
    std::lock_guard<std::mutex> lock(moderatorMutex_);

    ModerationAction action;
    action.id = generateActionId();
    action.channelId = channelId;
    action.messageId = messageId;
    action.action = FilterAction::DELETE;
    action.reason = reason;
    actionHistory_[action.id] = action;

    logInfo("Message " + messageId + " in channel " + channelId +
            " marked for deletion: " + reason, "auto_moderator");
    return true;
}

void AutoModerator::updateUserReputation(const std::string& userId, int change,
                                          const std::string& reason) {
    std::lock_guard<std::mutex> lock(moderatorMutex_);

    auto& rep = userReputations_[userId];
    rep.userId = userId;
    rep.reputationScore = std::max(-1000, std::min(1000, rep.reputationScore + change));
    rep.lastIncident = std::chrono::system_clock::now();
    if (!reason.empty()) {
        rep.violations.push_back(reason);
    }

    logInfo("Updated reputation for " + userId + " by " + std::to_string(change) +
            " (new score: " + std::to_string(rep.reputationScore) + ")", "auto_moderator");
}

UserReputation AutoModerator::getUserReputation(const std::string& userId) {
    std::lock_guard<std::mutex> lock(moderatorMutex_);

    auto it = userReputations_.find(userId);
    if (it != userReputations_.end()) {
        return it->second;
    }
    return UserReputation(userId);
}

void AutoModerator::setTrustedUser(const std::string& userId, bool trusted) {
    std::lock_guard<std::mutex> lock(moderatorMutex_);

    auto& rep = userReputations_[userId];
    rep.userId = userId;
    rep.isTrusted = trusted;

    logInfo("User " + userId + " trust set to: " + (trusted ? "trusted" : "untrusted"),
            "auto_moderator");
}

void AutoModerator::setStrictMode(bool strict) {
    strictMode_ = strict;
    logInfo(std::string("Strict mode ") + (strict ? "enabled" : "disabled"), "auto_moderator");
}

void AutoModerator::setAutoEscalation(bool enable) {
    autoEscalation_ = enable;
    logInfo(std::string("Auto-escalation ") + (enable ? "enabled" : "disabled"), "auto_moderator");
}

void AutoModerator::setReputationThreshold(int threshold) {
    reputationThreshold_ = threshold;
    logInfo("Reputation threshold set to: " + std::to_string(threshold), "auto_moderator");
}

void AutoModerator::setActionCooldown(int seconds) {
    actionCooldownSeconds_ = seconds;
    logInfo("Action cooldown set to: " + std::to_string(seconds) + " seconds", "auto_moderator");
}

std::vector<ModerationAction> AutoModerator::getUserActions(const std::string& userId) {
    std::lock_guard<std::mutex> lock(moderatorMutex_);

    std::vector<ModerationAction> result;
    for (const auto& [id, action] : actionHistory_) {
        if (action.userId == userId) {
            result.push_back(action);
        }
    }

    std::sort(result.begin(), result.end(),
              [](const ModerationAction& a, const ModerationAction& b) {
                  return a.timestamp < b.timestamp;
              });

    return result;
}

std::vector<ModerationAction> AutoModerator::getChannelActions(const std::string& channelId) {
    std::lock_guard<std::mutex> lock(moderatorMutex_);

    std::vector<ModerationAction> result;
    for (const auto& [id, action] : actionHistory_) {
        if (action.channelId == channelId) {
            result.push_back(action);
        }
    }

    std::sort(result.begin(), result.end(),
              [](const ModerationAction& a, const ModerationAction& b) {
                  return a.timestamp < b.timestamp;
              });

    return result;
}

std::vector<ModerationAction> AutoModerator::getRecentActions(int hours) {
    std::lock_guard<std::mutex> lock(moderatorMutex_);

    auto cutoff = std::chrono::system_clock::now() - std::chrono::hours(hours);

    std::vector<ModerationAction> result;
    for (const auto& [id, action] : actionHistory_) {
        if (action.timestamp >= cutoff) {
            result.push_back(action);
        }
    }

    std::sort(result.begin(), result.end(),
              [](const ModerationAction& a, const ModerationAction& b) {
                  return a.timestamp < b.timestamp;
              });

    return result;
}

bool AutoModerator::submitAppeal(const std::string& actionId, const std::string& reason) {
    std::lock_guard<std::mutex> lock(moderatorMutex_);

    auto it = actionHistory_.find(actionId);
    if (it == actionHistory_.end()) {
        logError("Appeal for unknown action: " + actionId, "auto_moderator");
        return false;
    }

    it->second.appealed = true;
    it->second.appealReason = reason;

    logInfo("Appeal submitted for action " + actionId + ": " + reason, "auto_moderator");
    return true;
}

bool AutoModerator::reviewAppeal(const std::string& actionId, bool approved,
                                  const std::string& moderatorId) {
    std::lock_guard<std::mutex> lock(moderatorMutex_);

    auto it = actionHistory_.find(actionId);
    if (it == actionHistory_.end()) {
        logError("Review for unknown action: " + actionId, "auto_moderator");
        return false;
    }

    if (!it->second.appealed) {
        logError("No pending appeal for action: " + actionId, "auto_moderator");
        return false;
    }

    it->second.moderatorId = moderatorId;

    if (approved) {
        auto& rep = userReputations_[it->second.userId];
        int restoration = 0;
        switch (it->second.action) {
            case FilterAction::WARN:
                rep.warningCount = std::max(0, rep.warningCount - 1);
                restoration = 5;
                break;
            case FilterAction::TIMEOUT:
                rep.timeoutCount = std::max(0, rep.timeoutCount - 1);
                restoration = 15;
                break;
            case FilterAction::KICK:
                rep.kickCount = std::max(0, rep.kickCount - 1);
                restoration = 25;
                break;
            case FilterAction::BAN:
                rep.banCount = std::max(0, rep.banCount - 1);
                restoration = 100;
                break;
            default:
                break;
        }
        rep.reputationScore = std::min(1000, rep.reputationScore + restoration);
        logInfo("Appeal APPROVED for action " + actionId + " by " + moderatorId, "auto_moderator");
    } else {
        logInfo("Appeal DENIED for action " + actionId + " by " + moderatorId, "auto_moderator");
    }

    // Mark appeal as reviewed
    it->second.appealed = false;

    return true;
}

std::vector<ModerationAction> AutoModerator::getPendingAppeals() {
    std::lock_guard<std::mutex> lock(moderatorMutex_);

    std::vector<ModerationAction> result;
    for (const auto& [id, action] : actionHistory_) {
        if (action.appealed) {
            result.push_back(action);
        }
    }

    return result;
}

FilterAction AutoModerator::determineAction(const ContentScanner::ScanResult& scanResult,
                                             const UserReputation& reputation) {
    FilterAction base = scanResult.recommendedAction;

    auto escalate = [](FilterAction a) {
        int next = static_cast<int>(a) + 1;
        return (next <= static_cast<int>(FilterAction::BAN))
                   ? static_cast<FilterAction>(next)
                   : FilterAction::BAN;
    };

    if (autoEscalation_ && shouldEscalate(reputation)) {
        base = escalate(base);
    }

    if (strictMode_) {
        base = escalate(base);
    }

    return base;
}

bool AutoModerator::shouldEscalate(const UserReputation& reputation) {
    return reputation.reputationScore < reputationThreshold_ ||
           reputation.warningCount >= 3 ||
           reputation.timeoutCount >= 2;
}

bool AutoModerator::isOnCooldown(const std::string& userId) {
    auto cutoff = std::chrono::system_clock::now() -
                  std::chrono::seconds(actionCooldownSeconds_);

    for (const auto& [id, action] : actionHistory_) {
        if (action.userId == userId && action.timestamp >= cutoff) {
            return true;
        }
    }
    return false;
}

void AutoModerator::logAction(const ModerationAction& action) {
    actionHistory_[action.id] = action;
    logInfo("Moderation action [" + filterActionToString(action.action) +
            "] for user " + action.userId + ": " + action.reason, "auto_moderator");
}

// ============================================================
// ContentCleaner implementation
// ============================================================

ContentCleaner::CleanupResult ContentCleaner::cleanChannel(const std::string& channelId,
                                                            const CleanupConfig& config) {
    CleanupResult result;

    logInfo("Starting cleanup for channel: " + channelId, "content_cleaner");

    // In a production system this fetches messages via Discord API.
    // findMessagesToDelete returns an empty vector when no client is available.
    auto toDelete = findMessagesToDelete(channelId, config);

    std::vector<std::string> ids;
    for (const auto& msg : toDelete) {
        ids.push_back(msg.id);
        result.messagesDeleted++;

        if (config.deleteSpam   && isSpamMessage(msg))               result.spamRemoved++;
        if (config.deleteEmpty  && isEmptyMessage(msg))              result.emptyRemoved++;
        if (config.deleteOldMessages && isOldMessage(msg, config.maxAge)) result.oldRemoved++;
    }

    if (config.deleteDuplicates) {
        auto groups = findDuplicateMessages(channelId);
        for (auto& group : groups) {
            for (size_t i = 1; i < group.size(); ++i) {
                ids.push_back(group[i].id);
                result.duplicatesRemoved++;
                result.messagesDeleted++;
            }
        }
    }

    if (!ids.empty()) {
        if (!bulkDeleteMessages(channelId, ids)) {
            result.errors.push_back("Bulk delete failed for channel " + channelId);
        }
    }

    logInfo("Cleanup complete for channel " + channelId + ": deleted " +
            std::to_string(result.messagesDeleted) + " messages", "content_cleaner");

    return result;
}

ContentCleaner::CleanupResult ContentCleaner::cleanGuild(const std::string& guildId,
                                                          const CleanupConfig& config) {
    CleanupResult total;

    logInfo("Starting guild-wide cleanup for: " + guildId, "content_cleaner");

    // Aggregate results across all scheduled channels for this guild.
    // A full implementation would enumerate guild channels via the Discord API.
    auto channelResults = cleanAllChannels(config);
    for (const auto& r : channelResults) {
        total.messagesDeleted   += r.messagesDeleted;
        total.duplicatesRemoved += r.duplicatesRemoved;
        total.spamRemoved       += r.spamRemoved;
        total.emptyRemoved      += r.emptyRemoved;
        total.oldRemoved        += r.oldRemoved;
        total.errors.insert(total.errors.end(), r.errors.begin(), r.errors.end());
    }

    logInfo("Guild cleanup complete for: " + guildId, "content_cleaner");
    return total;
}

std::vector<ContentCleaner::CleanupResult> ContentCleaner::cleanAllChannels(
    const CleanupConfig& config) {
    std::vector<std::string> channels;

    {
        std::lock_guard<std::mutex> lock(cleanerMutex_);
        for (const auto& [channelId, _] : scheduledCleanups_) {
            bool preserved = std::any_of(config.preserveChannels.begin(),
                                         config.preserveChannels.end(),
                                         [&channelId](const std::string& c) {
                                             return c == channelId;
                                         });
            if (!preserved) {
                channels.push_back(channelId);
            }
        }
    }

    std::vector<CleanupResult> results;
    results.reserve(channels.size());
    for (const auto& ch : channels) {
        results.push_back(cleanChannel(ch, config));
    }

    return results;
}

void ContentCleaner::scheduleCleanup(const std::string& channelId,
                                      const CleanupConfig& config,
                                      const std::chrono::hours& interval) {
    std::lock_guard<std::mutex> lock(cleanerMutex_);

    scheduledCleanups_[channelId] = config;
    nextCleanupTimes_[channelId] = std::chrono::system_clock::now() + interval;

    logInfo("Scheduled cleanup for channel " + channelId + " every " +
            std::to_string(interval.count()) + " hours", "content_cleaner");
}

void ContentCleaner::cancelScheduledCleanup(const std::string& channelId) {
    std::lock_guard<std::mutex> lock(cleanerMutex_);

    scheduledCleanups_.erase(channelId);
    nextCleanupTimes_.erase(channelId);

    logInfo("Cancelled scheduled cleanup for channel: " + channelId, "content_cleaner");
}

std::vector<std::string> ContentCleaner::getScheduledCleanups() const {
    std::lock_guard<std::mutex> lock(cleanerMutex_);

    std::vector<std::string> result;
    result.reserve(scheduledCleanups_.size());
    for (const auto& [channelId, _] : scheduledCleanups_) {
        result.push_back(channelId);
    }
    return result;
}

std::vector<std::vector<DiscordMessage>> ContentCleaner::findDuplicateMessages(
    const std::string& channelId) {
    // In production, fetch messages from Discord API and group by similarity.
    (void)channelId;
    return {};
}

bool ContentCleaner::areDuplicates(const DiscordMessage& msg1, const DiscordMessage& msg2,
                                    double threshold) {
    return calculateMessageSimilarity(msg1, msg2) >= threshold;
}

bool ContentCleaner::bulkDeleteMessages(const std::string& channelId,
                                         const std::vector<std::string>& messageIds) {
    if (messageIds.empty()) return true;

    logInfo("Bulk deleting " + std::to_string(messageIds.size()) +
            " messages from channel: " + channelId, "content_cleaner");

    // Discord API bulk-delete supports up to 100 messages at a time,
    // only for messages younger than 14 days.  A production implementation
    // would batch calls here via the Discord REST client.

    return true;
}

bool ContentCleaner::archiveChannel(const std::string& channelId,
                                     const std::string& archivePath) {
    logInfo("Archiving channel " + channelId + " to: " + archivePath, "content_cleaner");

    std::ofstream archive(archivePath);
    if (!archive.is_open()) {
        logError("Failed to open archive file: " + archivePath, "content_cleaner");
        return false;
    }

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    archive << "{\n"
            << "  \"channelId\": \"" << channelId << "\",\n"
            << "  \"archivedAt\": " << now << ",\n"
            << "  \"messages\": []\n"
            << "}\n";

    archive.close();
    logInfo("Channel " + channelId + " archived to " + archivePath, "content_cleaner");
    return true;
}

bool ContentCleaner::restoreFromArchive(const std::string& channelId,
                                         const std::string& archivePath) {
    logInfo("Restoring channel " + channelId + " from: " + archivePath, "content_cleaner");

    std::ifstream archive(archivePath);
    if (!archive.is_open()) {
        logError("Failed to open archive file: " + archivePath, "content_cleaner");
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(archive)),
                        std::istreambuf_iterator<char>());
    archive.close();

    logInfo("Archive loaded for channel " + channelId + " (" +
            std::to_string(content.length()) + " bytes)", "content_cleaner");
    return true;
}

std::vector<DiscordMessage> ContentCleaner::findMessagesToDelete(
    const std::string& channelId, const CleanupConfig& config) {
    // Production: fetch messages from Discord API and filter by config rules.
    (void)channelId;
    (void)config;
    return {};
}

bool ContentCleaner::isSpamMessage(const DiscordMessage& message) {
    const std::string& content = message.content;

    // Excessive character repetition
    size_t maxRepeat = 0;
    for (size_t i = 0; i < content.size(); ++i) {
        size_t run = 1;
        while (i + run < content.size() && content[i] == content[i + run]) ++run;
        maxRepeat = std::max(maxRepeat, run);
    }
    if (maxRepeat > 5) return true;
    if (content.length() > 2000) return true;

    // Common spam phrases
    std::string lower = content;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    const std::vector<std::string> spamPhrases = {
        "free nitro", "discord gift", "steam gift", "click here to claim",
        "win a prize", "limited time offer"
    };
    for (const auto& phrase : spamPhrases) {
        if (lower.find(phrase) != std::string::npos) return true;
    }

    return false;
}

bool ContentCleaner::isEmptyMessage(const DiscordMessage& message) {
    std::string trimmed = message.content;
    trimmed.erase(std::remove_if(trimmed.begin(), trimmed.end(),
                                 [](unsigned char c) { return std::isspace(c) != 0; }),
                  trimmed.end());
    return trimmed.empty();
}

bool ContentCleaner::isOldMessage(const DiscordMessage& message,
                                   const std::chrono::hours& maxAge) {
    return (std::chrono::system_clock::now() - message.timestamp) > maxAge;
}

double ContentCleaner::calculateMessageSimilarity(const DiscordMessage& msg1,
                                                   const DiscordMessage& msg2) {
    const std::string& s1 = msg1.content;
    const std::string& s2 = msg2.content;

    if (s1.empty() && s2.empty()) return 1.0;
    if (s1.empty() || s2.empty()) return 0.0;
    if (s1 == s2) return 1.0;

    // Jaccard similarity over character trigrams
    auto trigrams = [](const std::string& s) {
        std::unordered_set<std::string> tg;
        for (size_t i = 0; i + 3 <= s.size(); ++i) {
            tg.insert(s.substr(i, 3));
        }
        return tg;
    };

    auto t1 = trigrams(s1);
    auto t2 = trigrams(s2);

    if (t1.empty() && t2.empty()) return 1.0;
    if (t1.empty() || t2.empty()) return 0.0;

    size_t intersect = 0;
    for (const auto& t : t1) {
        if (t2.count(t)) ++intersect;
    }
    size_t unionSz = t1.size() + t2.size() - intersect;
    return static_cast<double>(intersect) / static_cast<double>(unionSz);
}

void ContentCleaner::cleanupLoop() {
    logInfo("Cleanup loop started", "content_cleaner");

    while (cleanupRunning_) {
        std::this_thread::sleep_for(std::chrono::minutes(5));

        auto now = std::chrono::system_clock::now();

        std::vector<std::pair<std::string, CleanupConfig>> due;
        {
            std::lock_guard<std::mutex> lock(cleanerMutex_);
            for (auto& [channelId, nextTime] : nextCleanupTimes_) {
                if (now >= nextTime) {
                    auto it = scheduledCleanups_.find(channelId);
                    if (it != scheduledCleanups_.end()) {
                        due.emplace_back(channelId, it->second);
                        nextTime = now + std::chrono::hours(24);
                    }
                }
            }
        }

        for (const auto& [channelId, config] : due) {
            cleanChannel(channelId, config);
        }
    }

    logInfo("Cleanup loop ended", "content_cleaner");
}

// ============================================================
// ModerationAnalytics implementation
// ============================================================

ModerationAnalytics::ModerationReport ModerationAnalytics::generateReport(
    const std::chrono::system_clock::time_point& startTime,
    const std::chrono::system_clock::time_point& endTime) {
    std::lock_guard<std::mutex> lock(analyticsMutex_);

    ModerationReport report;
    report.periodStart = startTime;
    report.periodEnd   = endTime;

    auto actions = getActionsInPeriod(startTime, endTime);

    for (const auto& action : actions) {
        report.totalActions++;
        switch (action.action) {
            case FilterAction::WARN:    report.warningsIssued++; break;
            case FilterAction::TIMEOUT: report.timeoutsIssued++; break;
            case FilterAction::KICK:    report.kicksIssued++;    break;
            case FilterAction::BAN:     report.bansIssued++;     break;
            case FilterAction::DELETE:  report.messagesDeleted++; break;
            default: break;
        }
        if (!action.channelId.empty()) {
            report.violationsByChannel[action.channelId]++;
        }
    }

    report.topViolators     = findTopViolators(actions, 5);
    report.commonViolations = findCommonViolations(actions, 5);

    return report;
}

ModerationAnalytics::ModerationReport ModerationAnalytics::generateDailyReport() {
    auto now   = std::chrono::system_clock::now();
    return generateReport(now - std::chrono::hours(24), now);
}

ModerationAnalytics::ModerationReport ModerationAnalytics::generateWeeklyReport() {
    auto now = std::chrono::system_clock::now();
    return generateReport(now - std::chrono::hours(24 * 7), now);
}

ModerationAnalytics::ModerationReport ModerationAnalytics::generateMonthlyReport() {
    auto now = std::chrono::system_clock::now();
    return generateReport(now - std::chrono::hours(24 * 30), now);
}

std::vector<double> ModerationAnalytics::getViolationTrends(int days) {
    std::vector<double> trends(static_cast<size_t>(days), 0.0);
    auto now = std::chrono::system_clock::now();

    for (int d = 0; d < days; ++d) {
        auto dayStart = now - std::chrono::hours(24 * (days - d));
        auto dayEnd   = now - std::chrono::hours(24 * (days - d - 1));
        std::lock_guard<std::mutex> lock(analyticsMutex_);
        auto acts = getActionsInPeriod(dayStart, dayEnd);
        trends[static_cast<size_t>(d)] = static_cast<double>(acts.size());
    }

    return trends;
}

std::vector<std::string> ModerationAnalytics::getTopViolationTypes(int limit) {
    auto now = std::chrono::system_clock::now();
    std::lock_guard<std::mutex> lock(analyticsMutex_);
    auto actions = getActionsInPeriod(now - std::chrono::hours(24 * 30), now);
    return findCommonViolations(actions, limit);
}

std::unordered_map<std::string, double> ModerationAnalytics::getChannelRiskScores() {
    auto now = std::chrono::system_clock::now();
    std::lock_guard<std::mutex> lock(analyticsMutex_);
    auto actions = getActionsInPeriod(now - std::chrono::hours(24 * 7), now);

    std::unordered_map<std::string, int> channelCounts;
    int total = static_cast<int>(actions.size());

    for (const auto& action : actions) {
        if (!action.channelId.empty()) {
            channelCounts[action.channelId]++;
        }
    }

    std::unordered_map<std::string, double> scores;
    for (const auto& [channelId, count] : channelCounts) {
        scores[channelId] = (total > 0)
            ? static_cast<double>(count) / static_cast<double>(total) * 100.0
            : 0.0;
    }

    return scores;
}

std::string ModerationAnalytics::exportReportAsJson(const ModerationReport& report) {
    auto toEpoch = [](const std::chrono::system_clock::time_point& tp) {
        return std::chrono::duration_cast<std::chrono::seconds>(
                   tp.time_since_epoch()).count();
    };

    std::ostringstream j;
    j << "{\n"
      << "  \"periodStart\": "        << toEpoch(report.periodStart)      << ",\n"
      << "  \"periodEnd\": "          << toEpoch(report.periodEnd)        << ",\n"
      << "  \"totalActions\": "       << report.totalActions              << ",\n"
      << "  \"warningsIssued\": "     << report.warningsIssued            << ",\n"
      << "  \"timeoutsIssued\": "     << report.timeoutsIssued            << ",\n"
      << "  \"kicksIssued\": "        << report.kicksIssued               << ",\n"
      << "  \"bansIssued\": "         << report.bansIssued                << ",\n"
      << "  \"messagesDeleted\": "    << report.messagesDeleted           << ",\n"
      << "  \"averageResponseTime\": "<< report.averageResponseTime       << ",\n";

    j << "  \"topViolators\": [";
    for (size_t i = 0; i < report.topViolators.size(); ++i) {
        if (i > 0) j << ", ";
        j << "\"" << report.topViolators[i] << "\"";
    }
    j << "],\n";

    j << "  \"commonViolations\": [";
    for (size_t i = 0; i < report.commonViolations.size(); ++i) {
        if (i > 0) j << ", ";
        j << "\"" << report.commonViolations[i] << "\"";
    }
    j << "],\n";

    j << "  \"violationsByChannel\": {";
    bool first = true;
    for (const auto& [ch, cnt] : report.violationsByChannel) {
        if (!first) j << ", ";
        j << "\"" << ch << "\": " << cnt;
        first = false;
    }
    j << "}\n}\n";

    return j.str();
}

std::string ModerationAnalytics::exportReportAsHtml(const ModerationReport& report) {
    auto formatTime = [](const std::chrono::system_clock::time_point& tp) {
        auto t = std::chrono::system_clock::to_time_t(tp);
        std::tm* tm_ptr = std::gmtime(&t);
        char buf[64] = {};
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S UTC", tm_ptr);
        return std::string(buf);
    };

    std::ostringstream h;
    h << "<!DOCTYPE html>\n<html>\n<head>\n"
      << "<title>Moderation Report</title>\n"
      << "<style>"
      << "body{font-family:sans-serif;max-width:900px;margin:0 auto;padding:20px;}"
      << "table{border-collapse:collapse;width:100%;}"
      << "th,td{border:1px solid #ddd;padding:8px;text-align:left;}"
      << "th{background:#f2f2f2;}"
      << "</style>\n"
      << "</head>\n<body>\n"
      << "<h1>Moderation Report</h1>\n"
      << "<p><strong>Period:</strong> "
      << formatTime(report.periodStart) << " to " << formatTime(report.periodEnd)
      << "</p>\n"
      << "<h2>Summary</h2>\n"
      << "<table>\n<tr><th>Metric</th><th>Count</th></tr>\n"
      << "<tr><td>Total Actions</td><td>"    << report.totalActions    << "</td></tr>\n"
      << "<tr><td>Warnings</td><td>"         << report.warningsIssued  << "</td></tr>\n"
      << "<tr><td>Timeouts</td><td>"         << report.timeoutsIssued  << "</td></tr>\n"
      << "<tr><td>Kicks</td><td>"            << report.kicksIssued     << "</td></tr>\n"
      << "<tr><td>Bans</td><td>"             << report.bansIssued      << "</td></tr>\n"
      << "<tr><td>Messages Deleted</td><td>" << report.messagesDeleted << "</td></tr>\n"
      << "</table>\n";

    if (!report.topViolators.empty()) {
        h << "<h2>Top Violators</h2>\n<ul>\n";
        for (const auto& v : report.topViolators) {
            h << "<li>" << v << "</li>\n";
        }
        h << "</ul>\n";
    }

    if (!report.violationsByChannel.empty()) {
        h << "<h2>Violations by Channel</h2>\n"
          << "<table>\n<tr><th>Channel ID</th><th>Violations</th></tr>\n";
        for (const auto& [ch, cnt] : report.violationsByChannel) {
            h << "<tr><td>" << ch << "</td><td>" << cnt << "</td></tr>\n";
        }
        h << "</table>\n";
    }

    h << "</body>\n</html>\n";
    return h.str();
}

bool ModerationAnalytics::exportReportToFile(const ModerationReport& report,
                                              const std::string& filePath) {
    bool useHtml = filePath.size() >= 5 &&
                   filePath.substr(filePath.size() - 5) == ".html";
    std::string content = useHtml ? exportReportAsHtml(report) : exportReportAsJson(report);

    std::ofstream file(filePath);
    if (!file.is_open()) {
        logError("Failed to open report file: " + filePath, "analytics");
        return false;
    }
    file << content;
    file.close();

    logInfo("Report exported to: " + filePath, "analytics");
    return true;
}

std::vector<ModerationAction> ModerationAnalytics::getActionsInPeriod(
    const std::chrono::system_clock::time_point& start,
    const std::chrono::system_clock::time_point& end) {
    // Analytics maintains its own history; populate via AutoModerator integration
    // or a shared data store.  Without external data, returns empty.
    (void)start;
    (void)end;
    return {};
}

std::vector<std::string> ModerationAnalytics::findTopViolators(
    const std::vector<ModerationAction>& actions, int limit) {
    std::unordered_map<std::string, int> counts;
    for (const auto& a : actions) {
        if (!a.userId.empty()) counts[a.userId]++;
    }

    std::vector<std::pair<std::string, int>> sorted(counts.begin(), counts.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
                  return a.second > b.second;
              });

    std::vector<std::string> result;
    for (int i = 0; i < limit && i < static_cast<int>(sorted.size()); ++i) {
        result.push_back(sorted[static_cast<size_t>(i)].first);
    }
    return result;
}

std::vector<std::string> ModerationAnalytics::findCommonViolations(
    const std::vector<ModerationAction>& actions, int limit) {
    std::unordered_map<std::string, int> counts;
    for (const auto& a : actions) {
        if (!a.reason.empty()) counts[a.reason]++;
    }

    std::vector<std::pair<std::string, int>> sorted(counts.begin(), counts.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
                  return a.second > b.second;
              });

    std::vector<std::string> result;
    for (int i = 0; i < limit && i < static_cast<int>(sorted.size()); ++i) {
        result.push_back(sorted[static_cast<size_t>(i)].first);
    }
    return result;
}

// ============================================================
// DiscrubExtension – remaining methods
// ============================================================

std::future<ContentCleaner::CleanupResult> DiscrubExtension::scheduleBatchCleanup(
    const std::string& channelId, const CleanupConfig& config) {
    return std::async(std::launch::async, [this, channelId, config]() {
        auto result = cleaner_.cleanChannel(channelId, config);
        if (cleanupHandler_) {
            cleanupHandler_(result);
        }
        return result;
    });
}

std::future<ModerationAnalytics::ModerationReport> DiscrubExtension::generateReport(
    const std::chrono::system_clock::time_point& startTime,
    const std::chrono::system_clock::time_point& endTime) {
    return std::async(std::launch::async, [this, startTime, endTime]() {
        return analytics_.generateReport(startTime, endTime);
    });
}

void DiscrubExtension::setViolationHandler(
    std::function<void(const DiscordMessage&, const ContentScanner::ScanResult&)> handler) {
    std::lock_guard<std::mutex> lock(configMutex_);
    violationHandler_ = std::move(handler);
    logInfo("Violation handler registered", "discrub_ext");
}

void DiscrubExtension::setActionHandler(
    std::function<void(const ModerationAction&)> handler) {
    std::lock_guard<std::mutex> lock(configMutex_);
    actionHandler_ = std::move(handler);
    logInfo("Action handler registered", "discrub_ext");
}

void DiscrubExtension::setCleanupHandler(
    std::function<void(const ContentCleaner::CleanupResult&)> handler) {
    std::lock_guard<std::mutex> lock(configMutex_);
    cleanupHandler_ = std::move(handler);
    logInfo("Cleanup handler registered", "discrub_ext");
}

void DiscrubExtension::handleViolation(const DiscordMessage& message,
                                        const ContentScanner::ScanResult& result) {
    if (violationHandler_) {
        violationHandler_(message, result);
    }

    // Delegate action to the moderator
    moderator_.processMessage(message);

    logWarning("Violation handled for message " + message.id + ": " + result.reason,
               "discrub_ext");
}

} // namespace elizaos
