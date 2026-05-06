#pragma once

#include "elizaos/agentmemory.hpp"
#include "elizaos/agentcomms.hpp"
#include "elizaos/core.hpp"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <functional>

namespace elizaos {

/**
 * @brief Trust Scoreboard - Reputation and Trust Management System
 * 
 * Tracks agent behavior, calculates trust scores, and provides reputation-based
 * decision support for multi-agent systems.
 */

// Forward declarations
class AgentMemoryManager;

/**
 * Trust event types
 */
enum class TrustEventType {
    TASK_COMPLETED,      // Successfully completed a task
    TASK_FAILED,         // Failed to complete a task
    TASK_TIMEOUT,        // Task timed out
    RESPONSE_FAST,       // Responded quickly
    RESPONSE_SLOW,       // Responded slowly
    COLLABORATION_GOOD,  // Good collaboration behavior
    COLLABORATION_POOR,  // Poor collaboration behavior
    COMMUNICATION_CLEAR, // Clear communication
    COMMUNICATION_UNCLEAR, // Unclear communication
    RULE_VIOLATION,      // Violated group rules
    HELPFUL_ACTION,      // Performed helpful action
    HARMFUL_ACTION       // Performed harmful action
};

/**
 * Trust event outcome
 */
enum class TrustOutcome {
    POSITIVE,  // Increases trust
    NEGATIVE,  // Decreases trust
    NEUTRAL    // No effect on trust
};

/**
 * Individual trust event record
 */
struct TrustEvent {
    std::string eventId;
    AgentId agentId;
    TrustEventType type;
    TrustOutcome outcome;
    double impactScore;  // -1.0 to 1.0
    std::string context;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
    
    TrustEvent(const AgentId& agent, TrustEventType t, TrustOutcome o, double impact)
        : agentId(agent), type(t), outcome(o), impactScore(impact),
          timestamp(std::chrono::system_clock::now()) {}
};

/**
 * Comprehensive trust score for an agent
 */
struct TrustScore {
    AgentId agentId;
    
    // Overall score (0.0 - 1.0)
    double overallScore;
    
    // Component scores (0.0 - 1.0)
    double reliabilityScore;    // Task completion consistency
    double responsivenessScore; // Response time and availability
    double qualityScore;        // Output quality
    double collaborationScore;  // Teamwork effectiveness
    double communicationScore;  // Communication clarity
    double complianceScore;     // Rule adherence
    
    // Statistics
    int totalEvents;
    int positiveEvents;
    int negativeEvents;
    int neutralEvents;
    
    // Timestamps
    std::chrono::system_clock::time_point firstSeen;
    std::chrono::system_clock::time_point lastUpdated;
    
    // Confidence level (0.0 - 1.0) based on number of events
    double confidence;
    
    TrustScore() : TrustScore("") {}
    
    TrustScore(const AgentId& agent)
        : agentId(agent), overallScore(0.5), reliabilityScore(0.5),
          responsivenessScore(0.5), qualityScore(0.5), collaborationScore(0.5),
          communicationScore(0.5), complianceScore(0.5),
          totalEvents(0), positiveEvents(0), negativeEvents(0), neutralEvents(0),
          firstSeen(std::chrono::system_clock::now()),
          lastUpdated(std::chrono::system_clock::now()),
          confidence(0.0) {}
};

/**
 * Trust comparison result
 */
struct TrustComparison {
    AgentId agent1;
    AgentId agent2;
    double scoreDifference;  // agent1 - agent2
    std::string recommendation;
    std::vector<std::string> reasons;
};

/**
 * Anomaly detection result
 */
struct TrustAnomaly {
    AgentId agentId;
    std::string anomalyType;
    double severity;  // 0.0 - 1.0
    std::string description;
    std::chrono::system_clock::time_point detectedAt;
    std::vector<std::string> evidence;
};

/**
 * Trust decay configuration
 */
struct TrustDecayConfig {
    bool enabled = true;
    std::chrono::hours decayInterval = std::chrono::hours(24 * 7); // Weekly
    double decayRate = 0.05;  // 5% decay per interval
    double minimumScore = 0.1; // Don't decay below this
};

/**
 * Trust calculation configuration
 */
struct TrustConfig {
    // Component weights (must sum to 1.0)
    double reliabilityWeight = 0.25;
    double responsivenessWeight = 0.15;
    double qualityWeight = 0.20;
    double collaborationWeight = 0.20;
    double communicationWeight = 0.10;
    double complianceWeight = 0.10;
    
