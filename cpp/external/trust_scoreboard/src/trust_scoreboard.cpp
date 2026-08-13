#include "elizaos/trust_scoreboard.hpp"
#include "elizaos/agentlogger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <atomic>
#include <limits>
#include <set>

#include <nlohmann/json.hpp>

namespace elizaos {
namespace {

using json = nlohmann::json;
constexpr int kTrustSnapshotVersion = 1;
constexpr const char* kTrustSnapshotSchema = "elizaos.trust_scoreboard";
constexpr const char* kTrustSnapshotTable = "trust_scoreboard";
constexpr const char* kTrustSnapshotRoom = "trust-scoreboard-snapshot";

std::int64_t trustTimeToMilliseconds(std::chrono::system_clock::time_point value) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(value.time_since_epoch()).count();
}

std::chrono::system_clock::time_point trustTimeFromMilliseconds(std::int64_t value) {
    return std::chrono::system_clock::time_point(std::chrono::milliseconds(value));
}

bool bounded01(double value) {
    return std::isfinite(value) && value >= 0.0 && value <= 1.0;
}

json trustConfigToJson(const TrustConfig& config) {
    return {
        {"reliability_weight", config.reliabilityWeight},
        {"responsiveness_weight", config.responsivenessWeight},
        {"quality_weight", config.qualityWeight},
        {"collaboration_weight", config.collaborationWeight},
        {"communication_weight", config.communicationWeight},
        {"compliance_weight", config.complianceWeight},
        {"decay", {
            {"enabled", config.decay.enabled},
            {"interval_hours", config.decay.decayInterval.count()},
            {"rate", config.decay.decayRate},
            {"minimum_score", config.decay.minimumScore}
        }},
        {"anomaly_threshold", config.anomalyThreshold},
        {"anomaly_window_events", config.anomalyWindowEvents},
        {"min_events_for_confidence", config.minEventsForConfidence},
        {"max_events_for_confidence", config.maxEventsForConfidence}
    };
}

TrustConfig trustConfigFromJson(const json& value) {
    TrustConfig config;
    config.reliabilityWeight = value.at("reliability_weight").get<double>();
    config.responsivenessWeight = value.at("responsiveness_weight").get<double>();
    config.qualityWeight = value.at("quality_weight").get<double>();
    config.collaborationWeight = value.at("collaboration_weight").get<double>();
    config.communicationWeight = value.at("communication_weight").get<double>();
    config.complianceWeight = value.at("compliance_weight").get<double>();
    const double weightSum = config.reliabilityWeight + config.responsivenessWeight +
        config.qualityWeight + config.collaborationWeight +
        config.communicationWeight + config.complianceWeight;
    if (!bounded01(config.reliabilityWeight) || !bounded01(config.responsivenessWeight) ||
        !bounded01(config.qualityWeight) || !bounded01(config.collaborationWeight) ||
        !bounded01(config.communicationWeight) || !bounded01(config.complianceWeight) ||
        std::abs(weightSum - 1.0) > 1e-6) {
        throw std::invalid_argument("trust component weights must be bounded and sum to one");
    }
    const auto& decay = value.at("decay");
    config.decay.enabled = decay.at("enabled").get<bool>();
    const auto intervalHours = decay.at("interval_hours").get<long long>();
    config.decay.decayRate = decay.at("rate").get<double>();
    config.decay.minimumScore = decay.at("minimum_score").get<double>();
    if (intervalHours <= 0 || !bounded01(config.decay.decayRate) ||
        !bounded01(config.decay.minimumScore)) {
        throw std::invalid_argument("invalid trust decay configuration");
    }
    config.decay.decayInterval = std::chrono::hours(intervalHours);
    config.anomalyThreshold = value.at("anomaly_threshold").get<double>();
    config.anomalyWindowEvents = value.at("anomaly_window_events").get<int>();
    config.minEventsForConfidence = value.at("min_events_for_confidence").get<int>();
    config.maxEventsForConfidence = value.at("max_events_for_confidence").get<int>();
    if (!bounded01(config.anomalyThreshold) || config.anomalyWindowEvents <= 0 ||
        config.minEventsForConfidence < 0 ||
        config.maxEventsForConfidence < config.minEventsForConfidence) {
        throw std::invalid_argument("invalid trust anomaly/confidence configuration");
    }
    return config;
}

json trustScoreToJson(const TrustScore& score) {
    return {
        {"agent_id", score.agentId}, {"overall", score.overallScore},
        {"reliability", score.reliabilityScore},
        {"responsiveness", score.responsivenessScore},
        {"quality", score.qualityScore}, {"collaboration", score.collaborationScore},
        {"communication", score.communicationScore}, {"compliance", score.complianceScore},
        {"total_events", score.totalEvents}, {"positive_events", score.positiveEvents},
        {"negative_events", score.negativeEvents}, {"neutral_events", score.neutralEvents},
        {"first_seen_ms", trustTimeToMilliseconds(score.firstSeen)},
        {"last_updated_ms", trustTimeToMilliseconds(score.lastUpdated)},
        {"confidence", score.confidence}
    };
}

TrustScore trustScoreFromJson(const json& value) {
    TrustScore score(value.at("agent_id").get<AgentId>());
    score.overallScore = value.at("overall").get<double>();
    score.reliabilityScore = value.at("reliability").get<double>();
    score.responsivenessScore = value.at("responsiveness").get<double>();
    score.qualityScore = value.at("quality").get<double>();
    score.collaborationScore = value.at("collaboration").get<double>();
    score.communicationScore = value.at("communication").get<double>();
    score.complianceScore = value.at("compliance").get<double>();
    score.totalEvents = value.at("total_events").get<int>();
    score.positiveEvents = value.at("positive_events").get<int>();
    score.negativeEvents = value.at("negative_events").get<int>();
    score.neutralEvents = value.at("neutral_events").get<int>();
    score.firstSeen = trustTimeFromMilliseconds(value.at("first_seen_ms").get<std::int64_t>());
    score.lastUpdated = trustTimeFromMilliseconds(value.at("last_updated_ms").get<std::int64_t>());
    score.confidence = value.at("confidence").get<double>();
    if (score.agentId.empty() || !bounded01(score.overallScore) ||
        !bounded01(score.reliabilityScore) || !bounded01(score.responsivenessScore) ||
        !bounded01(score.qualityScore) || !bounded01(score.collaborationScore) ||
        !bounded01(score.communicationScore) || !bounded01(score.complianceScore) ||
        !bounded01(score.confidence) || score.totalEvents < 0 || score.positiveEvents < 0 ||
        score.negativeEvents < 0 || score.neutralEvents < 0 ||
        score.positiveEvents + score.negativeEvents + score.neutralEvents != score.totalEvents) {
        throw std::invalid_argument("invalid persisted trust score");
    }
    return score;
}

json trustEventToJson(const TrustEvent& event) {
    return {
        {"event_id", event.eventId}, {"agent_id", event.agentId},
        {"type", static_cast<int>(event.type)}, {"outcome", static_cast<int>(event.outcome)},
        {"impact", event.impactScore}, {"context", event.context},
        {"timestamp_ms", trustTimeToMilliseconds(event.timestamp)},
        {"metadata", event.metadata}
    };
}

TrustEvent trustEventFromJson(const json& value) {
    const int typeValue = value.at("type").get<int>();
    const int outcomeValue = value.at("outcome").get<int>();
    if (typeValue < static_cast<int>(TrustEventType::TASK_COMPLETED) ||
        typeValue > static_cast<int>(TrustEventType::HARMFUL_ACTION) ||
        outcomeValue < static_cast<int>(TrustOutcome::POSITIVE) ||
        outcomeValue > static_cast<int>(TrustOutcome::NEUTRAL)) {
        throw std::invalid_argument("invalid persisted trust event enum");
    }
    const AgentId agentId = value.at("agent_id").get<AgentId>();
    const double impact = value.at("impact").get<double>();
    if (agentId.empty() || !std::isfinite(impact) || impact < -1.0 || impact > 1.0) {
        throw std::invalid_argument("invalid persisted trust event");
    }
    TrustEvent event(agentId, static_cast<TrustEventType>(typeValue),
                     static_cast<TrustOutcome>(outcomeValue), impact);
    event.eventId = value.at("event_id").get<std::string>();
    event.context = value.at("context").get<std::string>();
    event.timestamp = trustTimeFromMilliseconds(value.at("timestamp_ms").get<std::int64_t>());
    event.metadata = value.at("metadata").get<std::unordered_map<std::string, std::string>>();
    if (event.eventId.empty()) throw std::invalid_argument("persisted event id is empty");
    return event;
}

}  // namespace

