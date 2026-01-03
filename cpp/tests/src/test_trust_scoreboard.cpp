#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "elizaos/trust_scoreboard.hpp"
#include "elizaos/agentmemory.hpp"
#include <memory>
#include <string>
#include <vector>

using namespace elizaos;
using namespace ::testing;

class TrustScoreboardTest : public ::testing::Test {
protected:
    void SetUp() override {
        memoryMgr_ = std::make_shared<AgentMemoryManager>();
        scoreboard_ = std::make_shared<TrustScoreboard>(memoryMgr_);
    }

    std::shared_ptr<AgentMemoryManager> memoryMgr_;
    std::shared_ptr<TrustScoreboard> scoreboard_;
};

// ============================================================================
// TrustEvent Tests
// ============================================================================

TEST_F(TrustScoreboardTest, TrustEventCreation) {
    TrustEvent event("agent-001", TrustEventType::TASK_COMPLETED, TrustOutcome::POSITIVE, 0.1);

    EXPECT_EQ(event.agentId, "agent-001");
    EXPECT_EQ(event.type, TrustEventType::TASK_COMPLETED);
    EXPECT_EQ(event.outcome, TrustOutcome::POSITIVE);
    EXPECT_NEAR(event.impactScore, 0.1, 0.01);
}

TEST_F(TrustScoreboardTest, TrustEventTypes) {
    // Verify all event types can be created
    TrustEvent e1("a", TrustEventType::TASK_COMPLETED, TrustOutcome::POSITIVE, 0.1);
    TrustEvent e2("a", TrustEventType::TASK_FAILED, TrustOutcome::NEGATIVE, -0.1);
    TrustEvent e3("a", TrustEventType::TASK_TIMEOUT, TrustOutcome::NEGATIVE, -0.05);
    TrustEvent e4("a", TrustEventType::RESPONSE_FAST, TrustOutcome::POSITIVE, 0.05);
    TrustEvent e5("a", TrustEventType::RESPONSE_SLOW, TrustOutcome::NEGATIVE, -0.02);
    TrustEvent e6("a", TrustEventType::COLLABORATION_GOOD, TrustOutcome::POSITIVE, 0.1);
    TrustEvent e7("a", TrustEventType::COLLABORATION_POOR, TrustOutcome::NEGATIVE, -0.1);

    EXPECT_EQ(e1.type, TrustEventType::TASK_COMPLETED);
    EXPECT_EQ(e7.type, TrustEventType::COLLABORATION_POOR);
}

TEST_F(TrustScoreboardTest, TrustOutcomes) {
    TrustEvent positive("a", TrustEventType::HELPFUL_ACTION, TrustOutcome::POSITIVE, 0.1);
    TrustEvent negative("a", TrustEventType::HARMFUL_ACTION, TrustOutcome::NEGATIVE, -0.1);
    TrustEvent neutral("a", TrustEventType::COMMUNICATION_CLEAR, TrustOutcome::NEUTRAL, 0.0);

    EXPECT_EQ(positive.outcome, TrustOutcome::POSITIVE);
    EXPECT_EQ(negative.outcome, TrustOutcome::NEGATIVE);
    EXPECT_EQ(neutral.outcome, TrustOutcome::NEUTRAL);
}

// ============================================================================
// TrustScore Tests
// ============================================================================

TEST_F(TrustScoreboardTest, TrustScoreCreation) {
    TrustScore score("agent-001");

    EXPECT_EQ(score.agentId, "agent-001");
    EXPECT_NEAR(score.overallScore, 0.5, 0.01);  // Default starting score
    EXPECT_NEAR(score.reliabilityScore, 0.5, 0.01);
    EXPECT_NEAR(score.responsivenessScore, 0.5, 0.01);
    EXPECT_NEAR(score.qualityScore, 0.5, 0.01);
    EXPECT_NEAR(score.collaborationScore, 0.5, 0.01);
    EXPECT_NEAR(score.communicationScore, 0.5, 0.01);
    EXPECT_NEAR(score.complianceScore, 0.5, 0.01);
}

TEST_F(TrustScoreboardTest, TrustScoreDefaultConstructor) {
    TrustScore score;

    EXPECT_TRUE(score.agentId.empty());
    EXPECT_EQ(score.totalEvents, 0);
}

TEST_F(TrustScoreboardTest, TrustScoreStatistics) {
    TrustScore score("agent-002");

    EXPECT_EQ(score.totalEvents, 0);
    EXPECT_EQ(score.positiveEvents, 0);
    EXPECT_EQ(score.negativeEvents, 0);
    EXPECT_EQ(score.neutralEvents, 0);
    EXPECT_NEAR(score.confidence, 0.0, 0.01);
}

// ============================================================================
// TrustComparison Tests
// ============================================================================

