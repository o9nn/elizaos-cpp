#include "elizaos/discrub_ext.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <thread>

namespace elizaos {
namespace {

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

std::string lowerCopy(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char character) {
        return static_cast<char>(std::tolower(character));
    });
    return value;
}

std::string actionName(FilterAction action) {
    switch (action) {
        case FilterAction::NONE: return "none";
        case FilterAction::WARN: return "warn";
        case FilterAction::DELETE: return "delete";
        case FilterAction::TIMEOUT: return "timeout";
        case FilterAction::KICK: return "kick";
        case FilterAction::BAN: return "ban";
    }
    return "invalid";
}

DiscordAcknowledgement invalidResult(const std::string& operation,
                                     const std::string& resource,
                                     const std::string& detail) {
    DiscordAcknowledgement result;
    result.status = DiscordOperationStatus::INVALID_INPUT;
    result.operation = operation;
    result.resourceId = resource;
    result.detail = detail;
    return result;
}

bool actionLess(const ModerationAction& left, const ModerationAction& right) {
    if (left.timestamp != right.timestamp) {
        return left.timestamp < right.timestamp;
    }
    return left.id < right.id;
}

} // namespace

ContentScanner::ContentScanner()
    : profanityWords_{"damn", "hell", "crap"},
      allowedDomains_{"discord.com", "github.com", "google.com"},
      blockedDomains_{"suspicious-site.com", "malware.net"} {
    filters_.emplace_back("spam_repetition", "(.)\\1{5,}",
                          FilterAction::DELETE, 5);
    filters_.back().reason = "repeated-character spam heuristic";
    filters_.emplace_back("excessive_caps", "[A-Z]{10,}",
                          FilterAction::WARN, 2);
    filters_.back().reason = "excessive-capitalization heuristic";
}

ContentScanner::~ContentScanner() = default;

void ContentScanner::addFilter(const ContentFilter& filter) {
    if (filter.name.empty() || filter.name.size() > 64 || filter.severity < 1 ||
        filter.severity > 10 || filter.reason.size() > DISCRUB_MAX_REASON_LENGTH ||
        filter.description.size() > 512) {
        return;
    }
    std::lock_guard<std::mutex> lock(scannerMutex_);
    filters_.erase(std::remove_if(filters_.begin(), filters_.end(),
                                  [&filter](const ContentFilter& existing) {
                                      return existing.name == filter.name;
                                  }),
                   filters_.end());
    filters_.push_back(filter);
}

void ContentScanner::removeFilter(const std::string& name) {
    if (name.empty() || name.size() > 64) {
        return;
    }
    std::lock_guard<std::mutex> lock(scannerMutex_);
    filters_.erase(std::remove_if(filters_.begin(), filters_.end(),
                                  [&name](const ContentFilter& filter) {
                                      return filter.name == name;
                                  }),
                   filters_.end());
}

void ContentScanner::updateFilter(const std::string& name,
                                  const ContentFilter& filter) {
    if (name.empty() || name.size() > 64 || filter.name.empty() ||
        filter.name.size() > 64 || filter.severity < 1 || filter.severity > 10 ||
        filter.reason.size() > DISCRUB_MAX_REASON_LENGTH ||
        filter.description.size() > 512) {
        return;
    }
    std::lock_guard<std::mutex> lock(scannerMutex_);
    const auto iterator = std::find_if(filters_.begin(), filters_.end(),
                                       [&name](const ContentFilter& existing) {
                                           return existing.name == name;
                                       });
    if (iterator != filters_.end()) {
        *iterator = filter;
    }
}

std::vector<ContentFilter> ContentScanner::getFilters() const {
    std::lock_guard<std::mutex> lock(scannerMutex_);
    return filters_;
}

ContentScanner::ScanResult ContentScanner::scanMessage(const DiscordMessage& message) {
    if (!validDiscordId(message.id) || !validDiscordId(message.channelId) ||
        !validOptionalDiscordId(message.guildId) || !validDiscordId(message.authorId) ||
        !validTimestamp(message.timestamp) ||
        message.content.size() > DISCRUB_MAX_CONTENT_LENGTH) {
        ScanResult result;
        result.validInput = false;
        result.reason = "invalid message id, timestamp, or content bounds";
        return result;
    }
    std::lock_guard<std::mutex> lock(scannerMutex_);
    return scanLocked(message.content, &message);
}

ContentScanner::ScanResult ContentScanner::scanContent(const std::string& content) {
    if (content.size() > DISCRUB_MAX_CONTENT_LENGTH ||
        content.find('\0') != std::string::npos) {
        ScanResult result;
        result.validInput = false;
        result.reason = "content exceeds local classification bounds";
        return result;
    }
    std::lock_guard<std::mutex> lock(scannerMutex_);
    return scanLocked(content, nullptr);
}