// ============================================================================
// TrustScoreboard Implementation
// ============================================================================

TrustScoreboard::TrustScoreboard(std::shared_ptr<AgentMemoryManager> memoryMgr,
                                const TrustConfig& config)
    : memoryMgr_(memoryMgr), config_(config),
      lastDecayTime_(std::chrono::system_clock::now()) {
    
    AgentLogger logger;
    logger.log("TrustScoreboard initialized", "TrustScoreboard", "Init", LogLevel::INFO);
}

void TrustScoreboard::recordEvent(const AgentId& agentId, 
                                 TrustEventType type, 
                                 TrustOutcome outcome,
                                 double impactScore,
                                 const std::string& context) {
    // Create event
    TrustEvent event(agentId, type, outcome, impactScore);
    event.eventId = generateEventId();
    event.context = context;
    
    // Calculate impact if not provided
    if (impactScore == 0.0) {
        event.impactScore = calculateImpactScore(type, outcome);
    }
    
    // Store event
    {
        std::lock_guard<std::mutex> lock(eventsMutex_);
        eventHistory_[agentId].push_back(event);
    }
    
    // Update score
    {
        std::lock_guard<std::mutex> lock(scoresMutex_);
        
        // Initialize score if new agent
        if (scores_.find(agentId) == scores_.end()) {
            scores_[agentId] = TrustScore(agentId);
        }
        
        auto& score = scores_[agentId];
        
        // Update event counts
        score.totalEvents++;
        if (outcome == TrustOutcome::POSITIVE) {
            score.positiveEvents++;
        } else if (outcome == TrustOutcome::NEGATIVE) {
            score.negativeEvents++;
        } else {
            score.neutralEvents++;
        }
        
        // Update component scores
        updateComponentScores(score, event);
        
        // Recalculate overall score
        std::vector<TrustEvent> events;
        {
            std::lock_guard<std::mutex> eventLock(eventsMutex_);
            events = eventHistory_[agentId];
        }
        calculateScore(score, events);
        
        score.lastUpdated = std::chrono::system_clock::now();
    }
    
    AgentLogger logger;
    logger.log("Trust event recorded for agent " + agentId + ": " + 
               trust_utils::trustEventTypeToString(type),
               "TrustScoreboard", "Event", LogLevel::INFO);
}