    // Decay settings
    TrustDecayConfig decay;
    
    // Anomaly detection thresholds
    double anomalyThreshold = 0.3;  // Score drop threshold
    int anomalyWindowEvents = 10;   // Look at recent N events
    
    // Confidence calculation
    int minEventsForConfidence = 10;
    int maxEventsForConfidence = 100;
};

/**
 * Main Trust Scoreboard class
 */
class TrustScoreboard {
public:
    TrustScoreboard(std::shared_ptr<AgentMemoryManager> memoryMgr,
                   const TrustConfig& config = TrustConfig());
    
    ~TrustScoreboard() = default;
    
    // Event recording
    void recordEvent(const AgentId& agentId, 
                    TrustEventType type, 
                    TrustOutcome outcome,
                    double impactScore = 0.0,
                    const std::string& context = "");
    
    void recordTaskCompletion(const AgentId& agentId, bool success, 
                             std::chrono::milliseconds responseTime);
    
    void recordCollaboration(const AgentId& agentId, bool positive);
    
    void recordCommunication(const AgentId& agentId, bool clear);
    
    void recordRuleViolation(const AgentId& agentId, const std::string& violation);
    
    // Score retrieval
    TrustScore getTrustScore(const AgentId& agentId);
    double getOverallScore(const AgentId& agentId);
    std::vector<TrustEvent> getEventHistory(const AgentId& agentId, int limit = 100);
    
    // Leaderboard
    std::vector<TrustScore> getLeaderboard(int limit = 10);
    std::vector<TrustScore> getTopAgents(int limit = 10);
    std::vector<TrustScore> getBottomAgents(int limit = 10);
    
    // Comparison
    TrustComparison compareAgents(const AgentId& agent1, const AgentId& agent2);
    AgentId selectMostTrusted(const std::vector<AgentId>& candidates);
    std::vector<AgentId> rankByTrust(const std::vector<AgentId>& agents);
    
    // Anomaly detection
    std::vector<TrustAnomaly> detectAnomalies();
    std::vector<TrustAnomaly> detectAnomaliesForAgent(const AgentId& agentId);
    bool isAnomalous(const AgentId& agentId);
    
    // Decay management
    void applyDecay();
    void applyDecayForAgent(const AgentId& agentId);
    
    // Statistics
    int getTotalAgents() const;
    int getTotalEvents() const;
    double getAverageTrustScore() const;
    
    // Configuration
    void updateConfig(const TrustConfig& newConfig);
    TrustConfig getConfig() const { return config_; }
    
    // Persistence
    bool saveTrustData();
    bool loadTrustData();
    
private:
    std::shared_ptr<AgentMemoryManager> memoryMgr_;
    TrustConfig config_;
    
    std::unordered_map<AgentId, TrustScore> scores_;
    std::unordered_map<AgentId, std::vector<TrustEvent>> eventHistory_;
    
    mutable std::mutex scoresMutex_;
    mutable std::mutex eventsMutex_;
    
    std::chrono::system_clock::time_point lastDecayTime_;
    
    // Helper methods
    void calculateScore(TrustScore& score, const std::vector<TrustEvent>& events);
    void updateComponentScores(TrustScore& score, const TrustEvent& event);
    double calculateConfidence(int eventCount) const;
    double applyTimeDecay(double score, std::chrono::system_clock::time_point lastUpdate) const;
    TrustOutcome determineOutcome(TrustEventType type, bool success) const;
    double calculateImpactScore(TrustEventType type, TrustOutcome outcome) const;
    bool detectAnomalyPattern(const std::vector<TrustEvent>& recentEvents) const;
    std::string generateEventId();
};

/**
 * Utility functions
 */
namespace trust_utils {
    std::string trustEventTypeToString(TrustEventType type);
    TrustEventType stringToTrustEventType(const std::string& typeStr);
    std::string trustOutcomeToString(TrustOutcome outcome);
    TrustOutcome stringToTrustOutcome(const std::string& outcomeStr);
    std::string formatTrustScore(double score);
    std::string getTrustLevel(double score);  // "Excellent", "Good", "Fair", "Poor"
}

} // namespace elizaos