std::vector<ContentScanner::ScanResult> ContentScanner::scanMessages(
    const std::vector<DiscordMessage>& messages) {
    std::vector<ScanResult> results;
    results.reserve(messages.size());
    for (const auto& message : messages) {
        results.push_back(scanMessage(message));
    }
    return results;
}

ContentScanner::ScanResult ContentScanner::scanLocked(
    const std::string& content, const DiscordMessage* message) const {
    ScanResult result;
    const auto addEvidence = [&result](const std::string& filter, int severity,
                                       FilterAction action,
                                       const std::string& evidence) {
        if (std::find(result.triggeredFilters.begin(), result.triggeredFilters.end(),
                      filter) != result.triggeredFilters.end()) {
            return;
        }
        result.violation = true;
        result.triggeredFilters.push_back(filter);
        result.evidence.push_back(evidence);
        result.totalSeverity = std::min(100, result.totalSeverity + severity);
        if (static_cast<int>(action) > static_cast<int>(result.recommendedAction)) {
            result.recommendedAction = action;
        }
    };

    for (const auto& filter : filters_) {
        if (!filter.enabled) {
            continue;
        }
        try {
            if (std::regex_search(content, filter.pattern)) {
                addEvidence(filter.name, filter.severity, filter.action,
                            filter.reason.empty() ? "custom local pattern matched"
                                                  : filter.reason);
            }
        } catch (const std::regex_error&) {
            // A malformed custom rule is isolated from all other local classifiers.
        }
    }
    if (profanityFilterEnabled_ && detectProfanityLocked(content)) {
        addEvidence("local-profanity", 3, FilterAction::WARN,
                    "bounded local term-list match");
    }
    if (spamFilterEnabled_ && message != nullptr && detectSpamLocked(*message)) {
        addEvidence("local-spam", 5, FilterAction::DELETE,
                    "repetition, phrase, or mention-spam heuristic");
    }
    if (phishingFilterEnabled_ && detectPhishingLocked(content)) {
        addEvidence("local-phishing", 8, FilterAction::DELETE,
                    "suspicious phrase with URL or blocked host");
    }
    if (inviteFilterEnabled_ && detectInviteLinksLocked(content)) {
        addEvidence("local-invite", 4, FilterAction::DELETE,
                    "Discord invite URL pattern");
    }
    if (message != nullptr && mentionSpamEnabled_ &&
        detectMentionSpamLocked(*message)) {
        addEvidence("local-mention-spam", 5, FilterAction::DELETE,
                    "mention count exceeded configured bound");
    }
    if (result.violation) {
        std::ostringstream reason;
        reason << "local classification: ";
        for (std::size_t index = 0; index < result.triggeredFilters.size(); ++index) {
            if (index != 0) {
                reason << ", ";
            }
            reason << result.triggeredFilters[index];
        }
        result.reason = reason.str();
    }
    return result;
}

void ContentScanner::enableProfanityFilter(bool enable) {
    std::lock_guard<std::mutex> lock(scannerMutex_);
    profanityFilterEnabled_ = enable;
}

void ContentScanner::enableSpamFilter(bool enable) {
    std::lock_guard<std::mutex> lock(scannerMutex_);
    spamFilterEnabled_ = enable;
}

void ContentScanner::enablePhishingFilter(bool enable) {
    std::lock_guard<std::mutex> lock(scannerMutex_);
    phishingFilterEnabled_ = enable;
}

void ContentScanner::enableInviteFilter(bool enable) {
    std::lock_guard<std::mutex> lock(scannerMutex_);
    inviteFilterEnabled_ = enable;
}

void ContentScanner::enableMentionSpamFilter(bool enable, int maxMentions) {
    if (maxMentions < 0 || maxMentions > 100) {
        return;
    }
    std::lock_guard<std::mutex> lock(scannerMutex_);
    mentionSpamEnabled_ = enable;
    maxMentions_ = maxMentions;
}

void ContentScanner::addProfanityWords(const std::vector<std::string>& words) {
    std::lock_guard<std::mutex> lock(scannerMutex_);
    for (const auto& word : words) {
        if (!word.empty() && word.size() <= 64 && word.find('\0') == std::string::npos) {
            profanityWords_.insert(lowerCopy(word));
        }
    }
}

void ContentScanner::addAllowedDomains(const std::vector<std::string>& domains) {
    std::lock_guard<std::mutex> lock(scannerMutex_);
    for (const auto& domain : domains) {
        if (!domain.empty() && domain.size() <= 253) {
            allowedDomains_.insert(lowerCopy(domain));
        }
    }
}