void TrustScoreboard::recordTaskCompletion(const AgentId& agentId, 
                                          bool success,
                                          std::chrono::milliseconds responseTime) {
    TrustEventType type = success ? TrustEventType::TASK_COMPLETED : TrustEventType::TASK_FAILED;
    TrustOutcome outcome = success ? TrustOutcome::POSITIVE : TrustOutcome::NEGATIVE;
    
    // Adjust impact based on response time
    double impactScore = calculateImpactScore(type, outcome);
    if (success && responseTime < std::chrono::milliseconds(5000)) {
        impactScore *= 1.2; // Bonus for fast completion
    }
    
    std::stringstream context;
    context << "Response time: " << responseTime.count() << "ms";
    
    recordEvent(agentId, type, outcome, impactScore, context.str());
}

void TrustScoreboard::recordCollaboration(const AgentId& agentId, bool positive) {
    TrustEventType type = positive ? TrustEventType::COLLABORATION_GOOD : 
                                    TrustEventType::COLLABORATION_POOR;
    TrustOutcome outcome = positive ? TrustOutcome::POSITIVE : TrustOutcome::NEGATIVE;
    
    recordEvent(agentId, type, outcome);
}

void TrustScoreboard::recordCommunication(const AgentId& agentId, bool clear) {
    TrustEventType type = clear ? TrustEventType::COMMUNICATION_CLEAR : 
                                 TrustEventType::COMMUNICATION_UNCLEAR;
    TrustOutcome outcome = clear ? TrustOutcome::POSITIVE : TrustOutcome::NEGATIVE;
    
    recordEvent(agentId, type, outcome);
}

void TrustScoreboard::recordRuleViolation(const AgentId& agentId, 
                                         const std::string& violation) {
    recordEvent(agentId, TrustEventType::RULE_VIOLATION, 
               TrustOutcome::NEGATIVE, -0.3, violation);
}

TrustScore TrustScoreboard::getTrustScore(const AgentId& agentId) {
    std::lock_guard<std::mutex> lock(scoresMutex_);
    
    auto it = scores_.find(agentId);
    if (it == scores_.end()) {
        return TrustScore(agentId); // Return default score for unknown agent
    }
    
    // Apply time decay
    auto score = it->second;
    score.overallScore = applyTimeDecay(score.overallScore, score.lastUpdated);
    
    return score;
}

double TrustScoreboard::getOverallScore(const AgentId& agentId) {
    return getTrustScore(agentId).overallScore;
}

std::vector<TrustEvent> TrustScoreboard::getEventHistory(const AgentId& agentId, int limit) {
    std::lock_guard<std::mutex> lock(eventsMutex_);
    
    auto it = eventHistory_.find(agentId);
    if (it == eventHistory_.end()) {
        return {};
    }
    
    const auto& events = it->second;
    
    if (limit <= 0 || static_cast<size_t>(limit) >= events.size()) {
        return events;
    }
    
    // Return most recent events
    return std::vector<TrustEvent>(events.end() - limit, events.end());
}

std::vector<TrustScore> TrustScoreboard::getLeaderboard(int limit) {
    std::lock_guard<std::mutex> lock(scoresMutex_);
    
    std::vector<TrustScore> leaderboard;
    leaderboard.reserve(scores_.size());
    
    for (const auto& [agentId, score] : scores_) {
        leaderboard.push_back(score);
    }
    
    // Sort by overall score descending
    std::sort(leaderboard.begin(), leaderboard.end(),
             [](const TrustScore& a, const TrustScore& b) {
                 return a.overallScore > b.overallScore;
             });
    
    if (limit > 0 && static_cast<size_t>(limit) < leaderboard.size()) {
        leaderboard.resize(limit);
    }
    
    return leaderboard;
}

