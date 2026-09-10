#include <gtest/gtest.h>

#include "elizaos/trust_scoreboard.hpp"

#include <atomic>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <limits>
#include <set>
#include <string>
#include <thread>
#include <vector>

using namespace elizaos;
namespace fs = std::filesystem;

namespace {

class TempDirectory {
public:
    TempDirectory() {
        static std::atomic<unsigned long> sequence{0UL};
        path = fs::temp_directory_path() /
            ("eliza-trust-scoreboard-" +
             std::to_string(sequence.fetch_add(1UL)));
        fs::create_directories(path);
    }
    ~TempDirectory() { std::error_code error; fs::remove_all(path, error); }
    fs::path path;
};

TrustEvent evidence(const std::string& id, const std::string& agent,
                    TrustEventType type, TrustOutcome outcome, double impact,
                    std::chrono::system_clock::time_point timestamp =
                        std::chrono::system_clock::now()) {
    TrustEvent event(agent, type, outcome, impact);
    event.eventId = id;
    event.timestamp = timestamp;
    event.context = "observed evidence";
    event.metadata.emplace("source", "unit-test");
    return event;
}

} // namespace

TEST(TrustScoreboardConstruction, DefaultsAreInitializedAndNoBackendFails) {
    TrustScoreboard board(nullptr);
    EXPECT_EQ(board.getTotalAgents(), 0);
    EXPECT_EQ(board.getTotalEvents(), 0);
    EXPECT_DOUBLE_EQ(board.getAverageTrustScore(), 0.5);
    EXPECT_FALSE(board.saveTrustData());
    EXPECT_FALSE(board.loadTrustData());
    EXPECT_TRUE(board.getPersistencePath().empty());
}

TEST(TrustScoreboardConfig, InvalidConstructorConfigurationThrows) {
    TrustConfig config;
    config.anomalyThreshold = std::numeric_limits<double>::quiet_NaN();
    EXPECT_THROW((TrustScoreboard(nullptr, config)), std::invalid_argument);

    config = TrustConfig{};
    config.decay.decayInterval = std::chrono::hours(0);
    EXPECT_THROW((TrustScoreboard(nullptr, config)), std::invalid_argument);
}

TEST(TrustScoreboardConfig, UpdateNormalizesWeightsAndRollsBackInvalidInput) {
    TrustScoreboard board(nullptr);
    TrustConfig config;
    config.reliabilityWeight = 2.0;
    config.responsivenessWeight = 1.0;
    config.qualityWeight = 1.0;
    config.collaborationWeight = 0.0;
    config.communicationWeight = 0.0;
    config.complianceWeight = 0.0;
    ASSERT_TRUE(board.updateConfig(config));
    const auto normalized = board.getConfig();
    EXPECT_DOUBLE_EQ(normalized.reliabilityWeight, 0.5);
    EXPECT_DOUBLE_EQ(normalized.responsivenessWeight, 0.25);
    EXPECT_DOUBLE_EQ(normalized.qualityWeight, 0.25);

    config.anomalyWindowEvents = 4;
    EXPECT_FALSE(board.updateConfig(config));
    EXPECT_DOUBLE_EQ(board.getConfig().reliabilityWeight, 0.5);

    config = normalized;
    config.reliabilityWeight = -1.0;
    EXPECT_FALSE(board.updateConfig(config));
    config = normalized;
    config.maxEventsForConfidence = config.minEventsForConfidence - 1;
    EXPECT_FALSE(board.updateConfig(config));
    config = normalized;
    config.decay.decayRate = 1.1;
    EXPECT_FALSE(board.updateConfig(config));
}