void ContentScanner::addBlockedDomains(const std::vector<std::string>& domains) {
    std::lock_guard<std::mutex> lock(scannerMutex_);
    for (const auto& domain : domains) {
        if (!domain.empty() && domain.size() <= 253) {
            blockedDomains_.insert(lowerCopy(domain));
        }
    }
}

bool ContentScanner::detectProfanityLocked(const std::string& content) const {
    const std::string lower = lowerCopy(content);
    for (const auto& word : profanityWords_) {
        std::size_t position = lower.find(word);
        while (position != std::string::npos) {
            const bool leftBoundary = position == 0 ||
                std::isalnum(static_cast<unsigned char>(lower[position - 1])) == 0;
            const std::size_t right = position + word.size();
            const bool rightBoundary = right == lower.size() ||
                std::isalnum(static_cast<unsigned char>(lower[right])) == 0;
            if (leftBoundary && rightBoundary) {
                return true;
            }
            position = lower.find(word, position + 1);
        }
    }
    return false;
}

bool ContentScanner::detectSpamLocked(const DiscordMessage& message) const {
    std::size_t run = 1;
    for (std::size_t index = 1; index < message.content.size(); ++index) {
        run = message.content[index] == message.content[index - 1] ? run + 1 : 1;
        if (run > 5 && std::isspace(static_cast<unsigned char>(message.content[index])) == 0) {
            return true;
        }
    }
    const std::string lower = lowerCopy(message.content);
    static const std::vector<std::string> phrases{
        "free nitro", "click here to claim", "limited time offer", "win a prize"};
    return std::any_of(phrases.begin(), phrases.end(), [&lower](const std::string& phrase) {
        return lower.find(phrase) != std::string::npos;
    }) || (mentionSpamEnabled_ && detectMentionSpamLocked(message));
}

bool ContentScanner::detectPhishingLocked(const std::string& content) const {
    const auto urls = extractUrls(content);
    const std::string lower = lowerCopy(content);
    static const std::vector<std::string> phrases{
        "click here to claim", "free nitro", "discord gift", "steam gift",
        "verify your account"};
    const bool suspiciousPhrase = std::any_of(
        phrases.begin(), phrases.end(), [&lower](const std::string& phrase) {
            return lower.find(phrase) != std::string::npos;
        });
    if (suspiciousPhrase && !urls.empty()) {
        return true;
    }
    for (const auto& url : urls) {
        const auto scheme = url.find("://");
        const auto hostStart = scheme == std::string::npos ? 0 : scheme + 3;
        const auto hostEnd = url.find_first_of("/:?#", hostStart);
        const std::string host = lowerCopy(url.substr(hostStart, hostEnd - hostStart));
        const auto domainMatches = [&host](const std::string& domain) {
            return host == domain ||
                   (host.size() > domain.size() &&
                    host.compare(host.size() - domain.size(), domain.size(), domain) == 0 &&
                    host[host.size() - domain.size() - 1] == '.');
        };
        if (std::any_of(allowedDomains_.begin(), allowedDomains_.end(), domainMatches)) {
            continue;
        }
        if (std::any_of(blockedDomains_.begin(), blockedDomains_.end(), domainMatches)) {
            return true;
        }
    }
    return false;
}

bool ContentScanner::detectInviteLinksLocked(const std::string& content) const {
    static const std::regex invite(
        R"((https?://)?(www\.)?(discord\.gg|discord(app)?\.com/invite)/[A-Za-z0-9-]+)",
        std::regex::icase);
    return std::regex_search(content, invite);
}

bool ContentScanner::detectMentionSpamLocked(const DiscordMessage& message) const {
    return countMentions(message.content) > maxMentions_;
}

