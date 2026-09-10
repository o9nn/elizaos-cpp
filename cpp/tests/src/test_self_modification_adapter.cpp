// Comprehensive E2E tests for CharacterModificationDatabaseAdapter.

#include <gtest/gtest.h>

#include "elizaos/core.hpp"
#include "../packages/integration/autonomous_starter/src/plugin-self-modification/src/database/adapter.hpp"

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <thread>
#include <vector>

using namespace elizaos;

namespace {

using Clock = std::chrono::system_clock;

Clock::time_point atNanoseconds(std::int64_t value) {
    return Clock::time_point(std::chrono::nanoseconds(value));
}

std::int64_t nanoseconds(Clock::time_point value) {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(value.time_since_epoch()).count();
}

CharacterModification makeModification(const std::string& agentId, int version,
                                       const std::string& id = "") {
    CharacterModification value;
    value.id = id.empty() ? "mod-" + agentId + "-" + std::to_string(version) : id;
    value.agentId = agentId;
    value.versionNumber = version;
    value.diffXml = "<diff version=\"" + std::to_string(version) + "\">change</diff>";
    value.reasoning = "Test modification v" + std::to_string(version);
    value.appliedAt = atNanoseconds(1000000000LL + version * 101);
    value.createdAt = atNanoseconds(2000000000LL + version * 103);
    return value;
}

CharacterSnapshot makeSnapshot(const std::string& agentId, int version,
                               const std::string& id = "") {
    CharacterSnapshot value;
    value.id = id.empty() ? "snapshot-" + agentId + "-" + std::to_string(version) : id;
    value.agentId = agentId;
    value.versionNumber = version;
    value.characterData = "{\"name\":\"TestAgent\",\"version\":" +
                          std::to_string(version) + "}";
    value.createdAt = atNanoseconds(3000000000LL + version * 107);
    return value;
}

EvolutionRecommendation makeRecommendation(const std::string& agentId,
                                           const std::string& id = "rec-1") {
    EvolutionRecommendation value;
    value.id = id;
    value.agentId = agentId;
    value.roomId = "room-1";
    value.conversationId = "conversation-1";
    value.recommendation = "Increase empathy";
    value.analysisResult = "Low empathy score";
    value.createdAt = atNanoseconds(4000000123LL);
    value.metadata = {{"evaluator", "reflection-v2"}, {"priority", "high"}};
    return value;
}

std::filesystem::path uniqueTempDirectory() {
    static std::atomic<unsigned long long> sequence{0};
    const auto id = sequence.fetch_add(1, std::memory_order_relaxed);
    const auto path = std::filesystem::temp_directory_path() /
        ("elizaos-self-mod-adapter-" + std::to_string(nanoseconds(Clock::now())) + "-" +
         std::to_string(id));
    std::filesystem::create_directories(path);
    return path;
}

std::string readFile(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
}

void writeFile(const std::filesystem::path& path, const std::string& bytes) {
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    ASSERT_TRUE(output.good());
    output.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    ASSERT_TRUE(output.good());
}

class TempDirectory {
public:
    TempDirectory() : path_(uniqueTempDirectory()) {}
    ~TempDirectory() {
        std::error_code ignored;
        std::filesystem::remove_all(path_, ignored);
    }
    const std::filesystem::path& path() const { return path_; }

private:
    std::filesystem::path path_;
};

} // namespace

class SelfModAdapterTest : public ::testing::Test {
protected:
    CharacterModificationDatabaseAdapter adapter_;
    const std::string agentId_ = "agent-primary";
};

TEST_F(SelfModAdapterTest, DefaultModeRemainsInMemoryAndOrdersRecords) {
    EXPECT_FALSE(adapter_.isFileBacked());
    EXPECT_FALSE(adapter_.persist());
    EXPECT_FALSE(adapter_.lastPersistenceError().empty());

    EXPECT_TRUE(adapter_.trySaveModification(makeModification(agentId_, 3)));
    adapter_.saveModification(makeModification(agentId_, 1));
    adapter_.saveModification(makeModification(agentId_, 2));
    EXPECT_TRUE(adapter_.lastPersistenceError().empty());

    const auto loaded = adapter_.loadModifications(agentId_);
    ASSERT_EQ(loaded.size(), 3U);
    EXPECT_EQ(loaded[0].versionNumber, 1);
    EXPECT_EQ(loaded[1].versionNumber, 2);
    EXPECT_EQ(loaded[2].versionNumber, 3);
    EXPECT_EQ(adapter_.getLatestVersion(agentId_), 3);
}

