#include "elizaos/trust_scoreboard.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <initializer_list>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <system_error>

#if defined(_WIN32)
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace elizaos {
namespace {

using Json = nlohmann::json;
using Clock = std::chrono::system_clock;

constexpr int kPersistenceVersion = 1;
constexpr std::size_t kMaxIdLength = 256U;
constexpr std::size_t kMaxContextLength = 65536U;
constexpr std::size_t kMaxMetadataEntries = 1024U;
constexpr std::chrono::minutes kFutureTolerance{5};
constexpr double kScoreEpsilon = 1.0e-12;
constexpr std::uintmax_t kMaxPersistenceBytes = 64U * 1024U * 1024U;
constexpr std::size_t kMaxPersistedIdentifiers = 1000000U;

std::atomic<std::uint64_t> gEventSequence{0U};
std::atomic<std::uint64_t> gTemporarySequence{0U};

bool validType(TrustEventType type) noexcept {
    switch (type) {
        case TrustEventType::TASK_COMPLETED:
        case TrustEventType::TASK_FAILED:
        case TrustEventType::TASK_TIMEOUT:
        case TrustEventType::RESPONSE_FAST:
        case TrustEventType::RESPONSE_SLOW:
        case TrustEventType::COLLABORATION_GOOD:
        case TrustEventType::COLLABORATION_POOR:
        case TrustEventType::COMMUNICATION_CLEAR:
        case TrustEventType::COMMUNICATION_UNCLEAR:
        case TrustEventType::RULE_VIOLATION:
        case TrustEventType::HELPFUL_ACTION:
        case TrustEventType::HARMFUL_ACTION:
            return true;
    }
    return false;
}

bool validOutcome(TrustOutcome outcome) noexcept {
    switch (outcome) {
        case TrustOutcome::POSITIVE:
        case TrustOutcome::NEGATIVE:
        case TrustOutcome::NEUTRAL:
            return true;
    }
    return false;
}

TrustOutcome expectedOutcome(TrustEventType type) noexcept {
    switch (type) {
        case TrustEventType::TASK_COMPLETED:
        case TrustEventType::RESPONSE_FAST:
        case TrustEventType::COLLABORATION_GOOD:
        case TrustEventType::COMMUNICATION_CLEAR:
        case TrustEventType::HELPFUL_ACTION:
            return TrustOutcome::POSITIVE;
        case TrustEventType::TASK_FAILED:
        case TrustEventType::TASK_TIMEOUT:
        case TrustEventType::RESPONSE_SLOW:
        case TrustEventType::COLLABORATION_POOR:
        case TrustEventType::COMMUNICATION_UNCLEAR:
        case TrustEventType::RULE_VIOLATION:
        case TrustEventType::HARMFUL_ACTION:
            return TrustOutcome::NEGATIVE;
    }
    return TrustOutcome::NEUTRAL;
}

bool outcomeMatchesImpact(TrustOutcome outcome, double impact) noexcept {
    switch (outcome) {
        case TrustOutcome::POSITIVE:
            return impact > 0.0;
        case TrustOutcome::NEGATIVE:
            return impact < 0.0;
        case TrustOutcome::NEUTRAL:
            return impact == 0.0;
    }
    return false;
}

double clampScore(double value) noexcept {
    if (!std::isfinite(value)) {
        return 0.5;
    }
    return std::clamp(value, 0.0, 1.0);
}

std::int64_t toMillis(Clock::time_point value) {
    const auto count = std::chrono::duration_cast<std::chrono::milliseconds>(
        value.time_since_epoch()).count();
    return static_cast<std::int64_t>(count);
}

Clock::time_point fromMillis(std::int64_t value) {
    return Clock::time_point(std::chrono::milliseconds(value));
}

bool validText(const std::string& value, std::size_t maximum,
               bool requireNonWhitespace) noexcept {
    if (value.empty() || value.size() > maximum) {
        return false;
    }
    bool hasNonWhitespace = false;
    for (const char rawCharacter : value) {
        const auto character = static_cast<unsigned char>(rawCharacter);
        if (character == 0U || std::iscntrl(character) != 0) {
            return false;
        }
        hasNonWhitespace = hasNonWhitespace || std::isspace(character) == 0;
    }
    return !requireNonWhitespace || hasNonWhitespace;
}

std::string typeToken(TrustEventType type) {
    switch (type) {
        case TrustEventType::TASK_COMPLETED: return "task_completed";
        case TrustEventType::TASK_FAILED: return "task_failed";
        case TrustEventType::TASK_TIMEOUT: return "task_timeout";
        case TrustEventType::RESPONSE_FAST: return "response_fast";
        case TrustEventType::RESPONSE_SLOW: return "response_slow";
        case TrustEventType::COLLABORATION_GOOD: return "collaboration_good";
        case TrustEventType::COLLABORATION_POOR: return "collaboration_poor";
        case TrustEventType::COMMUNICATION_CLEAR: return "communication_clear";
        case TrustEventType::COMMUNICATION_UNCLEAR: return "communication_unclear";
        case TrustEventType::RULE_VIOLATION: return "rule_violation";
        case TrustEventType::HELPFUL_ACTION: return "helpful_action";
        case TrustEventType::HARMFUL_ACTION: return "harmful_action";
    }
    throw std::invalid_argument("invalid trust event type");
}

TrustEventType parseTypeToken(const std::string& token) {
    if (token == "task_completed") return TrustEventType::TASK_COMPLETED;
    if (token == "task_failed") return TrustEventType::TASK_FAILED;
    if (token == "task_timeout") return TrustEventType::TASK_TIMEOUT;
    if (token == "response_fast") return TrustEventType::RESPONSE_FAST;
    if (token == "response_slow") return TrustEventType::RESPONSE_SLOW;
    if (token == "collaboration_good") return TrustEventType::COLLABORATION_GOOD;
    if (token == "collaboration_poor") return TrustEventType::COLLABORATION_POOR;
    if (token == "communication_clear") return TrustEventType::COMMUNICATION_CLEAR;
    if (token == "communication_unclear") return TrustEventType::COMMUNICATION_UNCLEAR;
    if (token == "rule_violation") return TrustEventType::RULE_VIOLATION;
    if (token == "helpful_action") return TrustEventType::HELPFUL_ACTION;
    if (token == "harmful_action") return TrustEventType::HARMFUL_ACTION;
    throw std::invalid_argument("invalid trust event type token");
}

std::string outcomeToken(TrustOutcome outcome) {
    switch (outcome) {
        case TrustOutcome::POSITIVE: return "positive";
        case TrustOutcome::NEGATIVE: return "negative";
        case TrustOutcome::NEUTRAL: return "neutral";
    }
    throw std::invalid_argument("invalid trust outcome");
}

TrustOutcome parseOutcomeToken(const std::string& token) {
    if (token == "positive") return TrustOutcome::POSITIVE;
    if (token == "negative") return TrustOutcome::NEGATIVE;
    if (token == "neutral") return TrustOutcome::NEUTRAL;
    throw std::invalid_argument("invalid trust outcome token");
}

bool hasExactKeys(const Json& object,
                  std::initializer_list<const char*> keys) {
    if (!object.is_object() || object.size() != keys.size()) {
        return false;
    }
    for (const char* key : keys) {
        if (!object.contains(key)) {
            return false;
        }
    }
    return true;
}

std::uint64_t processIdentifier() noexcept {
#if defined(_WIN32)
    return static_cast<std::uint64_t>(GetCurrentProcessId());
#else
    return static_cast<std::uint64_t>(::getpid());
#endif
}

bool atomicWriteFile(const std::filesystem::path& path,
                     const std::string& contents) noexcept {
    const auto suffix = std::to_string(processIdentifier()) + "-" +
        std::to_string(gTemporarySequence.fetch_add(
            1U, std::memory_order_relaxed));
    const auto temporaryName = path.filename().string() + ".tmp-" + suffix;
#if defined(_WIN32)
    const auto temporary = path.parent_path() / temporaryName;
    HANDLE handle = CreateFileW(temporary.c_str(), GENERIC_WRITE, 0, nullptr,
                                CREATE_NEW,
                                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                                nullptr);
    if (handle == INVALID_HANDLE_VALUE) {
        return false;
    }
    bool success = true;
    std::size_t offset = 0U;
    while (offset < contents.size()) {
        const std::size_t remaining = contents.size() - offset;
        const DWORD chunk = static_cast<DWORD>(std::min<std::size_t>(
            remaining, static_cast<std::size_t>(MAXDWORD)));
        DWORD written = 0U;
        if (WriteFile(handle, contents.data() + offset, chunk, &written,
                      nullptr) == 0 || written != chunk) {
            success = false;
            break;
        }
        offset += static_cast<std::size_t>(written);
    }
    success = success && FlushFileBuffers(handle) != 0;
    success = CloseHandle(handle) != 0 && success;
    if (success) {
        success = MoveFileExW(temporary.c_str(), path.c_str(),
                              MOVEFILE_REPLACE_EXISTING |
                                  MOVEFILE_WRITE_THROUGH) != 0;
    }
    if (!success) {
        DeleteFileW(temporary.c_str());
    }
    return success;
#else
    const auto parentString = path.parent_path().string();
    const int directory = ::open(parentString.c_str(),
                                 O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
    if (directory < 0) {
        return false;
    }
    struct stat targetStatus {};
    if (::fstatat(directory, path.filename().c_str(), &targetStatus,
                  AT_SYMLINK_NOFOLLOW) == 0 &&
        !S_ISREG(targetStatus.st_mode)) {
        ::close(directory);
        return false;
    }
    const int file = ::openat(directory, temporaryName.c_str(),
                              O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC |
                                  O_NOFOLLOW,
                              S_IRUSR | S_IWUSR);
    if (file < 0) {
        ::close(directory);
        return false;
    }
    bool success = true;
    std::size_t offset = 0U;
    while (offset < contents.size()) {
        const auto written = ::write(file, contents.data() + offset,
                                     contents.size() - offset);
        if (written <= 0) {
            success = false;
            break;
        }
        offset += static_cast<std::size_t>(written);
    }
    success = success && ::fsync(file) == 0;
    success = ::close(file) == 0 && success;
    if (success) {
        success = ::renameat(directory, temporaryName.c_str(), directory,
                             path.filename().c_str()) == 0;
    }
    if (success) {
        success = ::fsync(directory) == 0;
    } else {
        ::unlinkat(directory, temporaryName.c_str(), 0);
    }
    ::close(directory);
    return success;
#endif
}

Json configToJson(const TrustConfig& config) {
    return Json{
        {"weights", {
            {"reliability", config.reliabilityWeight},
            {"responsiveness", config.responsivenessWeight},
            {"quality", config.qualityWeight},
            {"collaboration", config.collaborationWeight},
            {"communication", config.communicationWeight},
            {"compliance", config.complianceWeight}
        }},
        {"decay", {
            {"enabled", config.decay.enabled},
            {"interval_hours", config.decay.decayInterval.count()},
            {"rate", config.decay.decayRate},
            {"minimum_score", config.decay.minimumScore}
        }},
        {"anomaly_threshold", config.anomalyThreshold},
        {"anomaly_window_events", config.anomalyWindowEvents},
        {"min_events_for_confidence", config.minEventsForConfidence},
        {"max_events_for_confidence", config.maxEventsForConfidence},
        {"max_event_history_per_agent", config.maxEventHistoryPerAgent}
    };
}

TrustConfig configFromJson(const Json& json) {
    if (!json.is_object() || !json.contains("weights") ||
        !json.contains("decay")) {
        throw std::invalid_argument("missing configuration object");
    }
    const Json& weights = json.at("weights");
    const Json& decay = json.at("decay");
    TrustConfig config;
    config.reliabilityWeight = weights.at("reliability").get<double>();
    config.responsivenessWeight = weights.at("responsiveness").get<double>();
    config.qualityWeight = weights.at("quality").get<double>();
    config.collaborationWeight = weights.at("collaboration").get<double>();
    config.communicationWeight = weights.at("communication").get<double>();
    config.complianceWeight = weights.at("compliance").get<double>();
    config.decay.enabled = decay.at("enabled").get<bool>();
    config.decay.decayInterval = std::chrono::hours(
        decay.at("interval_hours").get<std::int64_t>());
    config.decay.decayRate = decay.at("rate").get<double>();
    config.decay.minimumScore = decay.at("minimum_score").get<double>();
    config.anomalyThreshold = json.at("anomaly_threshold").get<double>();
    config.anomalyWindowEvents = json.at("anomaly_window_events").get<int>();
    config.minEventsForConfidence =
        json.at("min_events_for_confidence").get<int>();
    config.maxEventsForConfidence =
        json.at("max_events_for_confidence").get<int>();
    config.maxEventHistoryPerAgent =
        json.at("max_event_history_per_agent").get<std::size_t>();
    return config;
}

Json eventToJson(const TrustEvent& event) {
    std::map<std::string, std::string> orderedMetadata(event.metadata.begin(),
                                                       event.metadata.end());
    return Json{
        {"event_id", event.eventId},
        {"agent_id", event.agentId},
        {"type", typeToken(event.type)},
        {"outcome", outcomeToken(event.outcome)},
        {"impact", event.impactScore},
        {"context", event.context},
        {"timestamp_ms", toMillis(event.timestamp)},
        {"metadata", orderedMetadata}
    };
}

TrustEvent eventFromJson(const Json& json) {
    TrustEvent event(json.at("agent_id").get<std::string>(),
                     parseTypeToken(json.at("type").get<std::string>()),
                     parseOutcomeToken(json.at("outcome").get<std::string>()),
                     json.at("impact").get<double>());
    event.eventId = json.at("event_id").get<std::string>();
    event.context = json.at("context").get<std::string>();
    event.timestamp = fromMillis(json.at("timestamp_ms").get<std::int64_t>());
    event.metadata = json.at("metadata").get<
        std::unordered_map<std::string, std::string>>();
    return event;
}

} // namespace

TrustScoreboard::TrustScoreboard(
    std::shared_ptr<AgentMemoryManager> memoryMgr, const TrustConfig& config,
    const std::filesystem::path& persistencePath)
    : memoryMgr_(std::move(memoryMgr)), config_(config),
      lastDecayTime_(Clock::now()) {
    if (!normalizeAndValidateConfig(config_)) {
        throw std::invalid_argument("invalid trust configuration");
    }
    if (!persistencePath.empty() && !setPersistencePath(persistencePath)) {
        throw std::invalid_argument("unsafe trust persistence path");
    }
}

bool TrustScoreboard::recordEvent(const TrustEvent& evidence) {
    const auto now = Clock::now();
    if (!validateEvent(evidence, now)) {
        return false;
    }

    std::lock_guard<std::mutex> lock(stateMutex_);
    if (seenEventIds_.find(evidence.eventId) != seenEventIds_.end()) {
        return false;
    }

    auto& history = eventHistory_[evidence.agentId];
    history.push_back(evidence);
    std::sort(history.begin(), history.end(), eventLess);
    seenEventIds_.insert(evidence.eventId);

    while (history.size() > config_.maxEventHistoryPerAgent) {
        history.erase(history.begin());
    }
    rebuildAgentLocked(evidence.agentId, now);
    return true;
}

bool TrustScoreboard::recordEvent(const AgentId& agentId, TrustEventType type,
                                  TrustOutcome outcome, double impactScore,
                                  const std::string& context) {
    if (impactScore == 0.0 && outcome != TrustOutcome::NEUTRAL) {
        impactScore = calculateImpactScore(type, outcome);
    }
    TrustEvent event(agentId, type, outcome, impactScore);
    event.context = context;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        event.eventId = generateEventIdLocked();
    }
    return recordEvent(event);
}

bool TrustScoreboard::recordTaskCompletion(
    const AgentId& agentId, bool success,
    std::chrono::milliseconds responseTime) {
    if (responseTime.count() < 0) {
        return false;
    }
    const TrustEventType type = success ? TrustEventType::TASK_COMPLETED
                                        : TrustEventType::TASK_FAILED;
    const TrustOutcome outcome = success ? TrustOutcome::POSITIVE
                                         : TrustOutcome::NEGATIVE;
    double impact = calculateImpactScore(type, outcome);
    if (success && responseTime < std::chrono::milliseconds(5000)) {
        impact = std::min(1.0, impact * 1.2);
    }
    std::ostringstream context;
    context << "Response time: " << responseTime.count() << "ms";
    return recordEvent(agentId, type, outcome, impact, context.str());
}

bool TrustScoreboard::recordCollaboration(const AgentId& agentId,
                                          bool positive) {
    return recordEvent(
        agentId, positive ? TrustEventType::COLLABORATION_GOOD
                          : TrustEventType::COLLABORATION_POOR,
        positive ? TrustOutcome::POSITIVE : TrustOutcome::NEGATIVE);
}

bool TrustScoreboard::recordCommunication(const AgentId& agentId, bool clear) {
    return recordEvent(agentId,
                       clear ? TrustEventType::COMMUNICATION_CLEAR
                             : TrustEventType::COMMUNICATION_UNCLEAR,
                       clear ? TrustOutcome::POSITIVE
                             : TrustOutcome::NEGATIVE);
}

bool TrustScoreboard::recordRuleViolation(const AgentId& agentId,
                                          const std::string& violation) {
    if (!validText(violation, kMaxContextLength, true)) {
        return false;
    }
    return recordEvent(agentId, TrustEventType::RULE_VIOLATION,
                       TrustOutcome::NEGATIVE, -0.5, violation);
}

TrustScore TrustScoreboard::getTrustScore(const AgentId& agentId) const {
    const auto now = Clock::now();
    std::lock_guard<std::mutex> lock(stateMutex_);
    return scoreSnapshotLocked(agentId, now);
}

double TrustScoreboard::getOverallScore(const AgentId& agentId) const {
    return getTrustScore(agentId).overallScore;
}

std::vector<TrustEvent> TrustScoreboard::getEventHistory(
    const AgentId& agentId, int limit) const {
    if (!validateAgentId(agentId) || limit <= 0) {
        return {};
    }
    std::lock_guard<std::mutex> lock(stateMutex_);
    const auto found = eventHistory_.find(agentId);
    if (found == eventHistory_.end()) {
        return {};
    }
    const auto& events = found->second;
    const std::size_t requested = static_cast<std::size_t>(limit);
    const auto begin = requested >= events.size()
        ? events.begin()
        : events.end() - static_cast<std::ptrdiff_t>(requested);
    return {begin, events.end()};
}

std::vector<TrustScore> TrustScoreboard::getLeaderboard(int limit) const {
    if (limit <= 0) {
        return {};
    }
    const auto now = Clock::now();
    std::vector<TrustScore> result;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        result.reserve(scores_.size());
        for (const auto& entry : scores_) {
            result.push_back(scoreSnapshotLocked(entry.first, now));
        }
    }
    std::sort(result.begin(), result.end(), [](const TrustScore& lhs,
                                               const TrustScore& rhs) {
        if (std::abs(lhs.overallScore - rhs.overallScore) > kScoreEpsilon) {
            return lhs.overallScore > rhs.overallScore;
        }
        return lhs.agentId < rhs.agentId;
    });
    const std::size_t requested = static_cast<std::size_t>(limit);
    if (result.size() > requested) {
        result.resize(requested);
    }
    return result;
}

