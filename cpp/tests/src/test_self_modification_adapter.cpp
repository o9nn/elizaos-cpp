// test_self_modification_adapter.cpp
//
// Comprehensive E2E tests for the CharacterModificationDatabaseAdapter.
// Validates all CRUD operations, rate limiting, locking, evolution recommendations,
// rollback behavior, and thread safety of the in-memory adapter.

#include <gtest/gtest.h>
#include "elizaos/core.hpp"
#include "../../cpp/packages/integration/autonomous_starter/src/plugin-self-modification/src/database/adapter.hpp"
#include <chrono>
#include <string>
#include <thread>
#include <vector>

using namespace elizaos;

namespace {

std::string testAgentId() {
    static std::string id = generateUUID();
    return id;
}

CharacterModification makeModification(const std::string& agentId, int version) {
    CharacterModification mod;
    mod.id = generateUUID();
    mod.agentId = agentId;
    mod.versionNumber = version;
    mod.diffXml = "<diff version=\"" + std::to_string(version) + "\">change</diff>";
    mod.reasoning = "Test modification v" + std::to_string(version);
    mod.appliedAt = std::chrono::system_clock::now();
    mod.createdAt = std::chrono::system_clock::now();
    return mod;
}

CharacterSnapshot makeSnapshot(const std::string& agentId, int version) {
    CharacterSnapshot snap;
    snap.id = generateUUID();
    snap.agentId = agentId;
    snap.versionNumber = version;
    snap.characterData = "{\"name\":\"TestAgent\",\"version\":" + std::to_string(version) + "}";
    snap.createdAt = std::chrono::system_clock::now();
    return snap;
}

} // namespace

// ===========================================================================
// Modification CRUD
// ===========================================================================

class SelfModAdapterTest : public ::testing::Test {
protected:
    CharacterModificationDatabaseAdapter adapter_;
    std::string agentId_ = generateUUID();
};

TEST_F(SelfModAdapterTest, SaveAndLoadModification) {
    auto mod = makeModification(agentId_, 1);
    adapter_.saveModification(mod);

    auto loaded = adapter_.loadModifications(agentId_);
    ASSERT_EQ(loaded.size(), 1u);
    EXPECT_EQ(loaded[0].id, mod.id);
    EXPECT_EQ(loaded[0].versionNumber, 1);
    EXPECT_EQ(loaded[0].reasoning, "Test modification v1");
}

TEST_F(SelfModAdapterTest, ModificationsOrderedByVersion) {
    adapter_.saveModification(makeModification(agentId_, 3));
    adapter_.saveModification(makeModification(agentId_, 1));
    adapter_.saveModification(makeModification(agentId_, 2));

    auto loaded = adapter_.loadModifications(agentId_);
    ASSERT_EQ(loaded.size(), 3u);
    EXPECT_EQ(loaded[0].versionNumber, 1);
    EXPECT_EQ(loaded[1].versionNumber, 2);
    EXPECT_EQ(loaded[2].versionNumber, 3);
}

TEST_F(SelfModAdapterTest, LoadModificationsForUnknownAgentReturnsEmpty) {
    auto loaded = adapter_.loadModifications("nonexistent-agent-id");
    EXPECT_TRUE(loaded.empty());
}

TEST_F(SelfModAdapterTest, MultipleAgentsIsolated) {
    std::string agent2 = generateUUID();
    adapter_.saveModification(makeModification(agentId_, 1));
    adapter_.saveModification(makeModification(agent2, 1));
    adapter_.saveModification(makeModification(agentId_, 2));

    EXPECT_EQ(adapter_.loadModifications(agentId_).size(), 2u);
    EXPECT_EQ(adapter_.loadModifications(agent2).size(), 1u);
}

// ===========================================================================
// Snapshot CRUD
// ===========================================================================