TEST_F(SelfModAdapterTest, SnapshotLockAndAgentIsolationRemainCompatible) {
    const std::string otherAgent = "agent-other";
    adapter_.saveSnapshot(makeSnapshot(agentId_, 5));
    adapter_.saveSnapshot(makeSnapshot(agentId_, 2));
    adapter_.saveModification(makeModification(otherAgent, 1));
    adapter_.setLockStatus(agentId_, true, "admin", "maintenance");

    const auto snapshots = adapter_.loadSnapshots(agentId_);
    ASSERT_EQ(snapshots.size(), 2U);
    EXPECT_EQ(snapshots[0].versionNumber, 2);
    EXPECT_EQ(snapshots[1].versionNumber, 5);
    EXPECT_TRUE(adapter_.loadModifications(agentId_).empty());
    EXPECT_EQ(adapter_.loadModifications(otherAgent).size(), 1U);

    const auto lock = adapter_.getLockStatus(agentId_);
    EXPECT_TRUE(lock.locked);
    EXPECT_EQ(lock.lockedBy, "admin");
    EXPECT_EQ(lock.lockReason, "maintenance");
    EXPECT_FALSE(adapter_.getLockStatus("unknown-agent").locked);
}

TEST_F(SelfModAdapterTest, RollbackRetainsTimestampAndLatestVersion) {
    adapter_.saveModification(makeModification(agentId_, 1));
    adapter_.saveModification(makeModification(agentId_, 2));
    adapter_.saveModification(makeModification(agentId_, 3));
    const auto rolledBackAt = atNanoseconds(5000000999LL);

    ASSERT_TRUE(adapter_.tryRollbackModifications(agentId_, 1, rolledBackAt));
    const auto loaded = adapter_.loadModifications(agentId_);
    ASSERT_EQ(loaded.size(), 3U);
    EXPECT_FALSE(loaded[0].rolledBackAt.has_value());
    ASSERT_TRUE(loaded[1].rolledBackAt.has_value());
    ASSERT_TRUE(loaded[2].rolledBackAt.has_value());
    EXPECT_EQ(*loaded[1].rolledBackAt, rolledBackAt);
    EXPECT_EQ(*loaded[2].rolledBackAt, rolledBackAt);
    EXPECT_EQ(adapter_.getLatestVersion(agentId_), 1);
}

TEST_F(SelfModAdapterTest, RateLimitCountsOnlySuccessfulRecentAttemptsAndPrunesExpired) {
    const auto now = Clock::now();
    ASSERT_TRUE(adapter_.trySaveRateLimitAttempt(agentId_, true, now - std::chrono::minutes(30)));
    ASSERT_TRUE(adapter_.trySaveRateLimitAttempt(agentId_, true, now - std::chrono::hours(2)));
    ASSERT_TRUE(adapter_.trySaveRateLimitAttempt(agentId_, false, now - std::chrono::minutes(10)));
    ASSERT_TRUE(adapter_.trySaveRateLimitAttempt(agentId_, true, now - std::chrono::hours(25)));

    const auto status = adapter_.checkRateLimit(agentId_);
    EXPECT_EQ(status.hourlyCount, 1);
    EXPECT_EQ(status.dailyCount, 2);
    const auto attempts = adapter_.loadRateLimitAttempts(agentId_);
    ASSERT_EQ(attempts.size(), 3U);
    EXPECT_FALSE(attempts.back().successful);
    EXPECT_TRUE(adapter_.tryPruneExpiredRateLimitHistory(now));
    EXPECT_EQ(adapter_.loadRateLimitAttempts(agentId_).size(), 3U);
}

