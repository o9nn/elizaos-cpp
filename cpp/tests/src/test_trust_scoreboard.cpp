#include <gtest/gtest.h>

#include "elizaos/trust_scoreboard.hpp"

#include <atomic>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <thread>
#include <vector>

using namespace elizaos;
namespace fs = std::filesystem;

namespace {

fs::path tempPath(const std::string& name) {
    static std::atomic<unsigned long> sequence{0UL};
    static const auto processNonce = [] {
        std::random_device random;
        return (static_cast<unsigned long long>(random()) << 32U) ^
               static_cast<unsigned long long>(random());
    }();
    const auto directory = fs::temp_directory_path() /
        ("eliza-trust-integration-" +
         std::to_string(processNonce) + "-" +
         std::to_string(sequence.fetch_add(1UL)));
    fs::create_directories(directory);
    return directory / name;
}

TrustEvent observed(const std::string& id, const std::string& agent,
                    TrustEventType type, TrustOutcome outcome, double impact,
                    std::chrono::system_clock::time_point when) {
    TrustEvent event(agent, type, outcome, impact);
    event.eventId = id;
    event.timestamp = when;
    event.context = "integration evidence";
    return event;
}

} // namespace

TEST(TrustScoreboardComprehensive, EveryConvenienceRecorderCreatesOneEvidenceRow) {
    TrustScoreboard board(nullptr);
    EXPECT_TRUE(board.recordTaskCompletion("agent", true,
                                           std::chrono::milliseconds(100)));
    EXPECT_TRUE(board.recordTaskCompletion("agent", false,
                                           std::chrono::milliseconds(9000)));
    EXPECT_TRUE(board.recordCollaboration("agent", true));
    EXPECT_TRUE(board.recordCollaboration("agent", false));
    EXPECT_TRUE(board.recordCommunication("agent", true));
    EXPECT_TRUE(board.recordCommunication("agent", false));
    EXPECT_TRUE(board.recordRuleViolation("agent", "documented breach"));
    EXPECT_EQ(board.getTotalEvents(), 7);
    EXPECT_EQ(board.getTrustScore("agent").totalEvents, 7);
    const auto history = board.getEventHistory("agent", 100);
    for (const auto& event : history) {
        EXPECT_FALSE(event.eventId.empty());
        EXPECT_EQ(event.agentId, "agent");
        EXPECT_TRUE(std::isfinite(event.impactScore));
    }
}

TEST(TrustScoreboardComprehensive, DecayUsesElapsedIntervalsAndMinimumFloor) {
    TrustConfig config;
    config.decay.enabled = true;
    config.decay.decayInterval = std::chrono::hours(1);
    config.decay.decayRate = 0.5;
    config.decay.minimumScore = 0.2;
    TrustScoreboard board(nullptr, config);
    const auto old = std::chrono::system_clock::now() - std::chrono::hours(4);
    ASSERT_TRUE(board.recordEvent(observed("old", "agent",
        TrustEventType::TASK_COMPLETED, TrustOutcome::POSITIVE, 1.0, old)));
    const auto decayed = board.getOverallScore("agent");
    EXPECT_GE(decayed, 0.2);
    EXPECT_LT(decayed, 0.3);
    board.applyDecayForAgent("agent");
    EXPECT_NEAR(board.getOverallScore("agent"), decayed, 0.01);
    board.applyDecay();
    EXPECT_GE(board.getOverallScore("agent"), 0.2);
}

TEST(TrustScoreboardComprehensive, ConfigUpdateRecalculatesExistingRecords) {
    TrustScoreboard board(nullptr);
    ASSERT_TRUE(board.recordEvent("agent", TrustEventType::HELPFUL_ACTION,
                                  TrustOutcome::POSITIVE, 1.0));
    const double before = board.getOverallScore("agent");
    TrustConfig config = board.getConfig();
    config.reliabilityWeight = 0.0;
    config.responsivenessWeight = 0.0;
    config.qualityWeight = 1.0;
    config.collaborationWeight = 0.0;
    config.communicationWeight = 0.0;
    config.complianceWeight = 0.0;
    ASSERT_TRUE(board.updateConfig(config));
    EXPECT_GT(board.getOverallScore("agent"), before);
}

TEST(TrustScoreboardComprehensive, PersistedReplayIdsRemainRejectedAfterLoad) {
    const fs::path path = tempPath("trust.json");
    TrustScoreboard source(nullptr, TrustConfig{}, path);
    const auto timestamp = std::chrono::system_clock::now();
    auto event = observed("external-receipt", "agent",
        TrustEventType::TASK_COMPLETED, TrustOutcome::POSITIVE, 0.5, timestamp);
    ASSERT_TRUE(source.recordEvent(event));
    ASSERT_TRUE(source.saveTrustData());

    TrustScoreboard loaded(nullptr, TrustConfig{}, path);
    ASSERT_TRUE(loaded.loadTrustData());
    EXPECT_FALSE(loaded.recordEvent(event));
    EXPECT_EQ(loaded.getTotalEvents(), 1);
    std::error_code error;
    fs::remove_all(path.parent_path(), error);
}