TEST_F(SelfModAdapterTest, SaveAndLoadSnapshot) {
    auto snap = makeSnapshot(agentId_, 1);
    adapter_.saveSnapshot(snap);

    auto loaded = adapter_.loadSnapshots(agentId_);
    ASSERT_EQ(loaded.size(), 1u);
    EXPECT_EQ(loaded[0].id, snap.id);
    EXPECT_EQ(loaded[0].characterData, snap.characterData);
}

TEST_F(SelfModAdapterTest, SnapshotsOrderedByVersion) {
    adapter_.saveSnapshot(makeSnapshot(agentId_, 5));
    adapter_.saveSnapshot(makeSnapshot(agentId_, 2));
    adapter_.saveSnapshot(makeSnapshot(agentId_, 8));

    auto loaded = adapter_.loadSnapshots(agentId_);
    ASSERT_EQ(loaded.size(), 3u);
    EXPECT_EQ(loaded[0].versionNumber, 2);
    EXPECT_EQ(loaded[1].versionNumber, 5);
    EXPECT_EQ(loaded[2].versionNumber, 8);
}

// ===========================================================================
// Rollback
// ===========================================================================

TEST_F(SelfModAdapterTest, RollbackMarksModificationsAfterVersion) {
    adapter_.saveModification(makeModification(agentId_, 1));
    adapter_.saveModification(makeModification(agentId_, 2));
    adapter_.saveModification(makeModification(agentId_, 3));

    adapter_.rollbackModifications(agentId_, 1);

    auto loaded = adapter_.loadModifications(agentId_);
    ASSERT_EQ(loaded.size(), 3u);
    EXPECT_FALSE(loaded[0].rolledBackAt.has_value());  // v1 untouched
    EXPECT_TRUE(loaded[1].rolledBackAt.has_value());   // v2 rolled back
    EXPECT_TRUE(loaded[2].rolledBackAt.has_value());   // v3 rolled back
}

TEST_F(SelfModAdapterTest, RollbackDoesNotAffectOtherAgents) {
    std::string agent2 = generateUUID();
    adapter_.saveModification(makeModification(agentId_, 1));
    adapter_.saveModification(makeModification(agentId_, 2));
    adapter_.saveModification(makeModification(agent2, 1));

    adapter_.rollbackModifications(agentId_, 0);

    auto agent2Mods = adapter_.loadModifications(agent2);
    ASSERT_EQ(agent2Mods.size(), 1u);
    EXPECT_FALSE(agent2Mods[0].rolledBackAt.has_value());
}

// ===========================================================================
// Rate Limiting
// ===========================================================================

TEST_F(SelfModAdapterTest, RateLimitInitiallyZero) {
    auto status = adapter_.checkRateLimit(agentId_);
    EXPECT_EQ(status.hourlyCount, 0);
    EXPECT_EQ(status.dailyCount, 0);
}

TEST_F(SelfModAdapterTest, RateLimitCountsSuccessfulAttempts) {
    adapter_.saveRateLimitAttempt(agentId_, true);
    adapter_.saveRateLimitAttempt(agentId_, true);
    adapter_.saveRateLimitAttempt(agentId_, false);  // unsuccessful - not counted

    auto status = adapter_.checkRateLimit(agentId_);
    EXPECT_EQ(status.hourlyCount, 2);
    EXPECT_EQ(status.dailyCount, 2);
}

// ===========================================================================
// Locking
// ===========================================================================

TEST_F(SelfModAdapterTest, LockStatusDefaultsToUnlocked) {
    auto status = adapter_.getLockStatus(agentId_);
    EXPECT_FALSE(status.locked);
    EXPECT_TRUE(status.lockedBy.empty());
}

TEST_F(SelfModAdapterTest, SetAndGetLockStatus) {
    adapter_.setLockStatus(agentId_, true, "admin", "Maintenance mode");

    auto status = adapter_.getLockStatus(agentId_);
    EXPECT_TRUE(status.locked);
    EXPECT_EQ(status.lockedBy, "admin");
    EXPECT_EQ(status.lockReason, "Maintenance mode");
}