TEST_F(SelfModAdapterTest, RecommendationContextMetadataAndProcessedStateAreRetained) {
    auto recommendation = makeRecommendation(agentId_);
    ASSERT_TRUE(adapter_.trySaveEvolutionRecommendation(recommendation));

    auto unprocessed = adapter_.getUnprocessedRecommendations(agentId_);
    ASSERT_EQ(unprocessed.size(), 1U);
    EXPECT_EQ(unprocessed[0].agentId, agentId_);
    EXPECT_EQ(unprocessed[0].roomId, "room-1");
    EXPECT_EQ(unprocessed[0].conversationId, "conversation-1");
    EXPECT_EQ(unprocessed[0].metadata.at("evaluator"), "reflection-v2");

    const auto processedAt = atNanoseconds(6000000777LL);
    ASSERT_TRUE(adapter_.tryMarkRecommendationsProcessed(agentId_, {recommendation.id}, processedAt));
    EXPECT_TRUE(adapter_.getUnprocessedRecommendations(agentId_).empty());
    const auto all = adapter_.loadRecommendations(agentId_);
    ASSERT_EQ(all.size(), 1U);
    EXPECT_TRUE(all[0].processed);
    ASSERT_TRUE(all[0].processedAt.has_value());
    EXPECT_EQ(*all[0].processedAt, processedAt);
    EXPECT_EQ(all[0].metadata, recommendation.metadata);
}

TEST_F(SelfModAdapterTest, CompatibilityRecommendationOverloadRetainsContext) {
    adapter_.saveEvolutionRecommendation(agentId_, "room-compatible", "conversation-compatible",
                                         "Add humor", "Too formal");
    const auto values = adapter_.getUnprocessedRecommendations(agentId_);
    ASSERT_EQ(values.size(), 1U);
    EXPECT_EQ(values[0].agentId, agentId_);
    EXPECT_EQ(values[0].roomId, "room-compatible");
    EXPECT_EQ(values[0].conversationId, "conversation-compatible");
}

TEST_F(SelfModAdapterTest, InvalidInputsAreRejectedWithoutPartialMutation) {
    auto valid = makeModification(agentId_, 1, "shared-mod-id");
    ASSERT_TRUE(adapter_.trySaveModification(valid));

    auto emptyId = makeModification(agentId_, 2);
    emptyId.id.clear();
    EXPECT_FALSE(adapter_.trySaveModification(emptyId));

    auto negativeVersion = makeModification(agentId_, -1);
    EXPECT_FALSE(adapter_.trySaveModification(negativeVersion));

    auto duplicateVersion = makeModification(agentId_, 1, "another-id");
    EXPECT_FALSE(adapter_.trySaveModification(duplicateVersion));

    auto duplicateId = makeModification("agent-other", 7, valid.id);
    EXPECT_FALSE(adapter_.trySaveModification(duplicateId));

    auto invalidSnapshot = makeSnapshot(agentId_, -1);
    EXPECT_FALSE(adapter_.trySaveSnapshot(invalidSnapshot));
    ASSERT_EQ(adapter_.loadModifications(agentId_).size(), 1U);
    EXPECT_TRUE(adapter_.loadSnapshots(agentId_).empty());

    EXPECT_FALSE(adapter_.tryRollbackModifications(agentId_, -1));
    EXPECT_FALSE(adapter_.trySaveRateLimitAttempt("", true));
    EXPECT_FALSE(adapter_.trySetLockStatus("", true));
    EXPECT_FALSE(adapter_.tryMarkRecommendationsProcessed(agentId_, {}));
    EXPECT_FALSE(adapter_.lastPersistenceError().empty());
}

TEST_F(SelfModAdapterTest, EmptyRecommendationAndInvalidQueryLimitsAreRejected) {
    auto recommendation = makeRecommendation(agentId_);
    recommendation.recommendation.clear();
    EXPECT_FALSE(adapter_.trySaveEvolutionRecommendation(recommendation));

    recommendation = makeRecommendation(agentId_, "rec-bad-room");
    recommendation.roomId.clear();
    EXPECT_FALSE(adapter_.trySaveEvolutionRecommendation(recommendation));

    recommendation = makeRecommendation(agentId_, "rec-good");
    ASSERT_TRUE(adapter_.trySaveEvolutionRecommendation(recommendation));
    EXPECT_TRUE(adapter_.getUnprocessedRecommendations(agentId_, 0).empty());
    EXPECT_FALSE(adapter_.lastPersistenceError().empty());
    EXPECT_TRUE(adapter_.getUnprocessedRecommendations(
        agentId_, CharacterModificationDatabaseAdapter::kMaxRecommendationQueryLimit + 1).empty());
    EXPECT_EQ(adapter_.loadRecommendations(agentId_).size(), 1U);
}