TEST(TrustScoreboardEvidence, RejectsInvalidIdentifiersValuesAndSemantics) {
    TrustScoreboard board(nullptr);
    EXPECT_FALSE(board.recordEvent("", TrustEventType::TASK_COMPLETED,
                                   TrustOutcome::POSITIVE, 0.2));
    EXPECT_FALSE(board.recordEvent("agent", TrustEventType::TASK_COMPLETED,
                                   TrustOutcome::POSITIVE,
                                   std::numeric_limits<double>::infinity()));
    EXPECT_FALSE(board.recordEvent("agent", TrustEventType::TASK_COMPLETED,
                                   TrustOutcome::POSITIVE, -0.2));
    EXPECT_FALSE(board.recordEvent("agent", TrustEventType::TASK_COMPLETED,
                                   TrustOutcome::NEUTRAL, 0.2));
    EXPECT_FALSE(board.recordEvent("agent", TrustEventType::TASK_FAILED,
                                   TrustOutcome::POSITIVE, 0.2));

    auto invalidId = evidence("bad id", "agent",
                              TrustEventType::TASK_COMPLETED,
                              TrustOutcome::POSITIVE, 0.2);
    EXPECT_FALSE(board.recordEvent(invalidId));
    auto future = evidence("future", "agent", TrustEventType::TASK_COMPLETED,
                           TrustOutcome::POSITIVE, 0.2,
                           std::chrono::system_clock::now() +
                               std::chrono::hours(25));
    EXPECT_FALSE(board.recordEvent(future));
    EXPECT_FALSE(board.recordTaskCompletion("agent", true,
                                            std::chrono::milliseconds(-1)));
    EXPECT_FALSE(board.recordRuleViolation("agent", "   "));
    EXPECT_EQ(board.getTotalEvents(), 0);
}

TEST(TrustScoreboardEvidence, ExplicitEvidenceIsStoredAndReplayRejected) {
    TrustScoreboard board(nullptr);
    auto event = evidence("receipt-001", "agent-a",
                          TrustEventType::HELPFUL_ACTION,
                          TrustOutcome::POSITIVE, 0.8);
    ASSERT_TRUE(board.recordEvent(event));
    EXPECT_FALSE(board.recordEvent(event));
    ASSERT_EQ(board.getTotalEvents(), 1);
    const auto history = board.getEventHistory("agent-a", 10);
    ASSERT_EQ(history.size(), 1U);
    EXPECT_EQ(history.front().eventId, "receipt-001");
    EXPECT_EQ(history.front().context, "observed evidence");
    EXPECT_EQ(history.front().metadata.at("source"), "unit-test");
}

TEST(TrustScoreboardEvidence, ConvenienceIdsAreUniqueUnderConcurrency) {
    TrustScoreboard board(nullptr);
    constexpr int threadCount = 8;
    static constexpr int eventsPerThread = 100;
    std::vector<std::thread> threads;
    for (int thread = 0; thread < threadCount; ++thread) {
        threads.emplace_back([&board, thread] {
            const auto agent = "agent-" + std::to_string(thread);
            for (int event = 0; event < eventsPerThread; ++event) {
                EXPECT_TRUE(board.recordCommunication(agent, true));
            }
        });
    }
    for (auto& thread : threads) thread.join();
    EXPECT_EQ(board.getTotalEvents(), threadCount * eventsPerThread);
    std::set<std::string> identifiers;
    for (int thread = 0; thread < threadCount; ++thread) {
        for (const auto& event : board.getEventHistory(
                 "agent-" + std::to_string(thread), eventsPerThread)) {
            EXPECT_TRUE(identifiers.insert(event.eventId).second);
        }
    }
    EXPECT_EQ(identifiers.size(),
              static_cast<std::size_t>(threadCount * eventsPerThread));
}