TEST(TrustScoreboardComprehensive, StrictLoadRejectsDuplicateEvidenceTransactionally) {
    const fs::path path = tempPath("trust.json");
    TrustScoreboard source(nullptr, TrustConfig{}, path);
    ASSERT_TRUE(source.recordEvent("source", TrustEventType::TASK_COMPLETED,
                                   TrustOutcome::POSITIVE, 0.2));
    ASSERT_TRUE(source.saveTrustData());

    std::ifstream input(path);
    std::string document((std::istreambuf_iterator<char>(input)), {});
    const auto eventsEnd = document.rfind("\n  ]");
    ASSERT_NE(eventsEnd, std::string::npos);
    const auto eventStart = document.find("    {");
    ASSERT_NE(eventStart, std::string::npos);
    const auto eventEnd = document.find("\n    }", eventStart);
    ASSERT_NE(eventEnd, std::string::npos);
    const auto encodedEvent = document.substr(
        eventStart, eventEnd + std::string("\n    }").size() - eventStart);
    document.insert(eventsEnd, ",\n" + encodedEvent);
    {
        std::ofstream output(path, std::ios::trunc);
        output << document;
    }

    TrustScoreboard target(nullptr, TrustConfig{}, path);
    ASSERT_TRUE(target.recordEvent("existing", TrustEventType::TASK_FAILED,
                                   TrustOutcome::NEGATIVE, -0.2));
    EXPECT_FALSE(target.loadTrustData());
    EXPECT_EQ(target.getTotalEvents(), 1);
    EXPECT_EQ(target.getEventHistory("existing", 1).size(), 1U);
    std::error_code error;
    fs::remove_all(path.parent_path(), error);
}

TEST(TrustScoreboardComprehensive, SnapshotCopiesDoNotDangleAcrossMutation) {
    TrustScoreboard board(nullptr);
    ASSERT_TRUE(board.recordCommunication("agent", true));
    auto oldHistory = board.getEventHistory("agent", 10);
    auto oldLeaderboard = board.getLeaderboard(10);
    ASSERT_TRUE(board.recordRuleViolation("agent", "new evidence"));
    EXPECT_EQ(oldHistory.size(), 1U);
    EXPECT_EQ(oldLeaderboard.size(), 1U);
    EXPECT_EQ(board.getEventHistory("agent", 10).size(), 2U);
}

TEST(TrustScoreboardComprehensive, ConcurrentRecordQueryConfigAndSaveStress) {
    const fs::path path = tempPath("trust.json");
    TrustScoreboard board(nullptr, TrustConfig{}, path);
    std::atomic<bool> start{false};
    std::atomic<int> failures{0};
    std::vector<std::thread> threads;

    for (int writer = 0; writer < 4; ++writer) {
        threads.emplace_back([&, writer] {
            while (!start.load(std::memory_order_acquire)) {}
            for (int index = 0; index < 250; ++index) {
                if (!board.recordEvent(
                        "agent-" + std::to_string(writer),
                        (index % 2 == 0) ? TrustEventType::TASK_COMPLETED
                                         : TrustEventType::TASK_FAILED,
                        (index % 2 == 0) ? TrustOutcome::POSITIVE
                                         : TrustOutcome::NEGATIVE,
                        (index % 2 == 0) ? 0.2 : -0.2)) {
                    failures.fetch_add(1);
                }
            }
        });
    }
    threads.emplace_back([&] {
        while (!start.load(std::memory_order_acquire)) {}
        for (int index = 0; index < 300; ++index) {
            const auto leaders = board.getLeaderboard(10);
            for (const auto& score : leaders) {
                if (!std::isfinite(score.overallScore) ||
                    score.overallScore < 0.0 || score.overallScore > 1.0) {
                    failures.fetch_add(1);
                }
            }
            (void)board.detectAnomalies();
            (void)board.getAverageTrustScore();
        }
    });
    threads.emplace_back([&] {
        while (!start.load(std::memory_order_acquire)) {}
        for (int index = 0; index < 100; ++index) {
            TrustConfig config = board.getConfig();
            config.reliabilityWeight = (index % 2 == 0) ? 3.0 : 1.0;
            if (!board.updateConfig(config)) failures.fetch_add(1);
        }
    });
    threads.emplace_back([&] {
        while (!start.load(std::memory_order_acquire)) {}
        for (int index = 0; index < 80; ++index) {
            if (!board.saveTrustData()) failures.fetch_add(1);
        }
    });

    start.store(true, std::memory_order_release);
    for (auto& thread : threads) thread.join();
    EXPECT_EQ(failures.load(), 0);
    EXPECT_EQ(board.getTotalEvents(), 1000);
    EXPECT_EQ(board.getTotalAgents(), 4);
    EXPECT_TRUE(board.saveTrustData());

    TrustScoreboard loaded(nullptr, TrustConfig{}, path);
    EXPECT_TRUE(loaded.loadTrustData());
    EXPECT_EQ(loaded.getTotalEvents(), 1000);
    std::error_code error;
    fs::remove_all(path.parent_path(), error);
}

TEST(TrustScoreboardComprehensive, ConcurrentIndependentLifecyclesAreSafe) {
    std::atomic<int> completed{0};
    std::vector<std::thread> threads;
    for (int worker = 0; worker < 12; ++worker) {
        threads.emplace_back([&] {
            for (int iteration = 0; iteration < 100; ++iteration) {
                TrustScoreboard board(nullptr);
                EXPECT_TRUE(board.recordCollaboration("agent", true));
                EXPECT_EQ(board.getTopAgents(1).size(), 1U);
            }
            completed.fetch_add(1);
        });
    }
    for (auto& thread : threads) thread.join();
    EXPECT_EQ(completed.load(), 12);
}