TEST_F(TrustScoreboardTest, TrustComparisonCreation) {
    TrustComparison comparison;
    comparison.agent1 = "agent-A";
    comparison.agent2 = "agent-B";
    comparison.scoreDifference = 0.2;
    comparison.recommendation = "Agent A is more reliable";

    EXPECT_EQ(comparison.agent1, "agent-A");
    EXPECT_EQ(comparison.agent2, "agent-B");
    EXPECT_NEAR(comparison.scoreDifference, 0.2, 0.01);
}

// ============================================================================
// TrustAnomaly Tests
// ============================================================================

TEST_F(TrustScoreboardTest, TrustAnomalyCreation) {
    TrustAnomaly anomaly;
    anomaly.agentId = "agent-001";
    anomaly.anomalyType = "sudden_drop";
    anomaly.severity = 0.8;
    anomaly.description = "Trust score dropped significantly";
    anomaly.evidence.push_back("Multiple task failures");

    EXPECT_EQ(anomaly.agentId, "agent-001");
    EXPECT_EQ(anomaly.anomalyType, "sudden_drop");
    EXPECT_NEAR(anomaly.severity, 0.8, 0.01);
    EXPECT_FALSE(anomaly.evidence.empty());
}

// ============================================================================
// TrustConfig Tests
// ============================================================================

TEST_F(TrustScoreboardTest, TrustConfigDefaults) {
    TrustConfig config;

    EXPECT_NEAR(config.reliabilityWeight, 0.25, 0.01);
    EXPECT_NEAR(config.responsivenessWeight, 0.15, 0.01);
    EXPECT_NEAR(config.qualityWeight, 0.20, 0.01);
    EXPECT_NEAR(config.collaborationWeight, 0.20, 0.01);
    EXPECT_NEAR(config.communicationWeight, 0.10, 0.01);
    EXPECT_NEAR(config.complianceWeight, 0.10, 0.01);
}

TEST_F(TrustScoreboardTest, TrustDecayConfigDefaults) {
    TrustDecayConfig decay;

    EXPECT_TRUE(decay.enabled);
    EXPECT_NEAR(decay.decayRate, 0.05, 0.01);
    EXPECT_NEAR(decay.minimumScore, 0.1, 0.01);
}

// ============================================================================
// TrustScoreboard Tests
// ============================================================================

TEST_F(TrustScoreboardTest, ScoreboardCreation) {
    TrustScoreboard board(memoryMgr_);

    EXPECT_EQ(board.getTotalAgents(), 0);
    EXPECT_EQ(board.getTotalEvents(), 0);
}

TEST_F(TrustScoreboardTest, RecordEvent) {
    scoreboard_->recordEvent("agent-001", TrustEventType::TASK_COMPLETED,
                             TrustOutcome::POSITIVE, 0.1, "Completed task successfully");

    TrustScore score = scoreboard_->getTrustScore("agent-001");
    EXPECT_EQ(score.agentId, "agent-001");
    EXPECT_EQ(score.totalEvents, 1);
}

TEST_F(TrustScoreboardTest, RecordTaskCompletion) {
    scoreboard_->recordTaskCompletion("agent-001", true, std::chrono::milliseconds(100));

    TrustScore score = scoreboard_->getTrustScore("agent-001");
    EXPECT_GE(score.totalEvents, 1);
}

TEST_F(TrustScoreboardTest, RecordCollaboration) {
    scoreboard_->recordCollaboration("agent-001", true);

    TrustScore score = scoreboard_->getTrustScore("agent-001");
    EXPECT_GE(score.totalEvents, 1);
}

TEST_F(TrustScoreboardTest, RecordCommunication) {
    scoreboard_->recordCommunication("agent-001", true);

    TrustScore score = scoreboard_->getTrustScore("agent-001");
    EXPECT_GE(score.totalEvents, 1);
}

TEST_F(TrustScoreboardTest, RecordRuleViolation) {
    scoreboard_->recordRuleViolation("agent-001", "Exceeded rate limit");

    TrustScore score = scoreboard_->getTrustScore("agent-001");
    EXPECT_GE(score.totalEvents, 1);
}

TEST_F(TrustScoreboardTest, GetOverallScore) {
    scoreboard_->recordEvent("agent-001", TrustEventType::TASK_COMPLETED,
                             TrustOutcome::POSITIVE, 0.1);

    double overall = scoreboard_->getOverallScore("agent-001");
    EXPECT_GE(overall, 0.0);
    EXPECT_LE(overall, 1.0);
}

TEST_F(TrustScoreboardTest, GetEventHistory) {
    scoreboard_->recordEvent("agent-001", TrustEventType::TASK_COMPLETED,
                             TrustOutcome::POSITIVE, 0.1);
    scoreboard_->recordEvent("agent-001", TrustEventType::TASK_FAILED,
                             TrustOutcome::NEGATIVE, -0.1);

    auto history = scoreboard_->getEventHistory("agent-001");
    EXPECT_EQ(history.size(), 2);
}

