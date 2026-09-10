#pragma once

#include "elizaos/core.hpp"
#include "elizaos/agentcomms.hpp"

#include <chrono>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

class AgentMemoryManager;

enum class TrustEventType {
    TASK_COMPLETED,
    TASK_FAILED,
    TASK_TIMEOUT,
    RESPONSE_FAST,
    RESPONSE_SLOW,
    COLLABORATION_GOOD,
    COLLABORATION_POOR,
    COMMUNICATION_CLEAR,
    COMMUNICATION_UNCLEAR,
    RULE_VIOLATION,
    HELPFUL_ACTION,
    HARMFUL_ACTION
};

enum class TrustOutcome { POSITIVE, NEGATIVE, NEUTRAL };

/** A caller-observed evidence record. Explicit records must carry a unique ID. */
struct TrustEvent {
    std::string eventId;
    AgentId agentId;
    TrustEventType type;
    TrustOutcome outcome;
    double impactScore;
    std::string context;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;

    TrustEvent(const AgentId& agent, TrustEventType eventType,
               TrustOutcome eventOutcome, double impact)
        : agentId(agent), type(eventType), outcome(eventOutcome),
          impactScore(impact), timestamp(std::chrono::system_clock::now()) {}
};

struct TrustScore {
    AgentId agentId;
    double overallScore;
    double reliabilityScore;
    double responsivenessScore;
    double qualityScore;
    double collaborationScore;
    double communicationScore;
    double complianceScore;
    int totalEvents;
    int positiveEvents;
    int negativeEvents;
    int neutralEvents;
    std::chrono::system_clock::time_point firstSeen;
    std::chrono::system_clock::time_point lastUpdated;
    double confidence;

    TrustScore() : TrustScore("") {}
    explicit TrustScore(const AgentId& agent)
        : agentId(agent), overallScore(0.5), reliabilityScore(0.5),
          responsivenessScore(0.5), qualityScore(0.5),
          collaborationScore(0.5), communicationScore(0.5),
          complianceScore(0.5), totalEvents(0), positiveEvents(0),
          negativeEvents(0), neutralEvents(0),
          firstSeen(std::chrono::system_clock::now()),
          lastUpdated(std::chrono::system_clock::now()), confidence(0.0) {}
};

struct TrustComparison {
    AgentId agent1;
    AgentId agent2;
    double scoreDifference = 0.0;
    std::string recommendation;
    std::vector<std::string> reasons;
};

struct TrustAnomaly {
    AgentId agentId;
    std::string anomalyType;
    double severity = 0.0;
    std::string description;
    std::chrono::system_clock::time_point detectedAt;
    /** Event IDs of the actual records supporting the finding. */
    std::vector<std::string> evidence;
};

struct TrustDecayConfig {
    bool enabled = true;
    std::chrono::hours decayInterval = std::chrono::hours(24 * 7);
    double decayRate = 0.05;
    double minimumScore = 0.1;
};

struct TrustConfig {
    double reliabilityWeight = 0.25;
    double responsivenessWeight = 0.15;
    double qualityWeight = 0.20;
    double collaborationWeight = 0.20;
    double communicationWeight = 0.10;
    double complianceWeight = 0.10;
    TrustDecayConfig decay;
    /** Inclusive recent-negative ratio that triggers an anomaly. */
    double anomalyThreshold = 0.3;
    int anomalyWindowEvents = 10;
    int minEventsForConfidence = 10;
    int maxEventsForConfidence = 100;
    /** Oldest records are evicted after this per-agent bound is reached. */
    std::size_t maxEventHistoryPerAgent = 1000U;
};

/**
 * Thread-safe reputation scoreboard. All snapshots are returned by value.
 * Persistence is disabled until an explicit safe local path is configured.
 */
class TrustScoreboard {
public:
    TrustScoreboard(std::shared_ptr<AgentMemoryManager> memoryMgr,
                    const TrustConfig& config = TrustConfig(),
                    const std::filesystem::path& persistencePath = {});
    ~TrustScoreboard() = default;

    /** Records caller-supplied evidence. Returns false for invalid/replayed data. */
    bool recordEvent(const TrustEvent& evidence);
    /** Assigns a process-unique ID and current timestamp to convenience data. */
    bool recordEvent(const AgentId& agentId, TrustEventType type,
                     TrustOutcome outcome, double impactScore = 0.0,
                     const std::string& context = "");

    bool recordTaskCompletion(const AgentId& agentId, bool success,
                              std::chrono::milliseconds responseTime);
    bool recordCollaboration(const AgentId& agentId, bool positive);
    bool recordCommunication(const AgentId& agentId, bool clear);
    bool recordRuleViolation(const AgentId& agentId,
                             const std::string& violation);