TEST(TrustScoreboardHistory, IsChronologicalLimitedAndBounded) {
    TrustConfig config;
    config.maxEventHistoryPerAgent = 3U;
    TrustScoreboard board(nullptr, config);
    const auto base = std::chrono::system_clock::now() -
                      std::chrono::minutes(10);
    ASSERT_TRUE(board.recordEvent(evidence("e3", "agent",
        TrustEventType::TASK_COMPLETED, TrustOutcome::POSITIVE, 0.2,
        base + std::chrono::minutes(3))));
    ASSERT_TRUE(board.recordEvent(evidence("e1", "agent",
        TrustEventType::TASK_COMPLETED, TrustOutcome::POSITIVE, 0.2,
        base + std::chrono::minutes(1))));
    ASSERT_TRUE(board.recordEvent(evidence("e2", "agent",
        TrustEventType::TASK_COMPLETED, TrustOutcome::POSITIVE, 0.2,
        base + std::chrono::minutes(2))));
    ASSERT_TRUE(board.recordEvent(evidence("e4", "agent",
        TrustEventType::TASK_COMPLETED, TrustOutcome::POSITIVE, 0.2,
        base + std::chrono::minutes(4))));
    const auto all = board.getEventHistory("agent", 100);
    ASSERT_EQ(all.size(), 3U);
    EXPECT_EQ(all[0].eventId, "e2");
    EXPECT_EQ(all[1].eventId, "e3");
    EXPECT_EQ(all[2].eventId, "e4");
    const auto recent = board.getEventHistory("agent", 2);
    ASSERT_EQ(recent.size(), 2U);
    EXPECT_EQ(recent[0].eventId, "e3");
    EXPECT_EQ(recent[1].eventId, "e4");
    EXPECT_TRUE(board.getEventHistory("agent", 0).empty());
    EXPECT_EQ(board.getTrustScore("agent").totalEvents, 3);
}

TEST(TrustScoreboardScoring, UsesActualRecordsForComponentsAndStatistics) {
    TrustScoreboard board(nullptr);
    ASSERT_TRUE(board.recordEvent("agent", TrustEventType::TASK_COMPLETED,
                                  TrustOutcome::POSITIVE, 1.0));
    ASSERT_TRUE(board.recordEvent("agent", TrustEventType::HARMFUL_ACTION,
                                  TrustOutcome::NEGATIVE, -1.0));
    ASSERT_TRUE(board.recordEvent("agent", TrustEventType::RESPONSE_FAST,
                                  TrustOutcome::NEUTRAL, 0.0));
    const auto score = board.getTrustScore("agent");
    EXPECT_DOUBLE_EQ(score.reliabilityScore, 0.6);
    EXPECT_DOUBLE_EQ(score.qualityScore, 0.4);
    EXPECT_DOUBLE_EQ(score.responsivenessScore, 0.5);
    EXPECT_EQ(score.totalEvents, 3);
    EXPECT_EQ(score.positiveEvents, 1);
    EXPECT_EQ(score.negativeEvents, 1);
    EXPECT_EQ(score.neutralEvents, 1);
    EXPECT_DOUBLE_EQ(score.confidence, 0.15);
    EXPECT_TRUE(std::isfinite(score.overallScore));
}

TEST(TrustScoreboardOrdering, LeaderboardsRanksSelectionsAndTiesAreStable) {
    TrustScoreboard board(nullptr);
    ASSERT_TRUE(board.recordEvent("zeta", TrustEventType::TASK_COMPLETED,
                                  TrustOutcome::POSITIVE, 1.0));
    ASSERT_TRUE(board.recordEvent("alpha", TrustEventType::TASK_COMPLETED,
                                  TrustOutcome::POSITIVE, 1.0));
    ASSERT_TRUE(board.recordEvent("low", TrustEventType::TASK_FAILED,
                                  TrustOutcome::NEGATIVE, -1.0));

    const auto leaders = board.getLeaderboard(10);
    ASSERT_EQ(leaders.size(), 3U);
    EXPECT_EQ(leaders[0].agentId, "alpha");
    EXPECT_EQ(leaders[1].agentId, "zeta");
    EXPECT_EQ(leaders[2].agentId, "low");
    EXPECT_TRUE(board.getLeaderboard(0).empty());

    const auto bottom = board.getBottomAgents(10);
    ASSERT_EQ(bottom.size(), 3U);
    EXPECT_EQ(bottom.front().agentId, "low");
    EXPECT_EQ(board.selectMostTrusted({"zeta", "alpha", "zeta", ""}),
              "alpha");
    EXPECT_TRUE(board.selectMostTrusted({"", " "}).empty());
    EXPECT_EQ(board.rankByTrust({"low", "zeta", "alpha", "alpha"}),
              (std::vector<AgentId>{"alpha", "zeta", "low"}));

    const auto tied = board.compareAgents("alpha", "zeta");
    EXPECT_DOUBLE_EQ(tied.scoreDifference, 0.0);
    EXPECT_EQ(tied.recommendation, "Agents are tied");
    const auto winner = board.compareAgents("alpha", "low");
    EXPECT_GT(winner.scoreDifference, 0.0);
    EXPECT_FALSE(winner.reasons.empty());
}