std::vector<TrustScore> TrustScoreboard::getTopAgents(int limit) {
    return getLeaderboard(limit);
}

std::vector<TrustScore> TrustScoreboard::getBottomAgents(int limit) {
    std::lock_guard<std::mutex> lock(scoresMutex_);
    
    std::vector<TrustScore> bottom;
    bottom.reserve(scores_.size());
    
    for (const auto& [agentId, score] : scores_) {
        bottom.push_back(score);
    }
    
    // Sort by overall score ascending
    std::sort(bottom.begin(), bottom.end(),
             [](const TrustScore& a, const TrustScore& b) {
                 return a.overallScore < b.overallScore;
             });
    
    if (limit > 0 && static_cast<size_t>(limit) < bottom.size()) {
        bottom.resize(limit);
    }
    
    return bottom;
}

TrustComparison TrustScoreboard::compareAgents(const AgentId& agent1, const AgentId& agent2) {
    TrustScore score1 = getTrustScore(agent1);
    TrustScore score2 = getTrustScore(agent2);
    
    TrustComparison comparison;
    comparison.agent1 = agent1;
    comparison.agent2 = agent2;
    comparison.scoreDifference = score1.overallScore - score2.overallScore;
    
    if (comparison.scoreDifference > 0.1) {
        comparison.recommendation = agent1 + " is more trusted";
        
        if (score1.reliabilityScore > score2.reliabilityScore + 0.1) {
            comparison.reasons.push_back("Higher reliability");
        }
        if (score1.collaborationScore > score2.collaborationScore + 0.1) {
            comparison.reasons.push_back("Better collaboration");
        }
        if (score1.qualityScore > score2.qualityScore + 0.1) {
            comparison.reasons.push_back("Higher quality output");
        }
    } else if (comparison.scoreDifference < -0.1) {
        comparison.recommendation = agent2 + " is more trusted";
        
        if (score2.reliabilityScore > score1.reliabilityScore + 0.1) {
            comparison.reasons.push_back("Higher reliability");
        }
        if (score2.collaborationScore > score1.collaborationScore + 0.1) {
            comparison.reasons.push_back("Better collaboration");
        }
        if (score2.qualityScore > score1.qualityScore + 0.1) {
            comparison.reasons.push_back("Higher quality output");
        }
    } else {
        comparison.recommendation = "Both agents have similar trust levels";
    }
    
    return comparison;
}

AgentId TrustScoreboard::selectMostTrusted(const std::vector<AgentId>& candidates) {
    if (candidates.empty()) {
        return "";
    }
    
    AgentId mostTrusted = candidates[0];
    double highestScore = getOverallScore(mostTrusted);
    
    for (size_t i = 1; i < candidates.size(); ++i) {
        double score = getOverallScore(candidates[i]);
        if (score > highestScore) {
            highestScore = score;
            mostTrusted = candidates[i];
        }
    }
    
    return mostTrusted;
}

std::vector<AgentId> TrustScoreboard::rankByTrust(const std::vector<AgentId>& agents) {
    std::vector<std::pair<AgentId, double>> agentScores;
    agentScores.reserve(agents.size());
    
    for (const auto& agentId : agents) {
        agentScores.emplace_back(agentId, getOverallScore(agentId));
    }
    
    // Sort by score descending
    std::sort(agentScores.begin(), agentScores.end(),
             [](const auto& a, const auto& b) {
                 return a.second > b.second;
             });
    
    std::vector<AgentId> ranked;
    ranked.reserve(agentScores.size());
    
    for (const auto& [agentId, score] : agentScores) {
        ranked.push_back(agentId);
    }
    
    return ranked;
}

std::vector<TrustAnomaly> TrustScoreboard::detectAnomalies() {
    std::vector<TrustAnomaly> anomalies;
    
    std::lock_guard<std::mutex> lock(scoresMutex_);
    
    for (const auto& [agentId, score] : scores_) {
        if (isAnomalous(agentId)) {
            auto agentAnomalies = detectAnomaliesForAgent(agentId);
            anomalies.insert(anomalies.end(), agentAnomalies.begin(), agentAnomalies.end());
        }
    }
    
    return anomalies;
}