std::vector<TrustScore> TrustScoreboard::getTopAgents(int limit) const {
    return getLeaderboard(limit);
}

std::vector<TrustScore> TrustScoreboard::getBottomAgents(int limit) const {
    if (limit <= 0) {
        return {};
    }
    const auto now = Clock::now();
    std::vector<TrustScore> result;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        result.reserve(scores_.size());
        for (const auto& entry : scores_) {
            result.push_back(scoreSnapshotLocked(entry.first, now));
        }
    }
    std::sort(result.begin(), result.end(), [](const TrustScore& lhs,
                                               const TrustScore& rhs) {
        if (std::abs(lhs.overallScore - rhs.overallScore) > kScoreEpsilon) {
            return lhs.overallScore < rhs.overallScore;
        }
        return lhs.agentId < rhs.agentId;
    });
    const std::size_t requested = static_cast<std::size_t>(limit);
    if (result.size() > requested) {
        result.resize(requested);
    }
    return result;
}

TrustComparison TrustScoreboard::compareAgents(const AgentId& agent1,
                                               const AgentId& agent2) const {
    TrustComparison result;
    result.agent1 = agent1;
    result.agent2 = agent2;
    if (!validateAgentId(agent1) || !validateAgentId(agent2)) {
        result.recommendation = "Invalid agent identifier";
        return result;
    }

    const auto now = Clock::now();
    TrustScore first;
    TrustScore second;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        if (scores_.find(agent1) == scores_.end() ||
            scores_.find(agent2) == scores_.end()) {
            result.recommendation = "Insufficient evidence";
            return result;
        }
        first = scoreSnapshotLocked(agent1, now);
        second = scoreSnapshotLocked(agent2, now);
    }
    result.scoreDifference = first.overallScore - second.overallScore;
    if (std::abs(result.scoreDifference) <= kScoreEpsilon) {
        result.scoreDifference = 0.0;
        result.recommendation = "Agents are tied";
        return result;
    }

    const bool firstWins = result.scoreDifference > 0.0;
    result.recommendation = (firstWins ? agent1 : agent2) + " is more trusted";
    const TrustScore& winner = firstWins ? first : second;
    const TrustScore& loser = firstWins ? second : first;
    if (winner.reliabilityScore > loser.reliabilityScore + kScoreEpsilon) {
        result.reasons.emplace_back("Higher reliability");
    }
    if (winner.responsivenessScore >
        loser.responsivenessScore + kScoreEpsilon) {
        result.reasons.emplace_back("Higher responsiveness");
    }
    if (winner.qualityScore > loser.qualityScore + kScoreEpsilon) {
        result.reasons.emplace_back("Higher quality");
    }
    if (winner.collaborationScore >
        loser.collaborationScore + kScoreEpsilon) {
        result.reasons.emplace_back("Better collaboration");
    }
    if (winner.communicationScore >
        loser.communicationScore + kScoreEpsilon) {
        result.reasons.emplace_back("Clearer communication");
    }
    if (winner.complianceScore > loser.complianceScore + kScoreEpsilon) {
        result.reasons.emplace_back("Higher compliance");
    }
    return result;
}