    TrustScore getTrustScore(const AgentId& agentId) const;
    double getOverallScore(const AgentId& agentId) const;
    /** Chronological snapshot of the most recent limit rows. */
    std::vector<TrustEvent> getEventHistory(const AgentId& agentId,
                                            int limit = 100) const;

    /** Descending score, then ascending ID. Non-positive limits are empty. */
    std::vector<TrustScore> getLeaderboard(int limit = 10) const;
    std::vector<TrustScore> getTopAgents(int limit = 10) const;
    /** Ascending score, then ascending ID. */
    std::vector<TrustScore> getBottomAgents(int limit = 10) const;

    TrustComparison compareAgents(const AgentId& agent1,
                                  const AgentId& agent2) const;
    /** Ties resolve to the lexicographically smaller valid ID. */
    AgentId selectMostTrusted(const std::vector<AgentId>& candidates) const;
    /** Invalid and duplicate IDs are omitted; ties sort by ID. */
    std::vector<AgentId> rankByTrust(const std::vector<AgentId>& agents) const;

    std::vector<TrustAnomaly> detectAnomalies() const;
    std::vector<TrustAnomaly> detectAnomaliesForAgent(
        const AgentId& agentId) const;
    bool isAnomalous(const AgentId& agentId) const;

    void applyDecay();
    void applyDecayForAgent(const AgentId& agentId);

    int getTotalAgents() const;
    int getTotalEvents() const;
    double getAverageTrustScore() const;

    /** Valid weights are normalized atomically; invalid configurations fail. */
    bool updateConfig(const TrustConfig& newConfig);
    TrustConfig getConfig() const;

    /** Configures an existing, non-symlink parent and a regular-file target. */
    bool setPersistencePath(const std::filesystem::path& path);
    std::filesystem::path getPersistencePath() const;

    /** Atomic versioned JSON persistence. No configured path returns false. */
    bool saveTrustData() const;
    /** Strict transactional load; malformed input leaves all state unchanged. */
    bool loadTrustData();

private:
    std::shared_ptr<AgentMemoryManager> memoryMgr_;
    TrustConfig config_;
    std::unordered_map<AgentId, TrustScore> scores_;
    std::unordered_map<AgentId, std::vector<TrustEvent>> eventHistory_;
    std::unordered_set<std::string> seenEventIds_;
    std::unordered_map<AgentId, std::chrono::system_clock::time_point>
        scoreAsOf_;
    std::chrono::system_clock::time_point lastDecayTime_;
    std::filesystem::path persistencePath_;

    mutable std::mutex stateMutex_;
    mutable std::mutex persistenceMutex_;

    static bool normalizeAndValidateConfig(TrustConfig& config) noexcept;
    static bool validateAgentId(const AgentId& agentId) noexcept;
    static bool validateEventId(const std::string& eventId) noexcept;
    static bool validateEvent(const TrustEvent& event,
                              std::chrono::system_clock::time_point now) noexcept;
    static bool eventLess(const TrustEvent& lhs, const TrustEvent& rhs) noexcept;
    static void calculateScore(TrustScore& score,
                               const std::vector<TrustEvent>& events,
                               const TrustConfig& config);
    static void updateComponentScores(TrustScore& score,
                                      const TrustEvent& event) noexcept;
    static double calculateConfidence(int eventCount,
                                      const TrustConfig& config) noexcept;
    static double applyTimeDecay(
        double score, std::chrono::system_clock::time_point lastUpdate,
        const TrustConfig& config,
        std::chrono::system_clock::time_point now) noexcept;
    static double calculateImpactScore(TrustEventType type,
                                       TrustOutcome outcome) noexcept;
    static bool isSafePersistencePath(const std::filesystem::path& path) noexcept;

    std::string generateEventIdLocked();
    void rebuildAgentLocked(const AgentId& agentId,
                            std::chrono::system_clock::time_point now);
    TrustScore scoreSnapshotLocked(
        const AgentId& agentId,
        std::chrono::system_clock::time_point now) const;
};

namespace trust_utils {
std::string trustEventTypeToString(TrustEventType type);
TrustEventType stringToTrustEventType(const std::string& typeStr);
std::string trustOutcomeToString(TrustOutcome outcome);
TrustOutcome stringToTrustOutcome(const std::string& outcomeStr);
std::string formatTrustScore(double score);
std::string getTrustLevel(double score);
} // namespace trust_utils

} // namespace elizaos