TEST(SelfModAdapterPersistenceTest, RestartRoundTripPreservesEveryFieldAndTimestamp) {
    TempDirectory temp;
    const auto file = temp.path() / "database.json";
    const std::string agent = "agent-roundtrip";
    auto modification = makeModification(agent, 2, "mod-all-fields");
    auto snapshot = makeSnapshot(agent, 2, "snapshot-all-fields");
    auto recommendation = makeRecommendation(agent, "recommendation-all-fields");
    const auto rolledBackAt = atNanoseconds(7000000111LL);
    const auto successAt = Clock::now() - std::chrono::minutes(20);
    const auto failureAt = Clock::now() - std::chrono::minutes(10);
    const auto processedAt = atNanoseconds(8000000222LL);

    {
        CharacterModificationDatabaseAdapter adapter(file);
        ASSERT_TRUE(adapter.trySaveModification(modification)) << adapter.lastPersistenceError();
        ASSERT_TRUE(adapter.trySaveSnapshot(snapshot)) << adapter.lastPersistenceError();
        ASSERT_TRUE(adapter.tryRollbackModifications(agent, 1, rolledBackAt)) << adapter.lastPersistenceError();
        ASSERT_TRUE(adapter.trySaveRateLimitAttempt(agent, true, successAt)) << adapter.lastPersistenceError();
        ASSERT_TRUE(adapter.trySaveRateLimitAttempt(agent, false, failureAt)) << adapter.lastPersistenceError();
        ASSERT_TRUE(adapter.trySetLockStatus(agent, true, "operator", "audit")) << adapter.lastPersistenceError();
        ASSERT_TRUE(adapter.trySaveEvolutionRecommendation(recommendation)) << adapter.lastPersistenceError();
        ASSERT_TRUE(adapter.tryMarkRecommendationsProcessed(agent, {recommendation.id}, processedAt))
            << adapter.lastPersistenceError();
    }

    CharacterModificationDatabaseAdapter reloaded(file);
    ASSERT_TRUE(reloaded.lastPersistenceError().empty()) << reloaded.lastPersistenceError();

    const auto modifications = reloaded.loadModifications(agent);
    ASSERT_EQ(modifications.size(), 1U);
    EXPECT_EQ(modifications[0].id, modification.id);
    EXPECT_EQ(modifications[0].agentId, modification.agentId);
    EXPECT_EQ(modifications[0].versionNumber, modification.versionNumber);
    EXPECT_EQ(modifications[0].diffXml, modification.diffXml);
    EXPECT_EQ(modifications[0].reasoning, modification.reasoning);
    EXPECT_EQ(modifications[0].appliedAt, modification.appliedAt);
    EXPECT_EQ(modifications[0].createdAt, modification.createdAt);
    ASSERT_TRUE(modifications[0].rolledBackAt.has_value());
    EXPECT_EQ(*modifications[0].rolledBackAt, rolledBackAt);

    const auto snapshots = reloaded.loadSnapshots(agent);
    ASSERT_EQ(snapshots.size(), 1U);
    EXPECT_EQ(snapshots[0].id, snapshot.id);
    EXPECT_EQ(snapshots[0].agentId, snapshot.agentId);
    EXPECT_EQ(snapshots[0].versionNumber, snapshot.versionNumber);
    EXPECT_EQ(snapshots[0].characterData, snapshot.characterData);
    EXPECT_EQ(snapshots[0].createdAt, snapshot.createdAt);

    const auto attempts = reloaded.loadRateLimitAttempts(agent);
    ASSERT_EQ(attempts.size(), 2U);
    EXPECT_EQ(attempts[0].attemptedAt, successAt);
    EXPECT_TRUE(attempts[0].successful);
    EXPECT_EQ(attempts[1].attemptedAt, failureAt);
    EXPECT_FALSE(attempts[1].successful);

    const auto lock = reloaded.getLockStatus(agent);
    EXPECT_TRUE(lock.locked);
    EXPECT_EQ(lock.lockedBy, "operator");
    EXPECT_EQ(lock.lockReason, "audit");

    EXPECT_TRUE(reloaded.getUnprocessedRecommendations(agent).empty());
    const auto recommendations = reloaded.loadRecommendations(agent);
    ASSERT_EQ(recommendations.size(), 1U);
    EXPECT_EQ(recommendations[0].id, recommendation.id);
    EXPECT_EQ(recommendations[0].agentId, recommendation.agentId);
    EXPECT_EQ(recommendations[0].roomId, recommendation.roomId);
    EXPECT_EQ(recommendations[0].conversationId, recommendation.conversationId);
    EXPECT_EQ(recommendations[0].recommendation, recommendation.recommendation);
    EXPECT_EQ(recommendations[0].analysisResult, recommendation.analysisResult);
    EXPECT_EQ(recommendations[0].createdAt, recommendation.createdAt);
    EXPECT_EQ(recommendations[0].metadata, recommendation.metadata);
    EXPECT_TRUE(recommendations[0].processed);
    ASSERT_TRUE(recommendations[0].processedAt.has_value());
    EXPECT_EQ(*recommendations[0].processedAt, processedAt);
}