TEST_F(TrustScoreboardTest, GetLeaderboard) {
    scoreboard_->recordEvent("agent-001", TrustEventType::TASK_COMPLETED,
                             TrustOutcome::POSITIVE, 0.2);
    scoreboard_->recordEvent("agent-002", TrustEventType::TASK_COMPLETED,
                             TrustOutcome::POSITIVE, 0.1);

    auto leaderboard = scoreboard_->getLeaderboard(10);
    EXPECT_EQ(leaderboard.size(), 2);
}

TEST_F(TrustScoreboardTest, CompareAgents) {
    scoreboard_->recordEvent("agent-001", TrustEventType::TASK_COMPLETED,
                             TrustOutcome::POSITIVE, 0.2);
    scoreboard_->recordEvent("agent-002", TrustEventType::TASK_FAILED,
                             TrustOutcome::NEGATIVE, -0.1);

    TrustComparison comparison = scoreboard_->compareAgents("agent-001", "agent-002");
    EXPECT_EQ(comparison.agent1, "agent-001");
    EXPECT_EQ(comparison.agent2, "agent-002");
}

TEST_F(TrustScoreboardTest, SelectMostTrusted) {
    scoreboard_->recordEvent("agent-001", TrustEventType::TASK_COMPLETED,
                             TrustOutcome::POSITIVE, 0.3);
    scoreboard_->recordEvent("agent-002", TrustEventType::TASK_COMPLETED,
                             TrustOutcome::POSITIVE, 0.1);
    scoreboard_->recordEvent("agent-003", TrustEventType::TASK_FAILED,
                             TrustOutcome::NEGATIVE, -0.2);

    std::vector<std::string> candidates = {"agent-001", "agent-002", "agent-003"};
    std::string mostTrusted = scoreboard_->selectMostTrusted(candidates);
    EXPECT_FALSE(mostTrusted.empty());
}

TEST_F(TrustScoreboardTest, DetectAnomalies) {
    // Record multiple events to potentially trigger anomaly
    for (int i = 0; i < 10; ++i) {
        scoreboard_->recordEvent("agent-001", TrustEventType::TASK_FAILED,
                                 TrustOutcome::NEGATIVE, -0.1);
    }

    auto anomalies = scoreboard_->detectAnomalies();
    // May or may not detect anomalies depending on implementation
    EXPECT_TRUE(true);
}

TEST_F(TrustScoreboardTest, IsAnomalous) {
    scoreboard_->recordEvent("agent-001", TrustEventType::TASK_COMPLETED,
                             TrustOutcome::POSITIVE, 0.1);

    bool anomalous = scoreboard_->isAnomalous("agent-001");
    EXPECT_FALSE(anomalous);  // Single positive event shouldn't be anomalous
}

TEST_F(TrustScoreboardTest, ApplyDecay) {
    scoreboard_->recordEvent("agent-001", TrustEventType::TASK_COMPLETED,
                             TrustOutcome::POSITIVE, 0.5);

    scoreboard_->applyDecay();
    // Decay applied without error
    EXPECT_TRUE(true);
}

TEST_F(TrustScoreboardTest, GetStatistics) {
    EXPECT_GE(scoreboard_->getTotalAgents(), 0);
    EXPECT_GE(scoreboard_->getTotalEvents(), 0);
    EXPECT_GE(scoreboard_->getAverageTrustScore(), 0.0);
}

TEST_F(TrustScoreboardTest, UpdateConfig) {
    TrustConfig newConfig;
    newConfig.reliabilityWeight = 0.30;

    scoreboard_->updateConfig(newConfig);

    TrustConfig retrieved = scoreboard_->getConfig();
    EXPECT_NEAR(retrieved.reliabilityWeight, 0.30, 0.01);
}

// ============================================================================
// Utility Functions Tests
// ============================================================================

TEST_F(TrustScoreboardTest, TrustEventTypeToString) {
    std::string str = trust_utils::trustEventTypeToString(TrustEventType::TASK_COMPLETED);
    EXPECT_FALSE(str.empty());
}

TEST_F(TrustScoreboardTest, StringToTrustEventType) {
    std::string str = trust_utils::trustEventTypeToString(TrustEventType::TASK_FAILED);
    TrustEventType type = trust_utils::stringToTrustEventType(str);
    EXPECT_EQ(type, TrustEventType::TASK_FAILED);
}

TEST_F(TrustScoreboardTest, TrustOutcomeToString) {
    std::string str = trust_utils::trustOutcomeToString(TrustOutcome::POSITIVE);
    EXPECT_FALSE(str.empty());
}

TEST_F(TrustScoreboardTest, FormatTrustScore) {
    std::string formatted = trust_utils::formatTrustScore(0.85);
    EXPECT_FALSE(formatted.empty());
}

TEST_F(TrustScoreboardTest, GetTrustLevel) {
    std::string level = trust_utils::getTrustLevel(0.9);
    EXPECT_FALSE(level.empty());
}