AgentId TrustScoreboard::selectMostTrusted(
    const std::vector<AgentId>& candidates) const {
    const auto ranked = rankByTrust(candidates);
    return ranked.empty() ? AgentId{} : ranked.front();
}

std::vector<AgentId> TrustScoreboard::rankByTrust(
    const std::vector<AgentId>& agents) const {
    const auto now = Clock::now();
    std::set<AgentId> unique;
    for (const auto& agent : agents) {
        if (validateAgentId(agent)) {
            unique.insert(agent);
        }
    }
    std::vector<std::pair<AgentId, double>> scores;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        scores.reserve(unique.size());
        for (const auto& agent : unique) {
            if (scores_.find(agent) != scores_.end()) {
                scores.emplace_back(agent,
                    scoreSnapshotLocked(agent, now).overallScore);
            }
        }
    }
    std::sort(scores.begin(), scores.end(), [](const auto& lhs,
                                               const auto& rhs) {
        if (std::abs(lhs.second - rhs.second) > kScoreEpsilon) {
            return lhs.second > rhs.second;
        }
        return lhs.first < rhs.first;
    });
    std::vector<AgentId> result;
    result.reserve(scores.size());
    for (const auto& entry : scores) {
        result.push_back(entry.first);
    }
    return result;
}

std::vector<TrustAnomaly> TrustScoreboard::detectAnomalies() const {
    std::vector<AgentId> agents;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        agents.reserve(eventHistory_.size());
        for (const auto& entry : eventHistory_) {
            agents.push_back(entry.first);
        }
    }
    std::sort(agents.begin(), agents.end());
    std::vector<TrustAnomaly> result;
    for (const auto& agent : agents) {
        auto anomalies = detectAnomaliesForAgent(agent);
        result.insert(result.end(), anomalies.begin(), anomalies.end());
    }
    return result;
}