TEST(SelfModAdapterPersistenceTest, MalformedReloadRollsBackTransactionAndRepeatedLoadIsIdempotent) {
    TempDirectory temp;
    const auto file = temp.path() / "database.json";
    CharacterModificationDatabaseAdapter adapter(file);
    ASSERT_TRUE(adapter.trySaveModification(makeModification("agent-a", 1)));
    const std::string validBytes = readFile(file);

    ASSERT_TRUE(adapter.reload());
    ASSERT_TRUE(adapter.reload());
    EXPECT_EQ(adapter.loadModifications("agent-a").size(), 1U);

    writeFile(file, "{\"schemaVersion\":1,\"modifications\":[");
    EXPECT_FALSE(adapter.reload());
    EXPECT_FALSE(adapter.lastPersistenceError().empty());
    ASSERT_EQ(adapter.loadModifications("agent-a").size(), 1U);

    writeFile(file, validBytes);
    ASSERT_TRUE(adapter.reload()) << adapter.lastPersistenceError();
    ASSERT_TRUE(adapter.reload()) << adapter.lastPersistenceError();
    EXPECT_EQ(adapter.loadModifications("agent-a").size(), 1U);
}

TEST(SelfModAdapterPersistenceTest, ConstructorRejectsMalformedAndOversizedFiles) {
    TempDirectory temp;
    const auto malformed = temp.path() / "malformed.json";
    writeFile(malformed, "{}");
    CharacterModificationDatabaseAdapter malformedAdapter(malformed);
    EXPECT_FALSE(malformedAdapter.lastPersistenceError().empty());
    EXPECT_TRUE(malformedAdapter.loadModifications("agent").empty());

    const auto oversized = temp.path() / "oversized.json";
    {
        std::ofstream output(oversized, std::ios::binary | std::ios::trunc);
        ASSERT_TRUE(output.good());
        output.seekp(static_cast<std::streamoff>(CharacterModificationDatabaseAdapter::kMaxFileBytes));
        output.put('x');
    }
    CharacterModificationDatabaseAdapter oversizedAdapter(oversized);
    EXPECT_NE(oversizedAdapter.lastPersistenceError().find("byte limit"), std::string::npos);
}

TEST(SelfModAdapterPersistenceTest, AtomicOverwriteIsDeterministicAndLeavesNoTemporaryFile) {
    TempDirectory temp;
    const auto file = temp.path() / "database.json";
    CharacterModificationDatabaseAdapter adapter(file);
    ASSERT_TRUE(adapter.trySaveModification(makeModification("agent-b", 2)))
        << adapter.lastPersistenceError();
    ASSERT_TRUE(adapter.trySaveModification(makeModification("agent-a", 1)))
        << adapter.lastPersistenceError();
    ASSERT_TRUE(adapter.trySetLockStatus("agent-a", true, "owner", "reason"));

    const std::string first = readFile(file);
    ASSERT_FALSE(first.empty());
    EXPECT_FALSE(std::filesystem::exists(file.string() + ".tmp"));
    ASSERT_TRUE(adapter.persist()) << adapter.lastPersistenceError();
    const std::string second = readFile(file);
    EXPECT_EQ(first, second);
    EXPECT_FALSE(std::filesystem::exists(file.string() + ".tmp"));

    CharacterModificationDatabaseAdapter reloaded(file);
    ASSERT_TRUE(reloaded.lastPersistenceError().empty()) << reloaded.lastPersistenceError();
    EXPECT_EQ(reloaded.loadModifications("agent-a").size(), 1U);
    EXPECT_EQ(reloaded.loadModifications("agent-b").size(), 1U);
}