TEST_F(SelfModAdapterTest, UnlockAgent) {
    adapter_.setLockStatus(agentId_, true, "admin", "Locked");
    adapter_.setLockStatus(agentId_, false);

    auto status = adapter_.getLockStatus(agentId_);
    EXPECT_FALSE(status.locked);
}

// ===========================================================================
// Evolution Recommendations
// ===========================================================================

TEST_F(SelfModAdapterTest, SaveAndGetRecommendations) {
    adapter_.saveEvolutionRecommendation(agentId_, "room1", "conv1",
                                         "Increase empathy", "Analysis: low empathy score");
    adapter_.saveEvolutionRecommendation(agentId_, "room2", "conv2",
                                         "Add humor", "Analysis: too formal");

    auto recs = adapter_.getUnprocessedRecommendations(agentId_);
    ASSERT_EQ(recs.size(), 2u);
    // Ordered by creation time
    EXPECT_EQ(recs[0].recommendation, "Increase empathy");
    EXPECT_EQ(recs[1].recommendation, "Add humor");
}

TEST_F(SelfModAdapterTest, GetRecommendationsRespectsLimit) {
    for (int i = 0; i < 15; ++i) {
        adapter_.saveEvolutionRecommendation(agentId_, "room", "conv",
                                             "Rec " + std::to_string(i), "Analysis");
    }

    auto recs = adapter_.getUnprocessedRecommendations(agentId_, 5);
    EXPECT_EQ(recs.size(), 5u);
}

TEST_F(SelfModAdapterTest, MarkRecommendationsProcessed) {
    adapter_.saveEvolutionRecommendation(agentId_, "room", "conv", "Rec1", "A1");
    adapter_.saveEvolutionRecommendation(agentId_, "room", "conv", "Rec2", "A2");

    auto recs = adapter_.getUnprocessedRecommendations(agentId_);
    ASSERT_EQ(recs.size(), 2u);

    adapter_.markRecommendationsProcessed(agentId_, {recs[0].id});

    auto remaining = adapter_.getUnprocessedRecommendations(agentId_);
    ASSERT_EQ(remaining.size(), 1u);
    EXPECT_EQ(remaining[0].recommendation, "Rec2");
}

// ===========================================================================
// Version Tracking
// ===========================================================================

TEST_F(SelfModAdapterTest, GetLatestVersionReturnsZeroForNewAgent) {
    EXPECT_EQ(adapter_.getLatestVersion(agentId_), 0);
}

TEST_F(SelfModAdapterTest, GetLatestVersionReturnsHighestNonRolledBack) {
    adapter_.saveModification(makeModification(agentId_, 1));
    adapter_.saveModification(makeModification(agentId_, 2));
    adapter_.saveModification(makeModification(agentId_, 3));

    EXPECT_EQ(adapter_.getLatestVersion(agentId_), 3);

    adapter_.rollbackModifications(agentId_, 2);
    EXPECT_EQ(adapter_.getLatestVersion(agentId_), 2);
}

// ===========================================================================
// Thread Safety
// ===========================================================================

TEST_F(SelfModAdapterTest, ConcurrentAccessDoesNotCrash) {
    std::vector<std::thread> threads;
    for (int t = 0; t < 4; ++t) {
        threads.emplace_back([this, t]() {
            for (int i = 0; i < 50; ++i) {
                adapter_.saveModification(makeModification(agentId_, t * 50 + i));
                adapter_.loadModifications(agentId_);
                adapter_.saveRateLimitAttempt(agentId_, i % 2 == 0);
                adapter_.checkRateLimit(agentId_);
            }
        });
    }
    for (auto& t : threads) {
        t.join();
    }

    auto loaded = adapter_.loadModifications(agentId_);
    EXPECT_EQ(loaded.size(), 200u);
}