std::vector<TrustAnomaly> TrustScoreboard::detectAnomaliesForAgent(const AgentId& agentId) {
    std::vector<TrustAnomaly> anomalies;
    
    // Get recent events
    auto recentEvents = getEventHistory(agentId, config_.anomalyWindowEvents);
    
    if (recentEvents.size() < 5) {
        return anomalies; // Not enough data
    }
    
    // Check for sudden score drops
    int negativeCount = 0;
    for (const auto& event : recentEvents) {
        if (event.outcome == TrustOutcome::NEGATIVE) {
            negativeCount++;
        }
    }
    
    double negativeRatio = static_cast<double>(negativeCount) / recentEvents.size();
    
    if (negativeRatio > config_.anomalyThreshold) {
        TrustAnomaly anomaly;
        anomaly.agentId = agentId;
        anomaly.anomalyType = "High negative event rate";
        anomaly.severity = negativeRatio;
        anomaly.description = "Agent has " + std::to_string(negativeCount) + 
                            " negative events in last " + 
                            std::to_string(recentEvents.size()) + " events";
        anomaly.detectedAt = std::chrono::system_clock::now();
        anomaly.evidence.push_back("Negative event ratio: " + 
                                  std::to_string(negativeRatio * 100) + "%");
        
        anomalies.push_back(anomaly);
    }
    
    // Check for rule violations
    int violationCount = 0;
    for (const auto& event : recentEvents) {
        if (event.type == TrustEventType::RULE_VIOLATION) {
            violationCount++;
        }
    }
    
    if (violationCount > 0) {
        TrustAnomaly anomaly;
        anomaly.agentId = agentId;
        anomaly.anomalyType = "Rule violations detected";
        anomaly.severity = std::min(1.0, violationCount * 0.3);
        anomaly.description = "Agent has " + std::to_string(violationCount) + 
                            " rule violations";
        anomaly.detectedAt = std::chrono::system_clock::now();
        
        anomalies.push_back(anomaly);
    }
    
    return anomalies;
}

bool TrustScoreboard::isAnomalous(const AgentId& agentId) {
    auto anomalies = detectAnomaliesForAgent(agentId);
    return !anomalies.empty();
}

void TrustScoreboard::applyDecay() {
    if (!config_.decay.enabled) {
        return;
    }
    
    auto now = std::chrono::system_clock::now();
    auto timeSinceDecay = std::chrono::duration_cast<std::chrono::hours>(
        now - lastDecayTime_);
    
    if (timeSinceDecay < config_.decay.decayInterval) {
        return; // Not time yet
    }
    
    std::lock_guard<std::mutex> lock(scoresMutex_);
    
    for (auto& [agentId, score] : scores_) {
        applyDecayForAgent(agentId);
    }
    
    lastDecayTime_ = now;
    
    AgentLogger logger;
    logger.log("Applied trust decay to all agents", 
               "TrustScoreboard", "Decay", LogLevel::INFO);
}

void TrustScoreboard::applyDecayForAgent(const AgentId& agentId) {
    std::lock_guard<std::mutex> lock(scoresMutex_);
    
    auto it = scores_.find(agentId);
    if (it == scores_.end()) {
        return;
    }
    
    auto& score = it->second;
    
    // Apply decay to overall score
    score.overallScore = applyTimeDecay(score.overallScore, score.lastUpdated);
    
    // Don't let it drop below minimum
    if (score.overallScore < config_.decay.minimumScore) {
        score.overallScore = config_.decay.minimumScore;
    }
}

int TrustScoreboard::getTotalAgents() const {
    std::lock_guard<std::mutex> lock(scoresMutex_);
    return static_cast<int>(scores_.size());
}

int TrustScoreboard::getTotalEvents() const {
    std::lock_guard<std::mutex> lock(eventsMutex_);
    
    int total = 0;
    for (const auto& [agentId, events] : eventHistory_) {
        total += static_cast<int>(events.size());
    }
    
    return total;
}

double TrustScoreboard::getAverageTrustScore() const {
    std::lock_guard<std::mutex> lock(scoresMutex_);
    
    if (scores_.empty()) {
        return 0.5; // Default middle score
    }
    
    double sum = 0.0;
    for (const auto& [agentId, score] : scores_) {
        sum += score.overallScore;
    }
    
    return sum / scores_.size();
}

void TrustScoreboard::updateConfig(const TrustConfig& newConfig) {
    config_ = newConfig;
    
    AgentLogger logger;
    logger.log("Trust configuration updated", 
               "TrustScoreboard", "Config", LogLevel::INFO);
}