std::vector<std::string> ContentScanner::extractUrls(const std::string& content) {
    static const std::regex url(R"(https?://[^\s<>"']+)", std::regex::icase);
    std::vector<std::string> urls;
    for (std::sregex_iterator iterator(content.begin(), content.end(), url), end;
         iterator != end && urls.size() < 32; ++iterator) {
        urls.push_back(iterator->str());
    }
    return urls;
}

int ContentScanner::countMentions(const std::string& content) {
    static const std::regex mention(R"(<@!?[0-9]{17,20}>|@everyone|@here)");
    return static_cast<int>(std::distance(
        std::sregex_iterator(content.begin(), content.end(), mention),
        std::sregex_iterator()));
}

AutoModerator::AutoModerator()
    : clock_([] { return std::chrono::system_clock::now(); }) {}

AutoModerator::AutoModerator(std::shared_ptr<DiscordMutationAdapter> adapter)
    : mutationAdapter_(std::move(adapter)),
      clock_([] { return std::chrono::system_clock::now(); }) {}

AutoModerator::~AutoModerator() = default;

void AutoModerator::setMutationAdapter(
    std::shared_ptr<DiscordMutationAdapter> adapter) {
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    mutationAdapter_ = std::move(adapter);
}

void AutoModerator::setRetryPolicy(int maxAttempts,
                                   std::chrono::milliseconds maximumDelay) {
    if (maxAttempts < 1 || maxAttempts > 5 || maximumDelay.count() < 0 ||
        maximumDelay > std::chrono::seconds(5)) {
        return;
    }
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    maxAttempts_ = maxAttempts;
    maximumRetryDelay_ = maximumDelay;
}

void AutoModerator::setClock(Clock clock) {
    if (!clock) {
        return;
    }
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    clock_ = std::move(clock);
}

void AutoModerator::setActionObserver(ActionObserver observer) {
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    actionObserver_ = std::move(observer);
}

DiscordAcknowledgement AutoModerator::invokeMutation(
    const std::string& operation, const std::string& resourceId,
    const std::function<DiscordAcknowledgement(DiscordMutationAdapter&)>& call) {
    std::shared_ptr<DiscordMutationAdapter> adapter;
    int attempts = 1;
    std::chrono::milliseconds maximumDelay{0};
    {
        std::lock_guard<std::mutex> lock(moderatorMutex_);
        adapter = mutationAdapter_;
        attempts = maxAttempts_;
        maximumDelay = maximumRetryDelay_;
    }
    if (!adapter) {
        DiscordAcknowledgement result;
        result.status = DiscordOperationStatus::NO_ADAPTER;
        result.operation = operation;
        result.resourceId = resourceId;
        result.detail = "remote mutation requires a DiscordMutationAdapter";
        return result;
    }

    DiscordAcknowledgement result;
    for (int attempt = 1; attempt <= attempts; ++attempt) {
        try {
            result = call(*adapter);
        } catch (...) {
            result = {};
            result.status = DiscordOperationStatus::ADAPTER_ERROR;
            result.detail = "adapter threw an exception";
        }
        result.operation = operation;
        result.resourceId = resourceId;
        result.attempts = attempt;
        if (result.acknowledged() &&
            (result.receiptId.empty() || result.receiptId.size() > 256)) {
            result.status = DiscordOperationStatus::ADAPTER_ERROR;
            result.receiptId.clear();
            result.detail = "acknowledgement omitted a bounded receipt";
        }
        if (!result.retryable() || attempt == attempts) {
            return result;
        }
        const auto requested = std::max(result.retryAfter, std::chrono::milliseconds(0));
        std::this_thread::sleep_for(std::min(requested, maximumDelay));
    }
    return result;
}

ModerationAction AutoModerator::makeAction(FilterAction action,
                                           const std::string& guildId,
                                           const std::string& userId,
                                           const std::string& channelId,
                                           const std::string& messageId,
                                           const std::string& reason) {
    Clock clock;
    {
        std::lock_guard<std::mutex> lock(moderatorMutex_);
        clock = clock_;
    }
    ModerationAction result;
    std::ostringstream identifier;
    identifier << "action-" << std::setw(12) << std::setfill('0')
               << nextActionId_.fetch_add(1);
    result.id = identifier.str();
    result.guildId = guildId;
    result.userId = userId;
    result.channelId = channelId;
    result.messageId = messageId;
    result.action = action;
    result.reason = reason;
    result.timestamp = clock();
    return result;
}

void AutoModerator::commitAcknowledgedAction(ModerationAction action) {
    if (!action.acknowledgement.acknowledged()) {
        return;
    }
    ActionObserver observer;
    {
        std::lock_guard<std::mutex> lock(moderatorMutex_);
        actionHistory_[action.id] = action;
        if (!action.userId.empty()) {
            auto& reputation = userReputations_[action.userId];
            reputation.userId = action.userId;
            reputation.lastIncident = action.timestamp;
            reputation.violations.push_back(actionName(action.action) + ": " + action.reason);
            switch (action.action) {
                case FilterAction::WARN:
                    ++reputation.warningCount;
                    reputation.reputationScore = std::max(-1000, reputation.reputationScore - 5);
                    break;
                case FilterAction::TIMEOUT:
                    ++reputation.timeoutCount;
                    reputation.reputationScore = std::max(-1000, reputation.reputationScore - 15);
                    break;
                case FilterAction::KICK:
                    ++reputation.kickCount;
                    reputation.reputationScore = std::max(-1000, reputation.reputationScore - 25);
                    break;
                case FilterAction::BAN:
                    ++reputation.banCount;
                    reputation.reputationScore = std::max(-1000, reputation.reputationScore - 100);
                    break;
                case FilterAction::NONE:
                case FilterAction::DELETE:
                    break;
            }
        }
        observer = actionObserver_;
    }
    if (observer) {
        try {
            observer(action);
        } catch (...) {
            // Observer failures cannot change an already acknowledged transition.
        }
    }
}

AutoModerator::ModerationResult AutoModerator::processMessageAcknowledged(
    const DiscordMessage& message) {
    ModerationResult result;
    result.classification = scanner_.scanMessage(message);
    if (!result.classification.validInput) {
        result.acknowledgement = invalidResult("process_message", message.id,
                                              result.classification.reason);
        return result;
    }
    if (!result.classification.violation) {
        result.acknowledgement.status = DiscordOperationStatus::LOCAL_ONLY;
        result.acknowledgement.operation = "classify_message";
        result.acknowledgement.resourceId = message.id;
        result.acknowledgement.detail = "local classifiers found no violation";
        return result;
    }

    FilterAction action = FilterAction::NONE;
    {
        std::lock_guard<std::mutex> lock(moderatorMutex_);
        const auto iterator = userReputations_.find(message.authorId);
        const UserReputation reputation = iterator == userReputations_.end()
            ? UserReputation(message.authorId) : iterator->second;
        if (reputation.isTrusted) {
            result.acknowledgement.status = DiscordOperationStatus::LOCAL_ONLY;
            result.acknowledgement.operation = "classify_message";
            result.acknowledgement.resourceId = message.id;
            result.acknowledgement.detail = "trusted-user local exemption";
            return result;
        }
        if (isOnCooldownLocked(message.authorId, clock_())) {
            result.acknowledgement.status = DiscordOperationStatus::RATE_LIMITED;
            result.acknowledgement.operation = "moderate_message";
            result.acknowledgement.resourceId = message.id;
            result.acknowledgement.retryAfter =
                std::chrono::seconds(actionCooldownSeconds_);
            result.acknowledgement.detail = "local action cooldown active";
            return result;
        }
        action = determineAction(result.classification, reputation);
    }

    ModerationAction moderation = makeAction(
        action, message.guildId, message.authorId, message.channelId, message.id,
        result.classification.reason);
    result.acknowledgement = executeActionAcknowledged(moderation);
    moderation.acknowledgement = result.acknowledgement;
    result.action = moderation;
    return result;
}

AutoModerator::ModerationResult AutoModerator::processEditAcknowledged(
    const DiscordMessage& oldMessage, const DiscordMessage& newMessage) {
    if (oldMessage.id != newMessage.id || oldMessage.channelId != newMessage.channelId ||
        oldMessage.authorId != newMessage.authorId ||
        newMessage.timestamp < oldMessage.timestamp) {
        ModerationResult result;
        result.classification.validInput = false;
        result.classification.reason = "edited message changed immutable fields or timestamp order";
        result.acknowledgement = invalidResult("process_edit", newMessage.id,
                                              result.classification.reason);
        return result;
    }
    return processMessageAcknowledged(newMessage);
}

bool AutoModerator::processMessage(const DiscordMessage& message) {
    const auto result = processMessageAcknowledged(message);
    return result.classification.validInput &&
           (!result.classification.violation || result.acknowledgement.acknowledged());
}

bool AutoModerator::processEdit(const DiscordMessage& oldMessage,
                                const DiscordMessage& newMessage) {
    const auto result = processEditAcknowledged(oldMessage, newMessage);
    return result.classification.validInput &&
           (!result.classification.violation || result.acknowledgement.acknowledged());
}

bool AutoModerator::reviewUser(const std::string& userId) {
    if (!validDiscordId(userId)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    const auto iterator = userReputations_.find(userId);
    return iterator == userReputations_.end() || !shouldEscalate(iterator->second);
}

DiscordAcknowledgement AutoModerator::executeActionAcknowledged(
    const ModerationAction& requested) {
    ModerationAction action = requested;
    if (action.id.empty()) {
        action = makeAction(requested.action, requested.guildId, requested.userId,
                            requested.channelId, requested.messageId, requested.reason);
    }
    DiscordAcknowledgement acknowledgement;
    switch (action.action) {
        case FilterAction::NONE:
            acknowledgement.status = DiscordOperationStatus::LOCAL_ONLY;
            acknowledgement.operation = "none";
            acknowledgement.detail = "no remote mutation requested";
            break;
        case FilterAction::WARN:
            if (!validDiscordId(action.guildId) || !validDiscordId(action.userId) ||
                !validOptionalDiscordId(action.channelId) || !validReason(action.reason)) {
                acknowledgement = invalidResult("warn_user", action.userId,
                                                "invalid guild, user, channel, or reason");
                break;
            }
            acknowledgement = invokeMutation(
                "warn_user", action.userId, [&action](DiscordMutationAdapter& adapter) {
                    return adapter.warnUser(action.guildId, action.userId,
                                            action.channelId, action.reason);
                });
            break;
        case FilterAction::DELETE:
            if (!validDiscordId(action.channelId) || !validDiscordId(action.messageId) ||
                !validReason(action.reason, true)) {
                acknowledgement = invalidResult("delete_message", action.messageId,
                                                "invalid channel, message, or reason");
                break;
            }
            acknowledgement = invokeMutation(
                "delete_message", action.messageId,
                [&action](DiscordMutationAdapter& adapter) {
                    return adapter.deleteMessage(action.channelId, action.messageId,
                                                 action.reason);
                });
            break;
        case FilterAction::TIMEOUT:
            if (!validDiscordId(action.guildId) || !validDiscordId(action.userId) ||
                !validReason(action.reason)) {
                acknowledgement = invalidResult("timeout_user", action.userId,
                                                "invalid guild, user, or reason");
                break;
            }
            acknowledgement = invokeMutation(
                "timeout_user", action.userId, [&action](DiscordMutationAdapter& adapter) {
                    return adapter.timeoutUser(action.guildId, action.userId, 10,
                                               action.reason);
                });
            break;
        case FilterAction::KICK:
            if (!validDiscordId(action.guildId) || !validDiscordId(action.userId) ||
                !validReason(action.reason)) {
                acknowledgement = invalidResult("kick_user", action.userId,
                                                "invalid guild, user, or reason");
                break;
            }
            acknowledgement = invokeMutation(
                "kick_user", action.userId, [&action](DiscordMutationAdapter& adapter) {
                    return adapter.kickUser(action.guildId, action.userId, action.reason);
                });
            break;
        case FilterAction::BAN:
            if (!validDiscordId(action.guildId) || !validDiscordId(action.userId) ||
                !validReason(action.reason)) {
                acknowledgement = invalidResult("ban_user", action.userId,
                                                "invalid guild, user, or reason");
                break;
            }
            acknowledgement = invokeMutation(
                "ban_user", action.userId, [&action](DiscordMutationAdapter& adapter) {
                    return adapter.banUser(action.guildId, action.userId,
                                          action.reason, 0);
                });
            break;
    }
    action.acknowledgement = acknowledgement;
    if (acknowledgement.acknowledged()) {
        commitAcknowledgedAction(action);
    }
    return acknowledgement;
}

DiscordAcknowledgement AutoModerator::warnUserAcknowledged(
    const std::string& guildId, const std::string& userId,
    const std::string& reason, const std::string& channelId) {
    return executeActionAcknowledged(makeAction(FilterAction::WARN, guildId, userId,
                                                channelId, "", reason));
}

DiscordAcknowledgement AutoModerator::timeoutUserAcknowledged(
    const std::string& guildId, const std::string& userId, int minutes,
    const std::string& reason) {
    if (minutes < 1 || minutes > 40320 || !validDiscordId(guildId) ||
        !validDiscordId(userId) || !validReason(reason)) {
        return invalidResult("timeout_user", userId,
                             "invalid guild, user, duration, or reason");
    }
    ModerationAction action = makeAction(FilterAction::TIMEOUT, guildId, userId,
                                         "", "", reason);
    auto acknowledgement = invokeMutation(
        "timeout_user", userId, [&](DiscordMutationAdapter& adapter) {
            return adapter.timeoutUser(guildId, userId, minutes, reason);
        });
    action.acknowledgement = acknowledgement;
    if (acknowledgement.acknowledged()) {
        commitAcknowledgedAction(action);
    }
    return acknowledgement;
}

DiscordAcknowledgement AutoModerator::kickUserAcknowledged(
    const std::string& guildId, const std::string& userId,
    const std::string& reason) {
    return executeActionAcknowledged(makeAction(FilterAction::KICK, guildId, userId,
                                                "", "", reason));
}

DiscordAcknowledgement AutoModerator::banUserAcknowledged(
    const std::string& guildId, const std::string& userId,
    const std::string& reason, int deleteMessageDays) {
    if (deleteMessageDays < 0 || deleteMessageDays > 7 || !validDiscordId(guildId) ||
        !validDiscordId(userId) || !validReason(reason)) {
        return invalidResult("ban_user", userId,
                             "invalid guild, user, deletion window, or reason");
    }
    ModerationAction action = makeAction(FilterAction::BAN, guildId, userId,
                                         "", "", reason);
    auto acknowledgement = invokeMutation(
        "ban_user", userId, [&](DiscordMutationAdapter& adapter) {
            return adapter.banUser(guildId, userId, reason, deleteMessageDays);
        });
    action.acknowledgement = acknowledgement;
    if (acknowledgement.acknowledged()) {
        commitAcknowledgedAction(action);
    }
    return acknowledgement;
}

DiscordAcknowledgement AutoModerator::deleteMessageAcknowledged(
    const std::string& channelId, const std::string& messageId,
    const std::string& reason) {
    return executeActionAcknowledged(makeAction(FilterAction::DELETE, "", "",
                                                channelId, messageId, reason));
}

bool AutoModerator::executeAction(const ModerationAction& action) {
    return executeActionAcknowledged(action).acknowledged();
}

bool AutoModerator::warnUser(const std::string& userId, const std::string& reason,
                             const std::string& channelId) {
    std::string guild;
    {
        std::lock_guard<std::mutex> lock(moderatorMutex_);
        guild = guildContext_;
    }
    return warnUserAcknowledged(guild, userId, reason, channelId).acknowledged();
}

bool AutoModerator::timeoutUser(const std::string& userId, int minutes,
                                const std::string& reason) {
    std::string guild;
    {
        std::lock_guard<std::mutex> lock(moderatorMutex_);
        guild = guildContext_;
    }
    return timeoutUserAcknowledged(guild, userId, minutes, reason).acknowledged();
}

bool AutoModerator::kickUser(const std::string& userId, const std::string& reason) {
    std::string guild;
    {
        std::lock_guard<std::mutex> lock(moderatorMutex_);
        guild = guildContext_;
    }
    return kickUserAcknowledged(guild, userId, reason).acknowledged();
}

bool AutoModerator::banUser(const std::string& userId, const std::string& reason,
                            int deleteMessageDays) {
    std::string guild;
    {
        std::lock_guard<std::mutex> lock(moderatorMutex_);
        guild = guildContext_;
    }
    return banUserAcknowledged(guild, userId, reason,
                               deleteMessageDays).acknowledged();
}

bool AutoModerator::deleteMessage(const std::string& channelId,
                                  const std::string& messageId,
                                  const std::string& reason) {
    return deleteMessageAcknowledged(channelId, messageId, reason).acknowledged();
}

void AutoModerator::setGuildContext(const std::string& guildId) {
    if (!validDiscordId(guildId)) {
        return;
    }
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    guildContext_ = guildId;
}

void AutoModerator::updateUserReputation(const std::string& userId, int change,
                                         const std::string& reason) {
    if (!validDiscordId(userId) || change < -1000 || change > 1000 ||
        !validReason(reason, true)) {
        return;
    }
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    auto& reputation = userReputations_[userId];
    reputation.userId = userId;
    reputation.reputationScore =
        std::max(-1000, std::min(1000, reputation.reputationScore + change));
    if (!reason.empty()) {
        reputation.violations.push_back("local reputation note: " + reason);
    }
}

UserReputation AutoModerator::getUserReputation(const std::string& userId) {
    if (!validDiscordId(userId)) {
        return {};
    }
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    const auto iterator = userReputations_.find(userId);
    return iterator == userReputations_.end() ? UserReputation(userId)
                                              : iterator->second;
}

void AutoModerator::setTrustedUser(const std::string& userId, bool trusted) {
    if (!validDiscordId(userId)) {
        return;
    }
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    auto& reputation = userReputations_[userId];
    reputation.userId = userId;
    reputation.isTrusted = trusted;
}

void AutoModerator::setStrictMode(bool strict) {
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    strictMode_ = strict;
}

void AutoModerator::setAutoEscalation(bool enable) {
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    autoEscalation_ = enable;
}

void AutoModerator::setReputationThreshold(int threshold) {
    if (threshold < -1000 || threshold > 1000) {
        return;
    }
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    reputationThreshold_ = threshold;
}

void AutoModerator::setActionCooldown(int seconds) {
    if (seconds < 0 || seconds > 86400) {
        return;
    }
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    actionCooldownSeconds_ = seconds;
}

std::vector<ModerationAction> AutoModerator::getUserActions(
    const std::string& userId) {
    if (!validDiscordId(userId)) {
        return {};
    }
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    std::vector<ModerationAction> result;
    for (const auto& entry : actionHistory_) {
        if (entry.second.userId == userId) {
            result.push_back(entry.second);
        }
    }
    std::sort(result.begin(), result.end(), actionLess);
    return result;
}

std::vector<ModerationAction> AutoModerator::getChannelActions(
    const std::string& channelId) {
    if (!validDiscordId(channelId)) {
        return {};
    }
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    std::vector<ModerationAction> result;
    for (const auto& entry : actionHistory_) {
        if (entry.second.channelId == channelId) {
            result.push_back(entry.second);
        }
    }
    std::sort(result.begin(), result.end(), actionLess);
    return result;
}

std::vector<ModerationAction> AutoModerator::getRecentActions(int hours) {
    if (hours < 0 || hours > 24 * 365) {
        return {};
    }
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    const auto cutoff = clock_() - std::chrono::hours(hours);
    std::vector<ModerationAction> result;
    for (const auto& entry : actionHistory_) {
        if (entry.second.timestamp >= cutoff) {
            result.push_back(entry.second);
        }
    }
    std::sort(result.begin(), result.end(), actionLess);
    return result;
}

bool AutoModerator::submitAppeal(const std::string& actionId,
                                 const std::string& reason) {
    if (actionId.empty() || actionId.size() > 64 || !validReason(reason)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    const auto iterator = actionHistory_.find(actionId);
    if (iterator == actionHistory_.end() || iterator->second.appealed ||
        iterator->second.appealReviewed) {
        return false;
    }
    iterator->second.appealed = true;
    iterator->second.appealReason = reason;
    return true;
}

bool AutoModerator::reviewAppeal(const std::string& actionId, bool approved,
                                 const std::string& moderatorId) {
    if (actionId.empty() || actionId.size() > 64 || !validDiscordId(moderatorId)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    const auto iterator = actionHistory_.find(actionId);
    if (iterator == actionHistory_.end() || !iterator->second.appealed ||
        iterator->second.appealReviewed) {
        return false;
    }
    auto& action = iterator->second;
    action.moderatorId = moderatorId;
    action.appealed = false;
    action.appealReviewed = true;
    action.appealApproved = approved;
    if (approved && !action.userId.empty()) {
        auto& reputation = userReputations_[action.userId];
        int restoration = 0;
        switch (action.action) {
            case FilterAction::WARN:
                reputation.warningCount = std::max(0, reputation.warningCount - 1);
                restoration = 5;
                break;
            case FilterAction::TIMEOUT:
                reputation.timeoutCount = std::max(0, reputation.timeoutCount - 1);
                restoration = 15;
                break;
            case FilterAction::KICK:
                reputation.kickCount = std::max(0, reputation.kickCount - 1);
                restoration = 25;
                break;
            case FilterAction::BAN:
                reputation.banCount = std::max(0, reputation.banCount - 1);
                restoration = 100;
                break;
            case FilterAction::NONE:
            case FilterAction::DELETE:
                break;
        }
        reputation.reputationScore =
            std::min(1000, reputation.reputationScore + restoration);
    }
    return true;
}

std::vector<ModerationAction> AutoModerator::getPendingAppeals() {
    std::lock_guard<std::mutex> lock(moderatorMutex_);
    std::vector<ModerationAction> result;
    for (const auto& entry : actionHistory_) {
        if (entry.second.appealed && !entry.second.appealReviewed) {
            result.push_back(entry.second);
        }
    }
    std::sort(result.begin(), result.end(), actionLess);
    return result;
}

FilterAction AutoModerator::determineAction(
    const ContentScanner::ScanResult& scanResult,
    const UserReputation& reputation) const {
    FilterAction action = scanResult.recommendedAction;
    const auto escalate = [](FilterAction value) {
        const int next = std::min(static_cast<int>(FilterAction::BAN),
                                  static_cast<int>(value) + 1);
        return static_cast<FilterAction>(next);
    };
    if (autoEscalation_ && shouldEscalate(reputation)) {
        action = escalate(action);
    }
    if (strictMode_) {
        action = escalate(action);
    }
    return action;
}

bool AutoModerator::shouldEscalate(const UserReputation& reputation) const {
    return reputation.reputationScore < reputationThreshold_ ||
           reputation.warningCount >= 3 || reputation.timeoutCount >= 2;
}

bool AutoModerator::isOnCooldownLocked(
    const std::string& userId,
    std::chrono::system_clock::time_point now) const {
    if (actionCooldownSeconds_ == 0) {
        return false;
    }
    const auto cutoff = now - std::chrono::seconds(actionCooldownSeconds_);
    return std::any_of(actionHistory_.begin(), actionHistory_.end(),
                       [&](const auto& entry) {
                           return entry.second.userId == userId &&
                                  entry.second.timestamp >= cutoff;
                       });
}

} // namespace elizaos