TEST(SelfModAdapterPersistenceTest, PersistenceFailureIsReportedAndMutationRollsBack) {
    TempDirectory temp;
    const auto directoryAsFile = temp.path() / "state-directory";
    std::filesystem::create_directory(directoryAsFile);
    CharacterModificationDatabaseAdapter adapter(directoryAsFile);
    ASSERT_FALSE(adapter.lastPersistenceError().empty());

    auto modification = makeModification("agent-failure", 1);
    adapter.saveModification(modification);
    EXPECT_FALSE(adapter.lastPersistenceError().empty());
    EXPECT_TRUE(adapter.loadModifications("agent-failure").empty());
    EXPECT_FALSE(std::filesystem::exists(directoryAsFile.string() + ".tmp"));
}

TEST(SelfModAdapterPersistenceTest, ConcurrentMutationsPersistACompleteRestartableState) {
    TempDirectory temp;
    const auto file = temp.path() / "database.json";
    CharacterModificationDatabaseAdapter adapter(file);
    constexpr int threadCount = 6;
    constexpr int recordsPerThread = 30;
    std::atomic<bool> succeeded{true};
    std::vector<std::thread> threads;

    for (int threadIndex = 0; threadIndex < threadCount; ++threadIndex) {
        threads.emplace_back([&, threadIndex]() {
            const std::string agent = "concurrent-agent-" + std::to_string(threadIndex);
            for (int record = 0; record < recordsPerThread; ++record) {
                if (!adapter.trySaveModification(makeModification(agent, record))) {
                    succeeded.store(false, std::memory_order_relaxed);
                    return;
                }
                if (!adapter.trySaveRateLimitAttempt(agent, record % 2 == 0)) {
                    succeeded.store(false, std::memory_order_relaxed);
                    return;
                }
            }
        });
    }
    for (auto& thread : threads) thread.join();
    ASSERT_TRUE(succeeded.load(std::memory_order_relaxed)) << adapter.lastPersistenceError();
    EXPECT_FALSE(std::filesystem::exists(file.string() + ".tmp"));

    CharacterModificationDatabaseAdapter reloaded(file);
    ASSERT_TRUE(reloaded.lastPersistenceError().empty()) << reloaded.lastPersistenceError();
    for (int threadIndex = 0; threadIndex < threadCount; ++threadIndex) {
        const std::string agent = "concurrent-agent-" + std::to_string(threadIndex);
        EXPECT_EQ(reloaded.loadModifications(agent).size(), static_cast<std::size_t>(recordsPerThread));
        EXPECT_EQ(reloaded.loadRateLimitAttempts(agent).size(), static_cast<std::size_t>(recordsPerThread));
    }
}

TEST(SelfModAdapterPersistenceTest, FileValidationRejectsDuplicateIdsVersionsAndUnknownFields) {
    TempDirectory temp;
    const auto file = temp.path() / "database.json";
    CharacterModificationDatabaseAdapter adapter(file);
    ASSERT_TRUE(adapter.trySaveModification(makeModification("agent", 1, "mod-one")));

    auto json = nlohmann::json::parse(readFile(file));
    auto duplicate = json.at("modifications").at(0);
    duplicate["id"] = "mod-two";
    json["modifications"].push_back(duplicate);
    writeFile(file, json.dump());
    EXPECT_FALSE(adapter.reload());
    EXPECT_EQ(adapter.loadModifications("agent").size(), 1U);

    json["modifications"].erase(1);
    json["unexpected"] = true;
    writeFile(file, json.dump());
    EXPECT_FALSE(adapter.reload());
    EXPECT_EQ(adapter.loadModifications("agent").size(), 1U);
}

TEST(SelfModAdapterPersistenceTest, ConcurrentReadersAndWritersRemainSynchronizedInMemory) {
    CharacterModificationDatabaseAdapter adapter;
    constexpr int threadCount = 4;
    constexpr int recordsPerThread = 40;
    std::vector<std::thread> threads;
    for (int threadIndex = 0; threadIndex < threadCount; ++threadIndex) {
        threads.emplace_back([&, threadIndex]() {
            const std::string agent = "memory-agent-" + std::to_string(threadIndex);
            for (int record = 0; record < recordsPerThread; ++record) {
                EXPECT_TRUE(adapter.trySaveModification(makeModification(agent, record)));
                (void)adapter.loadModifications(agent);
                (void)adapter.getLatestVersion(agent);
            }
        });
    }
    for (auto& thread : threads) thread.join();
    for (int threadIndex = 0; threadIndex < threadCount; ++threadIndex) {
        EXPECT_EQ(adapter.loadModifications("memory-agent-" + std::to_string(threadIndex)).size(),
                  static_cast<std::size_t>(recordsPerThread));
    }
}