bool TrustScoreboard::saveTrustData() {
    if (!memoryMgr_) return false;
    try {
        std::unordered_map<AgentId, TrustScore> scoreSnapshot;
        std::unordered_map<AgentId, std::vector<TrustEvent>> eventSnapshot;
        {
            std::scoped_lock lock(scoresMutex_, eventsMutex_);
            scoreSnapshot = scores_;
            eventSnapshot = eventHistory_;
        }

        json scores = json::array();
        std::vector<AgentId> scoreIds;
        scoreIds.reserve(scoreSnapshot.size());
        for (const auto& [agentId, _] : scoreSnapshot) scoreIds.push_back(agentId);
        std::sort(scoreIds.begin(), scoreIds.end());
        for (const auto& agentId : scoreIds) scores.push_back(trustScoreToJson(scoreSnapshot.at(agentId)));

        json histories = json::object();
        std::vector<AgentId> historyIds;
        historyIds.reserve(eventSnapshot.size());
        for (const auto& [agentId, _] : eventSnapshot) historyIds.push_back(agentId);
        std::sort(historyIds.begin(), historyIds.end());
        for (const auto& agentId : historyIds) {
            json events = json::array();
            for (const auto& event : eventSnapshot.at(agentId)) events.push_back(trustEventToJson(event));
            histories[agentId] = std::move(events);
        }

        const json document = {
            {"schema", kTrustSnapshotSchema}, {"version", kTrustSnapshotVersion},
            {"saved_at_ms", trustTimeToMilliseconds(std::chrono::system_clock::now())},
            {"last_decay_ms", trustTimeToMilliseconds(lastDecayTime_)},
            {"config", trustConfigToJson(config_)},
            {"scores", std::move(scores)}, {"event_history", std::move(histories)}
        };

        const auto previous = memoryMgr_->getAllMemoriesFromTable(kTrustSnapshotTable);
        std::vector<UUID> previousSnapshotIds;
        for (const auto& memory : previous) {
            if (memory && memory->getRoomId() == kTrustSnapshotRoom) {
                previousSnapshotIds.push_back(memory->getId());
            }
        }

        const UUID snapshotId = generateUUID();
        auto memory = std::make_shared<Memory>(snapshotId, document.dump(),
                                               "trust-scoreboard", "trust-scoreboard");
        memory->setRoomId(kTrustSnapshotRoom);
        const UUID storedId = memoryMgr_->createMemory(memory, kTrustSnapshotTable, false);
        if (storedId != snapshotId || !memoryMgr_->getMemoryById(snapshotId)) return false;
        for (const auto& id : previousSnapshotIds) memoryMgr_->deleteMemory(id);
        return true;
    } catch (...) {
        return false;
    }
}

bool TrustScoreboard::loadTrustData() {
    if (!memoryMgr_) return false;
    try {
        const auto memories = memoryMgr_->getAllMemoriesFromTable(kTrustSnapshotTable);
        std::shared_ptr<Memory> newest;
        for (const auto& memory : memories) {
            if (!memory || memory->getRoomId() != kTrustSnapshotRoom) continue;
            if (!newest || memory->getCreatedAt() > newest->getCreatedAt()) newest = memory;
        }
        if (!newest) return false;

        const json document = json::parse(newest->getContent());
        if (!document.is_object() || document.value("schema", "") != kTrustSnapshotSchema ||
            document.value("version", 0) != kTrustSnapshotVersion) return false;
        TrustConfig loadedConfig = trustConfigFromJson(document.at("config"));
        const auto loadedLastDecay = trustTimeFromMilliseconds(
            document.at("last_decay_ms").get<std::int64_t>());

        const auto& scoreValues = document.at("scores");
        const auto& historyValues = document.at("event_history");
        if (!scoreValues.is_array() || !historyValues.is_object()) return false;

        std::unordered_map<AgentId, TrustScore> loadedScores;
        std::unordered_map<AgentId, std::vector<TrustEvent>> loadedHistory;
        for (const auto& value : scoreValues) {
            TrustScore score = trustScoreFromJson(value);
            if (!loadedScores.emplace(score.agentId, score).second) return false;
        }
        std::set<std::string> eventIds;
        for (auto it = historyValues.begin(); it != historyValues.end(); ++it) {
            if (!it.value().is_array() || it.key().empty()) return false;
            auto& events = loadedHistory[it.key()];
            for (const auto& eventValue : it.value()) {
                TrustEvent event = trustEventFromJson(eventValue);
                if (event.agentId != it.key() || !eventIds.insert(event.eventId).second) return false;
                events.push_back(std::move(event));
            }
        }
        for (const auto& [agentId, score] : loadedScores) {
            auto historyIt = loadedHistory.find(agentId);
            const std::size_t eventCount = historyIt == loadedHistory.end() ? 0 : historyIt->second.size();
            if (eventCount != static_cast<std::size_t>(score.totalEvents)) return false;
        }
        for (const auto& [agentId, _] : loadedHistory) {
            if (loadedScores.count(agentId) == 0) return false;
        }

        {
            std::scoped_lock lock(scoresMutex_, eventsMutex_);
            scores_.swap(loadedScores);
            eventHistory_.swap(loadedHistory);
            config_ = loadedConfig;
            lastDecayTime_ = loadedLastDecay;
        }
        return true;
    } catch (...) {
        return false;
    }
}