TEST(TrustScoreboardAnomaly, UsesRecentRecordsAndReportsEvidenceIds) {
    TrustConfig config;
    config.anomalyThreshold = 0.4;
    config.anomalyWindowEvents = 5;
    TrustScoreboard board(nullptr, config);
    ASSERT_TRUE(board.recordEvent(evidence("p1", "agent",
        TrustEventType::TASK_COMPLETED, TrustOutcome::POSITIVE, 0.2)));
    ASSERT_TRUE(board.recordEvent(evidence("n1", "agent",
        TrustEventType::TASK_FAILED, TrustOutcome::NEGATIVE, -0.3)));
    ASSERT_TRUE(board.recordEvent(evidence("n2", "agent",
        TrustEventType::RULE_VIOLATION, TrustOutcome::NEGATIVE, -0.5)));
    ASSERT_TRUE(board.recordEvent(evidence("p2", "agent",
        TrustEventType::TASK_COMPLETED, TrustOutcome::POSITIVE, 0.2)));
    ASSERT_TRUE(board.recordEvent(evidence("p3", "agent",
        TrustEventType::TASK_COMPLETED, TrustOutcome::POSITIVE, 0.2)));

    const auto anomalies = board.detectAnomaliesForAgent("agent");
    ASSERT_EQ(anomalies.size(), 2U);
    EXPECT_EQ(anomalies[0].anomalyType, "high_negative_event_rate");
    EXPECT_EQ(anomalies[0].evidence,
              (std::vector<std::string>{"n1", "n2"}));
    EXPECT_EQ(anomalies[1].anomalyType, "rule_violations");
    EXPECT_EQ(anomalies[1].evidence,
              (std::vector<std::string>{"n2"}));
    EXPECT_TRUE(board.isAnomalous("agent"));
    EXPECT_EQ(board.detectAnomalies().size(), 2U);
}

TEST(TrustScoreboardPersistence, RejectsUnsafePathsAndRoundTripsDeterministically) {
    TempDirectory temporary;
    const fs::path file = temporary.path / "trust.json";
    TrustScoreboard board(nullptr);
    EXPECT_FALSE(board.setPersistencePath(temporary.path / "missing" /
                                          "trust.json"));
    EXPECT_FALSE(board.setPersistencePath(temporary.path / ".." /
                                          "escape.json"));
    ASSERT_TRUE(board.setPersistencePath(file));
    ASSERT_TRUE(board.recordEvent(evidence("persist-2", "b",
        TrustEventType::TASK_FAILED, TrustOutcome::NEGATIVE, -0.3)));
    ASSERT_TRUE(board.recordEvent(evidence("persist-1", "a",
        TrustEventType::TASK_COMPLETED, TrustOutcome::POSITIVE, 0.2)));
    ASSERT_TRUE(board.saveTrustData());
    std::ifstream firstInput(file);
    const std::string first((std::istreambuf_iterator<char>(firstInput)), {});
    ASSERT_TRUE(board.saveTrustData());
    std::ifstream secondInput(file);
    const std::string second((std::istreambuf_iterator<char>(secondInput)), {});
    EXPECT_EQ(first, second);

    TrustScoreboard loaded(nullptr, TrustConfig{}, file);
    ASSERT_TRUE(loaded.loadTrustData());
    EXPECT_EQ(loaded.getTotalAgents(), 2);
    EXPECT_EQ(loaded.getTotalEvents(), 2);
    EXPECT_EQ(loaded.getEventHistory("a", 10).front().eventId, "persist-1");
    EXPECT_DOUBLE_EQ(loaded.getOverallScore("a"), board.getOverallScore("a"));
}