std::vector<TrustAnomaly> TrustScoreboard::detectAnomaliesForAgent(
    const AgentId& agentId) const {
    if (!validateAgentId(agentId)) {
        return {};
    }
    TrustConfig config;
    std::vector<TrustEvent> recent;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        config = config_;
        const auto found = eventHistory_.find(agentId);
        if (found == eventHistory_.end()) {
            return {};
        }
        const auto& all = found->second;
        const std::size_t window = static_cast<std::size_t>(
            config.anomalyWindowEvents);
        const auto begin = window >= all.size()
            ? all.begin()
            : all.end() - static_cast<std::ptrdiff_t>(window);
        recent.assign(begin, all.end());
    }
    if (recent.size() < 5U) {
        return {};
    }

    const auto detectedAt = Clock::now();
    std::vector<TrustAnomaly> result;
    std::vector<std::string> negativeEvidence;
    std::vector<std::string> violationEvidence;
    for (const auto& event : recent) {
        if (event.outcome == TrustOutcome::NEGATIVE) {
            negativeEvidence.push_back(event.eventId);
        }
        if (event.type == TrustEventType::RULE_VIOLATION) {
            violationEvidence.push_back(event.eventId);
        }
    }
    const double negativeRatio = static_cast<double>(negativeEvidence.size()) /
                                 static_cast<double>(recent.size());
    if (negativeRatio >= config.anomalyThreshold) {
        TrustAnomaly anomaly;
        anomaly.agentId = agentId;
        anomaly.anomalyType = "high_negative_event_rate";
        anomaly.severity = clampScore(negativeRatio);
        anomaly.description = "Recent negative-event ratio reached the configured threshold";
        anomaly.detectedAt = detectedAt;
        anomaly.evidence = std::move(negativeEvidence);
        result.push_back(std::move(anomaly));
    }
    if (!violationEvidence.empty()) {
        TrustAnomaly anomaly;
        anomaly.agentId = agentId;
        anomaly.anomalyType = "rule_violations";
        anomaly.severity = std::min(
            1.0, static_cast<double>(violationEvidence.size()) /
                 static_cast<double>(recent.size()));
        anomaly.description = "Recent evidence includes rule violations";
        anomaly.detectedAt = detectedAt;
        anomaly.evidence = std::move(violationEvidence);
        result.push_back(std::move(anomaly));
    }
    return result;
}