// ============================================================================
// Private Helper Methods
// ============================================================================

void TrustScoreboard::calculateScore(TrustScore& score, 
                                    const std::vector<TrustEvent>& events) {
    if (events.empty()) {
        score.overallScore = 0.5; // Default neutral score
        return;
    }
    
    // Calculate weighted average of component scores
    score.overallScore = 
        score.reliabilityScore * config_.reliabilityWeight +
        score.responsivenessScore * config_.responsivenessWeight +
        score.qualityScore * config_.qualityWeight +
        score.collaborationScore * config_.collaborationWeight +
        score.communicationScore * config_.communicationWeight +
        score.complianceScore * config_.complianceWeight;
    
    // Clamp to [0, 1]
    score.overallScore = std::max(0.0, std::min(1.0, score.overallScore));
    
    // Calculate confidence
    score.confidence = calculateConfidence(score.totalEvents);
}

void TrustScoreboard::updateComponentScores(TrustScore& score, const TrustEvent& event) {
    double adjustment = event.impactScore * 0.1; // Scale impact
    
    switch (event.type) {
        case TrustEventType::TASK_COMPLETED:
        case TrustEventType::TASK_FAILED:
        case TrustEventType::TASK_TIMEOUT:
            score.reliabilityScore += adjustment;
            break;
            
        case TrustEventType::RESPONSE_FAST:
        case TrustEventType::RESPONSE_SLOW:
            score.responsivenessScore += adjustment;
            break;
            
        case TrustEventType::COLLABORATION_GOOD:
        case TrustEventType::COLLABORATION_POOR:
            score.collaborationScore += adjustment;
            break;
            
        case TrustEventType::COMMUNICATION_CLEAR:
        case TrustEventType::COMMUNICATION_UNCLEAR:
            score.communicationScore += adjustment;
            break;
            
        case TrustEventType::RULE_VIOLATION:
            score.complianceScore += adjustment;
            break;
            
        case TrustEventType::HELPFUL_ACTION:
        case TrustEventType::HARMFUL_ACTION:
            score.qualityScore += adjustment;
            break;
    }
    
    // Clamp all component scores to [0, 1]
    score.reliabilityScore = std::max(0.0, std::min(1.0, score.reliabilityScore));
    score.responsivenessScore = std::max(0.0, std::min(1.0, score.responsivenessScore));
    score.qualityScore = std::max(0.0, std::min(1.0, score.qualityScore));
    score.collaborationScore = std::max(0.0, std::min(1.0, score.collaborationScore));
    score.communicationScore = std::max(0.0, std::min(1.0, score.communicationScore));
    score.complianceScore = std::max(0.0, std::min(1.0, score.complianceScore));
}

double TrustScoreboard::calculateConfidence(int eventCount) const {
    if (eventCount < config_.minEventsForConfidence) {
        return static_cast<double>(eventCount) / config_.minEventsForConfidence;
    }
    
    if (eventCount >= config_.maxEventsForConfidence) {
        return 1.0;
    }
    
    return static_cast<double>(eventCount - config_.minEventsForConfidence) /
           (config_.maxEventsForConfidence - config_.minEventsForConfidence);
}

double TrustScoreboard::applyTimeDecay(double score, 
                                      std::chrono::system_clock::time_point lastUpdate) const {
    if (!config_.decay.enabled) {
        return score;
    }
    
    auto now = std::chrono::system_clock::now();
    auto timeSinceUpdate = std::chrono::duration_cast<std::chrono::hours>(now - lastUpdate);
    
    int intervals = static_cast<int>(timeSinceUpdate / config_.decay.decayInterval);
    
    if (intervals <= 0) {
        return score;
    }
    
    // Apply exponential decay
    double decayedScore = score * std::pow(1.0 - config_.decay.decayRate, intervals);
    
    // Don't decay below minimum
    return std::max(config_.decay.minimumScore, decayedScore);
}

TrustOutcome TrustScoreboard::determineOutcome(TrustEventType type, bool success) const {
    switch (type) {
        case TrustEventType::TASK_COMPLETED:
        case TrustEventType::RESPONSE_FAST:
        case TrustEventType::COLLABORATION_GOOD:
        case TrustEventType::COMMUNICATION_CLEAR:
        case TrustEventType::HELPFUL_ACTION:
            return success ? TrustOutcome::POSITIVE : TrustOutcome::NEGATIVE;
            
        case TrustEventType::TASK_FAILED:
        case TrustEventType::TASK_TIMEOUT:
        case TrustEventType::RESPONSE_SLOW:
        case TrustEventType::COLLABORATION_POOR:
        case TrustEventType::COMMUNICATION_UNCLEAR:
        case TrustEventType::RULE_VIOLATION:
        case TrustEventType::HARMFUL_ACTION:
            return TrustOutcome::NEGATIVE;
            
        default:
            return TrustOutcome::NEUTRAL;
    }
}