TEST(TrustScoreboardPersistence, MalformedLoadRollsBackExistingState) {
    TempDirectory temporary;
    const fs::path file = temporary.path / "trust.json";
    TrustScoreboard board(nullptr, TrustConfig{}, file);
    ASSERT_TRUE(board.recordEvent(evidence("existing", "agent",
        TrustEventType::TASK_COMPLETED, TrustOutcome::POSITIVE, 0.2)));
    const auto before = board.getTrustScore("agent");
    {
        std::ofstream output(file);
        output << "{ malformed";
    }
    EXPECT_FALSE(board.loadTrustData());
    EXPECT_EQ(board.getTotalEvents(), 1);
    EXPECT_DOUBLE_EQ(board.getTrustScore("agent").overallScore,
                     before.overallScore);

    {
        std::ofstream output(file);
        output << R"({"schema":"elizaos.trust_scoreboard","version":999})";
    }
    EXPECT_FALSE(board.loadTrustData());
    EXPECT_EQ(board.getEventHistory("agent", 10).front().eventId, "existing");
}

TEST(TrustScoreboardUtilities, ConvertsStrictlyAndClampsNonFiniteFormatting) {
    const std::vector<TrustEventType> types{
        TrustEventType::TASK_COMPLETED, TrustEventType::TASK_FAILED,
        TrustEventType::TASK_TIMEOUT, TrustEventType::RESPONSE_FAST,
        TrustEventType::RESPONSE_SLOW, TrustEventType::COLLABORATION_GOOD,
        TrustEventType::COLLABORATION_POOR, TrustEventType::COMMUNICATION_CLEAR,
        TrustEventType::COMMUNICATION_UNCLEAR, TrustEventType::RULE_VIOLATION,
        TrustEventType::HELPFUL_ACTION, TrustEventType::HARMFUL_ACTION};
    for (const auto type : types) {
        EXPECT_EQ(trust_utils::stringToTrustEventType(
                      trust_utils::trustEventTypeToString(type)), type);
    }
    EXPECT_THROW(trust_utils::stringToTrustEventType("unknown"),
                 std::invalid_argument);
    EXPECT_EQ(trust_utils::stringToTrustOutcome("Positive"),
              TrustOutcome::POSITIVE);
    EXPECT_THROW(trust_utils::stringToTrustOutcome("unknown"),
                 std::invalid_argument);
    EXPECT_EQ(trust_utils::formatTrustScore(0.856), "85.60%");
    EXPECT_EQ(trust_utils::formatTrustScore(
                  std::numeric_limits<double>::quiet_NaN()), "50.00%");
    EXPECT_EQ(trust_utils::getTrustLevel(0.8), "Excellent");
    EXPECT_EQ(trust_utils::getTrustLevel(0.2), "Poor");
}

TEST(TrustScoreboardLifecycle, RepeatedConstructionQueryAndDestructionIsSafe) {
    for (int iteration = 0; iteration < 500; ++iteration) {
        TrustScoreboard board(nullptr);
        ASSERT_TRUE(board.recordCollaboration("agent", true));
        EXPECT_EQ(board.getTotalAgents(), 1);
        EXPECT_TRUE(std::isfinite(board.getAverageTrustScore()));
    }
}