bool TrustScoreboard::isAnomalous(const AgentId& agentId) const {
    return !detectAnomaliesForAgent(agentId).empty();
}

void TrustScoreboard::applyDecay() {
    const auto now = Clock::now();
    std::lock_guard<std::mutex> lock(stateMutex_);
    if (!config_.decay.enabled ||
        now - lastDecayTime_ < config_.decay.decayInterval) {
        return;
    }
    for (auto& entry : scores_) {
        auto& score = entry.second;
        const auto asOf = scoreAsOf_.find(entry.first);
        const auto baseline = asOf == scoreAsOf_.end()
            ? score.lastUpdated : asOf->second;
        score.overallScore = applyTimeDecay(
            score.overallScore, baseline, config_, now);
        scoreAsOf_[entry.first] = now;
    }
    lastDecayTime_ = now;
}

void TrustScoreboard::applyDecayForAgent(const AgentId& agentId) {
    if (!validateAgentId(agentId)) {
        return;
    }
    const auto now = Clock::now();
    std::lock_guard<std::mutex> lock(stateMutex_);
    if (!config_.decay.enabled) {
        return;
    }
    const auto found = scores_.find(agentId);
    if (found == scores_.end()) {
        return;
    }
    const auto asOf = scoreAsOf_.find(agentId);
    const auto baseline = asOf == scoreAsOf_.end()
        ? found->second.lastUpdated : asOf->second;
    found->second.overallScore = applyTimeDecay(
        found->second.overallScore, baseline, config_, now);
    scoreAsOf_[agentId] = now;
}

int TrustScoreboard::getTotalAgents() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    const std::size_t count = scores_.size();
    if (count > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        return std::numeric_limits<int>::max();
    }
    return static_cast<int>(count);
}

int TrustScoreboard::getTotalEvents() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    std::size_t total = 0U;
    for (const auto& entry : eventHistory_) {
        if (entry.second.size() >
            static_cast<std::size_t>(std::numeric_limits<int>::max()) - total) {
            return std::numeric_limits<int>::max();
        }
        total += entry.second.size();
    }
    return static_cast<int>(total);
}

double TrustScoreboard::getAverageTrustScore() const {
    const auto now = Clock::now();
    std::lock_guard<std::mutex> lock(stateMutex_);
    if (scores_.empty()) {
        return 0.5;
    }
    long double sum = 0.0L;
    for (const auto& entry : scores_) {
        sum += static_cast<long double>(
            scoreSnapshotLocked(entry.first, now).overallScore);
    }
    const long double average = sum /
        static_cast<long double>(scores_.size());
    return clampScore(static_cast<double>(average));
}

bool TrustScoreboard::updateConfig(const TrustConfig& newConfig) {
    TrustConfig normalized = newConfig;
    if (!normalizeAndValidateConfig(normalized)) {
        return false;
    }
    const auto now = Clock::now();
    std::lock_guard<std::mutex> lock(stateMutex_);
    config_ = normalized;
    for (auto& entry : eventHistory_) {
        auto& history = entry.second;
        while (history.size() > config_.maxEventHistoryPerAgent) {
            history.erase(history.begin());
        }
        rebuildAgentLocked(entry.first, now);
    }
    return true;
}

TrustConfig TrustScoreboard::getConfig() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return config_;
}

bool TrustScoreboard::setPersistencePath(
    const std::filesystem::path& path) {
    if (!isSafePersistencePath(path)) {
        return false;
    }
    const auto normalized = std::filesystem::absolute(path).lexically_normal();
    std::lock_guard<std::mutex> persistenceLock(persistenceMutex_);
    persistencePath_ = normalized;
    return true;
}

std::filesystem::path TrustScoreboard::getPersistencePath() const {
    std::lock_guard<std::mutex> lock(persistenceMutex_);
    return persistencePath_;
}

bool TrustScoreboard::saveTrustData() const {
    std::lock_guard<std::mutex> persistenceLock(persistenceMutex_);
    const auto path = persistencePath_;
    if (!isSafePersistencePath(path)) {
        return false;
    }

    TrustConfig config;
    std::vector<TrustEvent> events;
    std::vector<std::string> seenIdentifiers;
    Clock::time_point lastDecay;
    {
        std::lock_guard<std::mutex> stateLock(stateMutex_);
        config = config_;
        lastDecay = lastDecayTime_;
        events.reserve(seenEventIds_.size());
        for (const auto& entry : eventHistory_) {
            events.insert(events.end(), entry.second.begin(), entry.second.end());
        }
        seenIdentifiers.assign(seenEventIds_.begin(), seenEventIds_.end());
    }
    std::sort(events.begin(), events.end(), eventLess);
    std::sort(seenIdentifiers.begin(), seenIdentifiers.end());

    Json document = {
        {"schema", "elizaos.trust_scoreboard"},
        {"version", kPersistenceVersion},
        {"config", configToJson(config)},
        {"last_decay_ms", toMillis(lastDecay)},
        {"events", Json::array()},
        {"seen_event_ids", seenIdentifiers}
    };
    for (const auto& event : events) {
        document["events"].push_back(eventToJson(event));
    }
    const std::string encoded = document.dump(2) + '\n';
    if (encoded.size() > kMaxPersistenceBytes) {
        return false;
    }
    return atomicWriteFile(path, encoded);
}