double TrustScoreboard::calculateImpactScore(TrustEventType type, TrustOutcome outcome) const {
    (void)outcome; // Suppress unused warning - outcome determines sign, type determines magnitude
    double baseImpact = 0.0;
    
    switch (type) {
        case TrustEventType::TASK_COMPLETED:
            baseImpact = 0.2;
            break;
        case TrustEventType::TASK_FAILED:
            baseImpact = -0.3;
            break;
        case TrustEventType::TASK_TIMEOUT:
            baseImpact = -0.4;
            break;
        case TrustEventType::RESPONSE_FAST:
            baseImpact = 0.1;
            break;
        case TrustEventType::RESPONSE_SLOW:
            baseImpact = -0.1;
            break;
        case TrustEventType::COLLABORATION_GOOD:
            baseImpact = 0.15;
            break;
        case TrustEventType::COLLABORATION_POOR:
            baseImpact = -0.2;
            break;
        case TrustEventType::COMMUNICATION_CLEAR:
            baseImpact = 0.1;
            break;
        case TrustEventType::COMMUNICATION_UNCLEAR:
            baseImpact = -0.1;
            break;
        case TrustEventType::RULE_VIOLATION:
            baseImpact = -0.5;
            break;
        case TrustEventType::HELPFUL_ACTION:
            baseImpact = 0.25;
            break;
        case TrustEventType::HARMFUL_ACTION:
            baseImpact = -0.6;
            break;
    }
    
    return baseImpact;
}

bool TrustScoreboard::detectAnomalyPattern(const std::vector<TrustEvent>& recentEvents) const {
    if (recentEvents.size() < 5) {
        return false;
    }
    
    int negativeCount = 0;
    for (const auto& event : recentEvents) {
        if (event.outcome == TrustOutcome::NEGATIVE) {
            negativeCount++;
        }
    }
    
    double negativeRatio = static_cast<double>(negativeCount) / recentEvents.size();
    return negativeRatio > config_.anomalyThreshold;
}

std::string TrustScoreboard::generateEventId() {
    static std::atomic<int> counter{0};
    std::stringstream ss;
    ss << "event_" << std::setfill('0') << std::setw(8) << counter++;
    return ss.str();
}

// ============================================================================
// Utility Functions
// ============================================================================

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
        default: return "Unknown";
    }
}

TrustEventType stringToTrustEventType(const std::string& typeStr) {
    if (typeStr == "Task Completed") return TrustEventType::TASK_COMPLETED;
    if (typeStr == "Task Failed") return TrustEventType::TASK_FAILED;
    if (typeStr == "Task Timeout") return TrustEventType::TASK_TIMEOUT;
    if (typeStr == "Fast Response") return TrustEventType::RESPONSE_FAST;
    if (typeStr == "Slow Response") return TrustEventType::RESPONSE_SLOW;
    if (typeStr == "Good Collaboration") return TrustEventType::COLLABORATION_GOOD;
    if (typeStr == "Poor Collaboration") return TrustEventType::COLLABORATION_POOR;
    if (typeStr == "Clear Communication") return TrustEventType::COMMUNICATION_CLEAR;
    if (typeStr == "Unclear Communication") return TrustEventType::COMMUNICATION_UNCLEAR;
    if (typeStr == "Rule Violation") return TrustEventType::RULE_VIOLATION;
    if (typeStr == "Helpful Action") return TrustEventType::HELPFUL_ACTION;
    if (typeStr == "Harmful Action") return TrustEventType::HARMFUL_ACTION;
    return TrustEventType::TASK_COMPLETED; // Default
}

std::string trustOutcomeToString(TrustOutcome outcome) {
    switch (outcome) {
        case TrustOutcome::POSITIVE: return "Positive";
        case TrustOutcome::NEGATIVE: return "Negative";
        case TrustOutcome::NEUTRAL: return "Neutral";
        default: return "Unknown";
    }
}

TrustOutcome stringToTrustOutcome(const std::string& outcomeStr) {
    if (outcomeStr == "Positive") return TrustOutcome::POSITIVE;
    if (outcomeStr == "Negative") return TrustOutcome::NEGATIVE;
    if (outcomeStr == "Neutral") return TrustOutcome::NEUTRAL;
    return TrustOutcome::NEUTRAL; // Default
}

std::string formatTrustScore(double score) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << (score * 100) << "%";
    return ss.str();
}

std::string getTrustLevel(double score) {
    if (score >= 0.8) return "Excellent";
    if (score >= 0.6) return "Good";
    if (score >= 0.4) return "Fair";
    if (score >= 0.2) return "Poor";
    return "Very Poor";
}

} // namespace trust_utils

} // namespace elizaos