bool TrustScoreboard::loadTrustData() {
    std::lock_guard<std::mutex> persistenceLock(persistenceMutex_);
    const auto path = persistencePath_;
    if (!isSafePersistencePath(path)) {
        return false;
    }

    try {
        std::error_code sizeError;
        const auto fileSize = std::filesystem::file_size(path, sizeError);
        if (sizeError || fileSize > kMaxPersistenceBytes) {
            return false;
        }
        std::ifstream input(path, std::ios::binary);
        if (!input) {
            return false;
        }
        Json document;
        input >> document;
        if (!input.eof()) {
            input >> std::ws;
            if (!input.eof()) {
                return false;
            }
        }
        if (!hasExactKeys(document, {"schema", "version", "config",
                                     "last_decay_ms", "events",
                                     "seen_event_ids"}) ||
            document.at("schema").get<std::string>() !=
                "elizaos.trust_scoreboard" ||
            document.at("version").get<int>() != kPersistenceVersion ||
            !document.at("events").is_array() ||
            !document.at("seen_event_ids").is_array()) {
            return false;
        }

        TrustConfig loadedConfig = configFromJson(document.at("config"));
        if (!normalizeAndValidateConfig(loadedConfig)) {
            return false;
        }
        const auto loadedLastDecay = fromMillis(
            document.at("last_decay_ms").get<std::int64_t>());
        const auto now = Clock::now();
        if (loadedLastDecay.time_since_epoch().count() < 0 ||
            loadedLastDecay > now + kFutureTolerance) {
            return false;
        }

        std::unordered_map<AgentId, std::vector<TrustEvent>> loadedHistory;
        std::unordered_set<std::string> loadedIds;
        if (document.at("seen_event_ids").size() > kMaxPersistedIdentifiers) {
            return false;
        }
        for (const auto& encodedId : document.at("seen_event_ids")) {
            const auto identifier = encodedId.get<std::string>();
            if (!validateEventId(identifier) ||
                !loadedIds.insert(identifier).second) {
                return false;
            }
        }
        std::unordered_set<std::string> eventIds;
        for (const auto& encoded : document.at("events")) {
            if (!hasExactKeys(encoded, {"event_id", "agent_id", "type",
                                        "outcome", "impact", "context",
                                        "timestamp_ms", "metadata"})) {
                return false;
            }
            TrustEvent event = eventFromJson(encoded);
            if (!validateEvent(event, now) ||
                loadedIds.find(event.eventId) == loadedIds.end() ||
                !eventIds.insert(event.eventId).second) {
                return false;
            }
            auto& history = loadedHistory[event.agentId];
            history.push_back(std::move(event));
            if (history.size() > loadedConfig.maxEventHistoryPerAgent) {
                return false;
            }
        }
        for (auto& entry : loadedHistory) {
            std::sort(entry.second.begin(), entry.second.end(), eventLess);
        }

        std::unordered_map<AgentId, TrustScore> loadedScores;
        std::unordered_map<AgentId, Clock::time_point> loadedAsOf;
        for (const auto& entry : loadedHistory) {
            TrustScore score(entry.first);
            calculateScore(score, entry.second, loadedConfig);
            loadedScores.emplace(entry.first, score);
            loadedAsOf.emplace(entry.first, score.lastUpdated);
        }

        std::lock_guard<std::mutex> stateLock(stateMutex_);
        config_ = loadedConfig;
        eventHistory_.swap(loadedHistory);
        seenEventIds_.swap(loadedIds);
        scores_.swap(loadedScores);
        scoreAsOf_.swap(loadedAsOf);
        lastDecayTime_ = loadedLastDecay;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool TrustScoreboard::normalizeAndValidateConfig(TrustConfig& config) noexcept {
    const std::array<double*, 6U> weights{
        &config.reliabilityWeight, &config.responsivenessWeight,
        &config.qualityWeight, &config.collaborationWeight,
        &config.communicationWeight, &config.complianceWeight};
    long double sum = 0.0L;
    for (const double* weight : weights) {
        if (!std::isfinite(*weight) || *weight < 0.0) {
            return false;
        }
        sum += static_cast<long double>(*weight);
    }
    if (!std::isfinite(static_cast<double>(sum)) || sum <= 0.0L) {
        return false;
    }
    for (double* weight : weights) {
        *weight = static_cast<double>(static_cast<long double>(*weight) / sum);
    }
    return config.decay.decayInterval.count() > 0 &&
           std::isfinite(config.decay.decayRate) &&
           config.decay.decayRate >= 0.0 && config.decay.decayRate <= 1.0 &&
           std::isfinite(config.decay.minimumScore) &&
           config.decay.minimumScore >= 0.0 &&
           config.decay.minimumScore <= 1.0 &&
           std::isfinite(config.anomalyThreshold) &&
           config.anomalyThreshold > 0.0 && config.anomalyThreshold <= 1.0 &&
           config.anomalyWindowEvents >= 5 &&
           config.minEventsForConfidence > 0 &&
           config.maxEventsForConfidence >= config.minEventsForConfidence &&
           config.maxEventHistoryPerAgent > 0U &&
           config.maxEventHistoryPerAgent <= 100000U;
}

bool TrustScoreboard::validateAgentId(const AgentId& agentId) noexcept {
    return validText(agentId, kMaxIdLength, true);
}

bool TrustScoreboard::validateEventId(const std::string& eventId) noexcept {
    if (eventId.empty() || eventId.size() > kMaxIdLength) {
        return false;
    }
    for (const char rawCharacter : eventId) {
        const auto character = static_cast<unsigned char>(rawCharacter);
        if (std::isalnum(character) == 0 && character != '-' &&
            character != '_' && character != '.' && character != ':') {
            return false;
        }
    }
    return true;
}

bool TrustScoreboard::validateEvent(
    const TrustEvent& event, Clock::time_point now) noexcept {
    if (!validateEventId(event.eventId) || !validateAgentId(event.agentId) ||
        !validType(event.type) || !validOutcome(event.outcome) ||
        (event.outcome != TrustOutcome::NEUTRAL &&
         event.outcome != expectedOutcome(event.type)) ||
        !std::isfinite(event.impactScore) ||
        event.impactScore < -1.0 || event.impactScore > 1.0 ||
        !outcomeMatchesImpact(event.outcome, event.impactScore) ||
        event.context.size() > kMaxContextLength ||
        event.timestamp.time_since_epoch().count() < 0 ||
        event.timestamp > now + kFutureTolerance ||
        event.metadata.size() > kMaxMetadataEntries) {
        return false;
    }
    for (const auto& entry : event.metadata) {
        if (!validText(entry.first, kMaxIdLength, true) ||
            entry.second.size() > kMaxContextLength) {
            return false;
        }
    }
    return true;
}

bool TrustScoreboard::eventLess(const TrustEvent& lhs,
                                const TrustEvent& rhs) noexcept {
    if (lhs.timestamp != rhs.timestamp) {
        return lhs.timestamp < rhs.timestamp;
    }
    if (lhs.agentId != rhs.agentId) {
        return lhs.agentId < rhs.agentId;
    }
    return lhs.eventId < rhs.eventId;
}

void TrustScoreboard::calculateScore(TrustScore& score,
                                     const std::vector<TrustEvent>& events,
                                     const TrustConfig& config) {
    score = TrustScore(score.agentId);
    if (events.empty()) {
        return;
    }
    score.firstSeen = events.front().timestamp;
    score.lastUpdated = events.back().timestamp;
    for (const auto& event : events) {
        updateComponentScores(score, event);
        ++score.totalEvents;
        if (event.outcome == TrustOutcome::POSITIVE) {
            ++score.positiveEvents;
        } else if (event.outcome == TrustOutcome::NEGATIVE) {
            ++score.negativeEvents;
        } else {
            ++score.neutralEvents;
        }
    }
    const long double weighted =
        static_cast<long double>(score.reliabilityScore) * config.reliabilityWeight +
        static_cast<long double>(score.responsivenessScore) * config.responsivenessWeight +
        static_cast<long double>(score.qualityScore) * config.qualityWeight +
        static_cast<long double>(score.collaborationScore) * config.collaborationWeight +
        static_cast<long double>(score.communicationScore) * config.communicationWeight +
        static_cast<long double>(score.complianceScore) * config.complianceWeight;
    score.overallScore = clampScore(static_cast<double>(weighted));
    score.confidence = calculateConfidence(score.totalEvents, config);
}

void TrustScoreboard::updateComponentScores(TrustScore& score,
                                             const TrustEvent& event) noexcept {
    const double adjustment = event.impactScore * 0.1;
    switch (event.type) {
        case TrustEventType::TASK_COMPLETED:
        case TrustEventType::TASK_FAILED:
        case TrustEventType::TASK_TIMEOUT:
            score.reliabilityScore = clampScore(score.reliabilityScore + adjustment);
            break;
        case TrustEventType::RESPONSE_FAST:
        case TrustEventType::RESPONSE_SLOW:
            score.responsivenessScore = clampScore(score.responsivenessScore + adjustment);
            break;
        case TrustEventType::COLLABORATION_GOOD:
        case TrustEventType::COLLABORATION_POOR:
            score.collaborationScore = clampScore(score.collaborationScore + adjustment);
            break;
        case TrustEventType::COMMUNICATION_CLEAR:
        case TrustEventType::COMMUNICATION_UNCLEAR:
            score.communicationScore = clampScore(score.communicationScore + adjustment);
            break;
        case TrustEventType::RULE_VIOLATION:
            score.complianceScore = clampScore(score.complianceScore + adjustment);
            break;
        case TrustEventType::HELPFUL_ACTION:
        case TrustEventType::HARMFUL_ACTION:
            score.qualityScore = clampScore(score.qualityScore + adjustment);
            break;
    }
}

double TrustScoreboard::calculateConfidence(
    int eventCount, const TrustConfig& config) noexcept {
    if (eventCount <= 0) {
        return 0.0;
    }
    if (eventCount < config.minEventsForConfidence) {
        return 0.5 * static_cast<double>(eventCount) /
               static_cast<double>(config.minEventsForConfidence);
    }
    if (config.maxEventsForConfidence == config.minEventsForConfidence ||
        eventCount >= config.maxEventsForConfidence) {
        return 1.0;
    }
    return 0.5 + 0.5 *
        static_cast<double>(eventCount - config.minEventsForConfidence) /
        static_cast<double>(config.maxEventsForConfidence -
                            config.minEventsForConfidence);
}

double TrustScoreboard::applyTimeDecay(
    double score, Clock::time_point lastUpdate, const TrustConfig& config,
    Clock::time_point now) noexcept {
    score = clampScore(score);
    if (!config.decay.enabled || now <= lastUpdate ||
        config.decay.decayInterval.count() <= 0) {
        return score;
    }
    const auto elapsed = now - lastUpdate;
    const auto intervals = elapsed / config.decay.decayInterval;
    if (intervals <= 0) {
        return score;
    }
    const double multiplier = std::pow(
        1.0 - config.decay.decayRate, static_cast<double>(intervals));
    if (!std::isfinite(multiplier)) {
        return config.decay.minimumScore;
    }
    return std::clamp(score * multiplier,
                      config.decay.minimumScore, 1.0);
}

double TrustScoreboard::calculateImpactScore(
    TrustEventType type, TrustOutcome outcome) noexcept {
    double magnitude = 0.0;
    switch (type) {
        case TrustEventType::TASK_COMPLETED: magnitude = 0.2; break;
        case TrustEventType::TASK_FAILED: magnitude = 0.3; break;
        case TrustEventType::TASK_TIMEOUT: magnitude = 0.4; break;
        case TrustEventType::RESPONSE_FAST: magnitude = 0.1; break;
        case TrustEventType::RESPONSE_SLOW: magnitude = 0.1; break;
        case TrustEventType::COLLABORATION_GOOD: magnitude = 0.15; break;
        case TrustEventType::COLLABORATION_POOR: magnitude = 0.2; break;
        case TrustEventType::COMMUNICATION_CLEAR: magnitude = 0.1; break;
        case TrustEventType::COMMUNICATION_UNCLEAR: magnitude = 0.1; break;
        case TrustEventType::RULE_VIOLATION: magnitude = 0.5; break;
        case TrustEventType::HELPFUL_ACTION: magnitude = 0.25; break;
        case TrustEventType::HARMFUL_ACTION: magnitude = 0.6; break;
    }
    if (outcome == TrustOutcome::POSITIVE) {
        return magnitude;
    }
    if (outcome == TrustOutcome::NEGATIVE) {
        return -magnitude;
    }
    return 0.0;
}

bool TrustScoreboard::isSafePersistencePath(
    const std::filesystem::path& path) noexcept {
    try {
        if (path.empty() || path.filename().empty() ||
            path.filename() == "." || path.filename() == "..") {
            return false;
        }
        for (const auto& component : path) {
            if (component == "..") {
                return false;
            }
        }
        const auto absolute = std::filesystem::absolute(path).lexically_normal();
        const auto parent = absolute.parent_path();
        std::error_code error;
        if (parent.empty() || !std::filesystem::exists(parent, error) || error ||
            !std::filesystem::is_directory(parent, error) || error ||
            std::filesystem::is_symlink(parent, error) || error) {
            return false;
        }
        const auto canonicalParent = std::filesystem::canonical(parent, error);
        if (error || canonicalParent != parent) {
            return false;
        }
        if (std::filesystem::exists(absolute, error)) {
            if (error || std::filesystem::is_symlink(absolute, error) || error ||
                !std::filesystem::is_regular_file(absolute, error) || error) {
                return false;
            }
        } else if (error) {
            return false;
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string TrustScoreboard::generateEventIdLocked() {
    for (;;) {
        const auto sequence = gEventSequence.fetch_add(1U,
            std::memory_order_relaxed);
        std::ostringstream stream;
        stream << "event-" << std::hex << std::setfill('0')
               << std::setw(8) << processIdentifier() << '-'
               << std::setw(16) << sequence;
        const std::string candidate = stream.str();
        if (seenEventIds_.find(candidate) == seenEventIds_.end()) {
            return candidate;
        }
    }
}

void TrustScoreboard::rebuildAgentLocked(
    const AgentId& agentId, Clock::time_point now) {
    const auto history = eventHistory_.find(agentId);
    if (history == eventHistory_.end() || history->second.empty()) {
        scores_.erase(agentId);
        scoreAsOf_.erase(agentId);
        return;
    }
    TrustScore score(agentId);
    calculateScore(score, history->second, config_);
    scores_[agentId] = score;
    scoreAsOf_[agentId] = std::min(now, score.lastUpdated);
}

TrustScore TrustScoreboard::scoreSnapshotLocked(
    const AgentId& agentId, Clock::time_point now) const {
    if (!validateAgentId(agentId)) {
        return TrustScore{};
    }
    const auto found = scores_.find(agentId);
    if (found == scores_.end()) {
        return TrustScore(agentId);
    }
    TrustScore result = found->second;
    const auto asOf = scoreAsOf_.find(agentId);
    const auto baseline = asOf == scoreAsOf_.end()
        ? result.lastUpdated : asOf->second;
    result.overallScore = applyTimeDecay(
        result.overallScore, baseline, config_, now);
    return result;
}

namespace trust_utils {

std::string trustEventTypeToString(TrustEventType type) {
    switch (type) {
        case TrustEventType::TASK_COMPLETED: return "Task Completed";
        case TrustEventType::TASK_FAILED: return "Task Failed";
        case TrustEventType::TASK_TIMEOUT: return "Task Timeout";
        case TrustEventType::RESPONSE_FAST: return "Fast Response";
        case TrustEventType::RESPONSE_SLOW: return "Slow Response";
        case TrustEventType::COLLABORATION_GOOD: return "Good Collaboration";
        case TrustEventType::COLLABORATION_POOR: return "Poor Collaboration";
        case TrustEventType::COMMUNICATION_CLEAR: return "Clear Communication";
        case TrustEventType::COMMUNICATION_UNCLEAR: return "Unclear Communication";
        case TrustEventType::RULE_VIOLATION: return "Rule Violation";
        case TrustEventType::HELPFUL_ACTION: return "Helpful Action";
        case TrustEventType::HARMFUL_ACTION: return "Harmful Action";
    }
    throw std::invalid_argument("invalid trust event type");
}

TrustEventType stringToTrustEventType(const std::string& value) {
    if (value == "Task Completed") return TrustEventType::TASK_COMPLETED;
    if (value == "Task Failed") return TrustEventType::TASK_FAILED;
    if (value == "Task Timeout") return TrustEventType::TASK_TIMEOUT;
    if (value == "Fast Response") return TrustEventType::RESPONSE_FAST;
    if (value == "Slow Response") return TrustEventType::RESPONSE_SLOW;
    if (value == "Good Collaboration") return TrustEventType::COLLABORATION_GOOD;
    if (value == "Poor Collaboration") return TrustEventType::COLLABORATION_POOR;
    if (value == "Clear Communication") return TrustEventType::COMMUNICATION_CLEAR;
    if (value == "Unclear Communication") return TrustEventType::COMMUNICATION_UNCLEAR;
    if (value == "Rule Violation") return TrustEventType::RULE_VIOLATION;
    if (value == "Helpful Action") return TrustEventType::HELPFUL_ACTION;
    if (value == "Harmful Action") return TrustEventType::HARMFUL_ACTION;
    throw std::invalid_argument("unknown trust event type");
}

std::string trustOutcomeToString(TrustOutcome outcome) {
    switch (outcome) {
        case TrustOutcome::POSITIVE: return "Positive";
        case TrustOutcome::NEGATIVE: return "Negative";
        case TrustOutcome::NEUTRAL: return "Neutral";
    }
    throw std::invalid_argument("invalid trust outcome");
}

TrustOutcome stringToTrustOutcome(const std::string& value) {
    if (value == "Positive") return TrustOutcome::POSITIVE;
    if (value == "Negative") return TrustOutcome::NEGATIVE;
    if (value == "Neutral") return TrustOutcome::NEUTRAL;
    throw std::invalid_argument("unknown trust outcome");
}

std::string formatTrustScore(double score) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2)
           << (clampScore(score) * 100.0) << '%';
    return stream.str();
}

std::string getTrustLevel(double score) {
    score = clampScore(score);
    if (score >= 0.8) return "Excellent";
    if (score >= 0.6) return "Good";
    if (score >= 0.4) return "Fair";
    if (score >= 0.2) return "Poor";
    return "Very Poor";
}

} // namespace trust_utils
} // namespace elizaos
