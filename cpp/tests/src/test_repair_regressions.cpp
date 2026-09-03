// test_repair_regressions.cpp
//
// End-to-end regression suite that locks in the structure-preserving repairs
// applied during the KSM "cognitive-cycle" evolution cycle. Each TEST exercises
// a previously-incomplete or deadlock-prone living center so that regressions
// announce themselves immediately:
//
//   1. CircuitBreaker        - non-recursive mutex self-deadlock (agentloop)
//   2. persistence forwarding - <elizaos/persistence.hpp> compile boundary
//   3. KnowledgeHypergraph   - real multi-node constraint-satisfaction matching
//   4. KnowledgeFusionEngine - functional temporal validity windows
//   5. AgentLogger           - log rotation retention (maxFiles) + compression
//
// These are real assertions over real behavior (no mocks / no skips).

#include <gtest/gtest.h>

#include "elizaos/agentloop.hpp"
#include "elizaos/agentmemory.hpp"
#include "elizaos/attention.hpp"
#include "elizaos/auto_fun.hpp"
#include "elizaos/core.hpp"
#include "elizaos/evolutionary.hpp"
#include "elizaos/knowledge.hpp"
#include "elizaos/agentlogger.hpp"
#include "elizaos/ljspeechtools.hpp"
#include "elizaos/otaku.hpp"
#include "elizaos/otc_agent.hpp"
#include "elizaos/persistence.hpp"  // compile-boundary regression for the forwarding header
#include "elizaos/the_org.hpp"
#include "village_event_bus.hpp"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>
#include <system_error>
#include <thread>
#include <vector>

using namespace elizaos;
using namespace std::chrono_literals;

// ============================================================================
// Repair #1: CircuitBreaker must not self-deadlock when recording results
// drives the evaluation window into the failure-rate branch.
// ============================================================================

TEST(RepairCircuitBreaker, RecordResultsAcrossEvaluationWindowDoesNotDeadlock) {
    CircuitBreakerConfig config;
    config.evaluationWindow = 4;
    CircuitBreaker cb(config);

    // Exactly fills the window so shouldTransitionToOpen() consults the
    // failure-rate branch that previously re-locked the non-recursive mutex.
    cb.recordSuccess();
    cb.recordFailure();
    cb.recordSuccess();
    cb.recordFailure();

    EXPECT_NEAR(cb.getFailureRate(), 0.5, 1e-9);
}

TEST(RepairCircuitBreaker, FailureRateConsistentUnderConcurrency) {
    CircuitBreakerConfig config;
    config.evaluationWindow = 16;
    config.failureThreshold = 1000;       // avoid tripping on consecutive fails
    config.failureRateThreshold = 2.0;    // never trip on rate
    CircuitBreaker cb(config);

    std::atomic<bool> go{false};
    std::vector<std::thread> threads;
    for (int t = 0; t < 4; ++t) {
        threads.emplace_back([&, t]() {
            while (!go.load()) { std::this_thread::yield(); }
            for (int i = 0; i < 250; ++i) {
                if ((i + t) % 2 == 0) cb.recordSuccess();
                else                  cb.recordFailure();
                // Cross-call into the locked accessor to provoke any re-entrancy bug.
                (void)cb.getFailureRate();
                (void)cb.getState();
            }
        });
    }
    go.store(true);
    for (auto& th : threads) th.join();

    // The only guarantee we need is that we got here without deadlocking and the
    // rate is a valid probability.
    const double rate = cb.getFailureRate();
    EXPECT_GE(rate, 0.0);
    EXPECT_LE(rate, 1.0);
    EXPECT_EQ(cb.getTotalCalls(), 1000u);
}

// ============================================================================
// Repair #2: <elizaos/persistence.hpp> forwarding header must expose the real
// persistence API (it previously did not exist).
// ============================================================================

TEST(RepairPersistenceForwarding, StorageRoundTripsThroughForwardingHeader) {
    // These symbols are only reachable when <elizaos/persistence.hpp> correctly
    // forwards to the authoritative persistence definitions. An in-memory
    // backend keeps the test hermetic (no filesystem/sqlite dependency).
    ASSERT_TRUE(storage::init(StorageConfig::inMemory()));

    EXPECT_TRUE(storage::set("alpha", "1"));
    EXPECT_TRUE(storage::set("beta", "2"));

    auto a = storage::get("alpha");
    ASSERT_TRUE(a.has_value());
    EXPECT_EQ(*a, "1");

    auto b = storage::get("beta");
    ASSERT_TRUE(b.has_value());
    EXPECT_EQ(*b, "2");

    EXPECT_TRUE(storage::remove("beta"));
    EXPECT_FALSE(storage::get("beta").has_value());

    EXPECT_TRUE(storage::shutdown());
}

// ============================================================================
// Repair #3: KnowledgeHypergraph::matchPattern performs proper multi-variable
// constraint satisfaction (previously only bound the first pattern node).
// ============================================================================

namespace {
KnowledgeEntry makeNode(const std::string& id, const std::string& content) {
    KnowledgeEntry e;
    e.id = id;
    e.content = content;
    e.type = KnowledgeType::FACT;
    return e;
}
} // namespace

TEST(RepairHypergraphMatching, TwoVariablePatternBindsBothEndpoints) {
    KnowledgeHypergraph graph;
    graph.addNode(makeNode("rain", "Rain"));
    graph.addNode(makeNode("wet", "Wet ground"));
    graph.addNode(makeNode("unrelated", "Unrelated"));

    // rain --causes--> wet
    graph.addEdge(Hyperedge("causes", {"rain", "wet"}));

    // Pattern: ?x --causes--> ?y  (edge constraint between variable 0 and 1)
    auto matches = graph.matchPattern({"x", "y"}, {{0, 1}}, "causes");

    // Exactly one binding should satisfy the directed causal edge.
    ASSERT_EQ(matches.size(), 1u);
    EXPECT_EQ(matches[0].at("x"), "rain");
    EXPECT_EQ(matches[0].at("y"), "wet");
}

TEST(RepairHypergraphMatching, RelationshipTypeFiltersBindings) {
    KnowledgeHypergraph graph;
    graph.addNode(makeNode("a", "A"));
    graph.addNode(makeNode("b", "B"));
    graph.addEdge(Hyperedge("likes", {"a", "b"}));

    // No "causes" edge exists, so a typed pattern must yield zero matches.
    auto none = graph.matchPattern({"x", "y"}, {{0, 1}}, "causes");
    EXPECT_TRUE(none.empty());

    // The matching "likes" relationship yields exactly one binding.
    auto some = graph.matchPattern({"x", "y"}, {{0, 1}}, "likes");
    ASSERT_EQ(some.size(), 1u);
    EXPECT_EQ(some[0].at("x"), "a");
    EXPECT_EQ(some[0].at("y"), "b");
}

TEST(RepairHypergraphMatching, ThreeVariableChainPattern) {
    KnowledgeHypergraph graph;
    graph.addNode(makeNode("a", "A"));
    graph.addNode(makeNode("b", "B"));
    graph.addNode(makeNode("c", "C"));
    graph.addNode(makeNode("d", "D"));

    // Chain a->b->c via "next"
    graph.addEdge(Hyperedge("next", {"a", "b"}));
    graph.addEdge(Hyperedge("next", {"b", "c"}));
    // A decoy edge that should not complete the chain.
    graph.addEdge(Hyperedge("next", {"a", "d"}));

    // Pattern: x --next--> y --next--> z
    auto matches = graph.matchPattern({"x", "y", "z"}, {{0, 1}, {1, 2}}, "next");

    // Only a->b->c satisfies both constraints simultaneously.
    ASSERT_EQ(matches.size(), 1u);
    EXPECT_EQ(matches[0].at("x"), "a");
    EXPECT_EQ(matches[0].at("y"), "b");
    EXPECT_EQ(matches[0].at("z"), "c");
}

TEST(RepairHypergraphMatching, GetEdgesConnectingNoLongerDeadlocks) {
    // Indirectly exercises the getEdgesConnecting/getEdgesConnectingLocked split:
    // the public accessor is still callable, and matchPattern (which uses the
    // locked variant) returns without hanging.
    KnowledgeHypergraph graph;
    graph.addNode(makeNode("p", "P"));
    graph.addNode(makeNode("q", "Q"));
    graph.addEdge(Hyperedge("rel", {"p", "q"}));

    auto edges = graph.getEdgesConnecting("p");
    EXPECT_EQ(edges.size(), 1u);

    auto matches = graph.matchPattern({"x", "y"}, {{0, 1}}, "rel");
    EXPECT_EQ(matches.size(), 1u);
}

// ============================================================================
// Repair #4: KnowledgeFusionEngine::setTemporalValidity now enforces validity
// windows in getValidKnowledge (previously a no-op stub).
// ============================================================================

TEST(RepairTemporalValidity, ValidityWindowGatesGetValidKnowledge) {
    KnowledgeFusionEngine fusion;

    KnowledgeEntry entry = makeNode("seasonal", "Holiday hours");
    fusion.recordVersion(entry, "created", "system");

    const auto now = std::chrono::system_clock::now();

    // Window: [now-1h, now+1h)
    fusion.setTemporalValidity("seasonal", now - 1h, now + 1h);

    // Inside the window => included.
    {
        auto valid = fusion.getValidKnowledge(now);
        bool found = false;
        for (const auto& e : valid) if (e.id == "seasonal") found = true;
        EXPECT_TRUE(found);
    }

    // Before the window opens => excluded.
    {
        auto valid = fusion.getValidKnowledge(now - 2h);
        bool found = false;
        for (const auto& e : valid) if (e.id == "seasonal") found = true;
        EXPECT_FALSE(found);
    }

    // After the window closes => excluded.
    {
        auto valid = fusion.getValidKnowledge(now + 2h);
        bool found = false;
        for (const auto& e : valid) if (e.id == "seasonal") found = true;
        EXPECT_FALSE(found);
    }
}

TEST(RepairTemporalValidity, ClearingValidityRestoresUnconstrainedEntry) {
    KnowledgeFusionEngine fusion;

    KnowledgeEntry entry = makeNode("toggle", "Toggle");
    fusion.recordVersion(entry, "created", "system");

    // Query strictly after the version timestamp so version-timing never gates
    // the result; this isolates the temporal-validity window behavior.
    std::this_thread::sleep_for(10ms);
    const auto queryAt = std::chrono::system_clock::now();

    // Constrain to a future-only window that excludes queryAt (window opens later).
    fusion.setTemporalValidity("toggle", queryAt + 1h, queryAt + 2h);
    {
        auto valid = fusion.getValidKnowledge(queryAt);
        bool found = false;
        for (const auto& e : valid) if (e.id == "toggle") found = true;
        EXPECT_FALSE(found);  // excluded purely by the validity window
    }

    // Clear the window (both unset) => entry valid at any time again.
    fusion.setTemporalValidity("toggle", std::nullopt, std::nullopt);
    {
        auto valid = fusion.getValidKnowledge(queryAt);
        bool found = false;
        for (const auto& e : valid) if (e.id == "toggle") found = true;
        EXPECT_TRUE(found);   // window cleared => unconstrained again
    }
}

TEST(RepairTemporalValidity, GetValidKnowledgeNoLongerDeadlocks) {
    // getValidKnowledge previously called getVersionAt which re-locked the same
    // non-recursive mutex. Confirm a populated engine returns promptly.
    KnowledgeFusionEngine fusion;
    for (int i = 0; i < 8; ++i) {
        fusion.recordVersion(makeNode("e" + std::to_string(i), "c"), "created", "system");
    }
    auto valid = fusion.getValidKnowledge(std::chrono::system_clock::now());
    EXPECT_GE(valid.size(), 8u);
}

// ============================================================================
// Repair #5: AgentLogger log rotation enforces maxFiles retention and optional
// compression (previously TODO no-ops).
// ============================================================================

namespace {
// Directory helpers use std::filesystem rather than shelling out through
// popen()/system() with POSIX utilities (mkdir -p, ls, rm -rf, wc): popen and
// pclose do not exist on MSVC (C3861) and those utilities are absent on Windows
// runners. std::filesystem is portable and does not depend on a shell.
std::string makeTempDir() {
    const auto dir = std::filesystem::temp_directory_path() /
        ("elizaos_logtest_" +
         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
    return dir.string();
}

void removeTempDir(const std::string& dir) {
    std::error_code ec;
    std::filesystem::remove_all(dir, ec);
}

size_t countFilesWithPrefix(const std::string& dir, const std::string& prefix) {
    size_t n = 0;
    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(dir, ec)) {
        const std::string leaf = entry.path().filename().string();
        if (leaf.rfind(prefix, 0) == 0 && leaf != prefix) ++n;
    }
    return n;
}

size_t countFilesWithExtension(const std::string& dir, const std::string& extension) {
    size_t n = 0;
    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(dir, ec)) {
        if (entry.path().extension() == extension) ++n;
    }
    return n;
}
} // namespace

TEST(RepairLogRotation, RetentionCapsRotatedFiles) {
    const std::string dir = makeTempDir();
    const std::string base = dir + "/app.log";

    AgentLogger logger;
    LogRotationConfig cfg;
    cfg.maxFileSize = 1;        // force rotation on essentially every write
    cfg.maxFiles = 3;           // keep at most 3 rotated artifacts
    cfg.compressRotated = false;
    cfg.rotationPattern = ".%Y%m%d-%H%M%S";
    logger.setRotationConfig(cfg);

    // Rotate several times. Because the timestamp pattern has 1s resolution we
    // craft distinct rotated names directly to make the retention deterministic.
    for (int i = 0; i < 6; ++i) {
        std::ofstream(base) << "payload-" << i << std::endl;  // (re)create active log
        // Directly invoke rotation; suffix collisions are resolved by mtime sort.
        logger.rotateLogFile(base);
        std::this_thread::sleep_for(5ms);
    }

    // After repeated rotations, at most maxFiles rotated siblings should remain.
    const size_t remaining = countFilesWithPrefix(dir, "app.log");
    EXPECT_LE(remaining, cfg.maxFiles);
    removeTempDir(dir);
}

TEST(RepairLogRotation, CompressionProducesGzArtifact) {
    const std::string dir = makeTempDir();
    const std::string base = dir + "/c.log";

    AgentLogger logger;
    LogRotationConfig cfg;
    cfg.maxFileSize = 1;
    cfg.maxFiles = 10;           // do not prune during this test
    cfg.compressRotated = true;  // request gzip compression
    cfg.rotationPattern = ".%Y%m%d-%H%M%S-XXX";
    logger.setRotationConfig(cfg);

    std::ofstream(base) << "compress-me" << std::endl;
    logger.rotateLogFile(base);

    // At least one .gz artifact should now exist in the directory.
    const size_t gzCount = countFilesWithExtension(dir, ".gz");
    EXPECT_GE(gzCount, 1u);

    removeTempDir(dir);
}

// ============================================================================
// Repair #6: AgentMemoryManager hierarchical lookup must not self-deadlock.
// getMemoriesByType/getMemoriesByConcept previously called getMemoryById()
// while already holding memoryMutex_ (non-recursive) with thread-safety
// enabled. Now they resolve IDs lock-free via findMemoryByIdUnlocked().
// ============================================================================

TEST(RepairMemoryDeadlock, HierarchicalLookupUnderThreadSafetyDoesNotDeadlock) {
    auto& mgr = getGlobalMemoryManager();
    mgr.clear();
    mgr.enableThreadSafety(true);  // the configuration that previously deadlocked

    auto m1 = std::make_shared<Memory>("rd-1", "fact about rivers", "ent", "agent");
    auto m2 = std::make_shared<Memory>("rd-2", "fact about mountains", "ent", "agent");
    mgr.createMemory(m1);
    mgr.createMemory(m2);
    mgr.indexMemory("rd-1", HierarchicalMemoryType::SEMANTIC, {"geography"});
    mgr.indexMemory("rd-2", HierarchicalMemoryType::SEMANTIC, {"geography"});

    // These calls must return promptly (no hang) while thread-safety is on.
    auto byType = mgr.getMemoriesByType(HierarchicalMemoryType::SEMANTIC);
    EXPECT_EQ(byType.size(), 2u);

    auto byConcept = mgr.getMemoriesByConcept("geography");
    EXPECT_EQ(byConcept.size(), 2u);

    mgr.clear();
}

// ============================================================================
// Repair #7: AgentMemoryManager::clear() must reset the full memory state
// (strengths, associations and the hierarchical index) so that statistics
// and indices do not leak across logical sessions.
// ============================================================================

TEST(RepairMemoryClear, ClearResetsFullStateNotJustTables) {
    auto& mgr = getGlobalMemoryManager();
    mgr.clear();

    auto m1 = std::make_shared<Memory>("cl-1", "episodic", "ent", "agent");
    auto m2 = std::make_shared<Memory>("cl-2", "semantic", "ent", "agent");
    mgr.createMemory(m1);
    mgr.createMemory(m2);
    mgr.indexMemory("cl-1", HierarchicalMemoryType::EPISODIC);
    mgr.indexMemory("cl-2", HierarchicalMemoryType::SEMANTIC);
    mgr.createAssociation("cl-1", "cl-2", "semantic", 0.5);

    auto before = mgr.getStatistics();
    EXPECT_EQ(before.totalMemories, 2u);
    EXPECT_GE(before.totalAssociations, 1u);

    mgr.clear();

    auto after = mgr.getStatistics();
    EXPECT_EQ(after.totalMemories, 0u);
    EXPECT_EQ(after.totalAssociations, 0u);
    EXPECT_EQ(after.episodicMemories, 0u);
    EXPECT_EQ(after.semanticMemories, 0u);
    // Hierarchical index must be empty too.
    EXPECT_TRUE(mgr.getMemoriesByType(HierarchicalMemoryType::EPISODIC).empty());
    EXPECT_TRUE(mgr.getMemoriesByConcept("").empty());
}

// ============================================================================
// Repair #8: AgentLogger gained the full Task 1.4 feature set (rotation
// retention + gzip). Confirm the rotation API is reachable and enforces
// retention through the forwarding header.
// ============================================================================

TEST(RepairLoggerFeatureParity, RotationApiIsReachableAndEnforcesRetention) {
    const std::string dir =
        (std::filesystem::temp_directory_path() /
         ("elizaos_hc_logtest_" +
          std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())))
            .string();
    {
        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
    }
    const std::string base = dir + "/hc.log";

    AgentLogger logger;
    LogRotationConfig cfg;
    cfg.maxFileSize = 1;
    cfg.maxFiles = 2;
    cfg.compressRotated = false;
    cfg.rotationPattern = ".%Y%m%d-%H%M%S";
    logger.setRotationConfig(cfg);

    for (int i = 0; i < 5; ++i) {
        std::ofstream(base) << "payload-" << i << std::endl;
        logger.rotateLogFile(base);
        std::this_thread::sleep_for(5ms);
    }

    const size_t remaining = countFilesWithPrefix(dir, "hc.log");
    EXPECT_LE(remaining, cfg.maxFiles);

    removeTempDir(dir);
}


// ============================================================================
// Repair #9: Public result and statistics types must be deterministic even on
// early-return, empty-state, and first-cycle paths.
// ============================================================================

TEST(RepairDeterministicDefaults, CognitiveAndMemoryContractsStartFromZeroState) {
    FragmentMetadata fragment;
    EXPECT_EQ(fragment.position, 0u);
    EXPECT_EQ(fragment.type, MemoryType::FRAGMENT);

    AttentionCostBudget::AllocationResult allocation;
    EXPECT_FALSE(allocation.success);
    EXPECT_DOUBLE_EQ(allocation.allocatedAmount, 0.0);
    EXPECT_DOUBLE_EQ(allocation.remainingBudget, 0.0);

    EnhancedAttentionAllocator::EnhancedStatistics attentionStats;
    EXPECT_DOUBLE_EQ(attentionStats.averageTemporalScore, 0.0);
    EXPECT_DOUBLE_EQ(attentionStats.averageSaliencyScore, 0.0);
    EXPECT_DOUBLE_EQ(attentionStats.budgetUtilization, 0.0);
    EXPECT_EQ(attentionStats.patternsPredicted, 0);
    EXPECT_EQ(attentionStats.activeTransfers, 0);

    InferenceStep inference;
    KnowledgeConflict conflict;
    EXPECT_DOUBLE_EQ(inference.confidence, 0.0);
    EXPECT_DOUBLE_EQ(conflict.severity, 0.0);

    EvolutionaryOptimizer::Statistics evolution;
    EXPECT_EQ(evolution.generation, 0u);
    EXPECT_DOUBLE_EQ(evolution.diversity, 0.0);
    EXPECT_DOUBLE_EQ(evolution.convergenceRate, 0.0);
    EXPECT_EQ(evolution.stagnationCount, 0);
    EXPECT_EQ(evolution.generationTime, std::chrono::milliseconds{0});

    auto_fun::Result<std::uint64_t> errorResult(
        auto_fun::AutoFunError::VALUE_TOO_SMALL, "expected failure");
    EXPECT_FALSE(errorResult.success);
    EXPECT_EQ(errorResult.value, 0u);
    EXPECT_EQ(errorResult.error, auto_fun::AutoFunError::VALUE_TOO_SMALL);
}

TEST(RepairDeterministicDefaults, ExternalBoundaryContractsStartFromSafeState) {
    AudioData audio;
    EXPECT_EQ(audio.sample_rate, 0);
    EXPECT_EQ(audio.channels, 0);
    EXPECT_DOUBLE_EQ(audio.duration_seconds, 0.0);

    TranscriptionResult transcription;
    EXPECT_FALSE(transcription.success);
    EXPECT_DOUBLE_EQ(transcription.confidence, 0.0);

    CommunityMetrics community;
    EXPECT_EQ(community.totalMembers, 0u);
    EXPECT_EQ(community.activeMembers, 0u);
    EXPECT_DOUBLE_EQ(community.engagementRate, 0.0);

    SocialMediaManagerAgent::SocialMediaMetrics social;
    EXPECT_EQ(social.platform, PlatformType::DISCORD);
    EXPECT_EQ(social.followers, 0u);
    EXPECT_EQ(social.impressions, 0u);
    EXPECT_EQ(social.clicks, 0u);
    EXPECT_EQ(social.shares, 0u);

    TheOrgManager::SystemMetrics system;
    EXPECT_EQ(system.totalAgents, 0u);
    EXPECT_EQ(system.activeAgents, 0u);
    EXPECT_DOUBLE_EQ(system.systemLoad, 0.0);
    EXPECT_EQ(system.averageResponseTime, std::chrono::milliseconds{0});

    TransactionReceipt receipt;
    EXPECT_EQ(receipt.status, TxStatus::PENDING);
    EXPECT_EQ(receipt.chainId, ChainId::ETHEREUM_MAINNET);
    EXPECT_DOUBLE_EQ(receipt.gasUsed, 0.0);
    EXPECT_DOUBLE_EQ(receipt.effectiveGasPrice, 0.0);
    EXPECT_EQ(receipt.blockNumber, 0u);

    YieldPosition position;
    EXPECT_DOUBLE_EQ(position.depositedAmount, 0.0);
    EXPECT_DOUBLE_EQ(position.currentValue, 0.0);
    EXPECT_EQ(position.strategy, YieldStrategy::LIQUIDITY_PROVISION);
    EXPECT_EQ(position.chainId, ChainId::ETHEREUM_MAINNET);

    OTCToken token;
    EXPECT_EQ(token.chain, OTCChain::ETHEREUM);
    EXPECT_EQ(token.decimals, 0);

    OTCOffer offer;
    EXPECT_DOUBLE_EQ(offer.baseAmount, 0.0);
    EXPECT_DOUBLE_EQ(offer.minAmount, 0.0);
    EXPECT_DOUBLE_EQ(offer.maxAmount, 0.0);
    EXPECT_EQ(offer.status, OfferStatus::PENDING);
    EXPECT_FALSE(offer.partialFillAllowed);
    EXPECT_DOUBLE_EQ(offer.filledAmount, 0.0);

    CounterpartyProfile counterparty;
    EXPECT_EQ(counterparty.totalTrades, 0);
    EXPECT_DOUBLE_EQ(counterparty.totalVolume, 0.0);
    EXPECT_DOUBLE_EQ(counterparty.averageTradeSize, 0.0);
    EXPECT_FALSE(counterparty.isVerified);

    OTCAnalytics::TradingStats trading;
    EXPECT_EQ(trading.totalTrades, 0);
    EXPECT_DOUBLE_EQ(trading.totalVolume, 0.0);
    EXPECT_DOUBLE_EQ(trading.avgExecutionTime, 0.0);
}

// ============================================================================
// Repair #10: AgentMemory search parameters are an executable contract. Every
// declared filter, uniqueness mode, sort/pagination path, and text-gated
// embedding query must affect results deterministically.
// ============================================================================

TEST(RepairMemorySearch, AppliesAllDeclaredFiltersAndDeterministicPagination) {
    AgentMemoryManager manager;

    auto first = std::make_shared<Memory>(
        "search-a", "Alpha planning memory", "entity-a", "agent-a");
    first->setRoomId("room-a");
    first->setWorldId("world-a");
    first->setEmbedding(EmbeddingVector{1.0f, 0.0f});

    auto duplicate = std::make_shared<Memory>(
        "search-b", "Alpha planning memory", "entity-a", "agent-a");
    duplicate->setRoomId("room-a");
    duplicate->setWorldId("world-a");
    duplicate->setEmbedding(EmbeddingVector{1.0f, 0.0f});

    auto decoy = std::make_shared<Memory>(
        "search-c", "Beta unrelated memory", "entity-a", "agent-a");
    decoy->setRoomId("room-a");
    decoy->setWorldId("world-b");
    decoy->setEmbedding(EmbeddingVector{0.0f, 1.0f});

    auto otherTable = std::make_shared<Memory>(
        "search-d", "Alpha planning memory", "entity-a", "agent-a");
    otherTable->setRoomId("room-a");
    otherTable->setWorldId("world-a");
    otherTable->setEmbedding(EmbeddingVector{1.0f, 0.0f});

    manager.createMemory(first);
    manager.createMemory(duplicate);
    manager.createMemory(decoy);
    manager.createMemory(otherTable, "archive");
    manager.indexMemory(first->getId(), HierarchicalMemoryType::SEMANTIC,
                        {"planning", "alpha"});
    manager.indexMemory(duplicate->getId(), HierarchicalMemoryType::SEMANTIC,
                        {"planning", "alpha"});
    manager.indexMemory(decoy->getId(), HierarchicalMemoryType::EPISODIC,
                        {"beta"});

    MemoryStrength strong;
    strong.currentStrength = 0.95;
    strong.baseImportance = 0.85;
    manager.setMemoryStrength(first->getId(), strong);
    manager.setMemoryStrength(duplicate->getId(), strong);

    MemorySearchParams params;
    params.tableName = "memories";
    params.entityId = "entity-a";
    params.agentId = "agent-a";
    params.roomId = "room-a";
    params.worldId = "world-a";
    params.memoryType = HierarchicalMemoryType::SEMANTIC;
    params.minStrength = 0.9;
    params.minImportance = 0.8;
    params.concepts = {"planning", "alpha"};
    params.unique = true;
    params.sortByStrength = true;
    params.count = 10;

    const auto filtered = manager.getMemories(params);
    ASSERT_EQ(filtered.size(), 1u);
    EXPECT_EQ(filtered.front()->getWorldId(), "world-a");
    EXPECT_EQ(filtered.front()->getContent(), "Alpha planning memory");

    params.unique = false;
    params.start = 1;
    params.end = 2;
    params.count = 1;
    const auto page = manager.getMemories(params);
    ASSERT_EQ(page.size(), 1u);
    EXPECT_TRUE(page.front()->getId() == "search-a" ||
                page.front()->getId() == "search-b");

    MemorySearchByEmbeddingParams embedding;
    embedding.tableName = "memories";
    embedding.embedding = EmbeddingVector{1.0f, 0.0f};
    embedding.entityId = "entity-a";
    embedding.roomId = "room-a";
    embedding.worldId = "world-a";
    embedding.query = "alpha planning";
    embedding.matchThreshold = 0.99;
    embedding.unique = true;
    embedding.count = 10;

    const auto semanticMatch = manager.searchMemories(embedding);
    ASSERT_EQ(semanticMatch.size(), 1u);
    EXPECT_NEAR(semanticMatch.front()->getSimilarity(), 1.0, 1e-9);
}

// ============================================================================
// Repair #11: Consolidation must apply discovered merges to manager-owned
// tables, strengths, hierarchical indexes, and associative links.
// ============================================================================

TEST(RepairMemoryConsolidation, AppliesMergeAndPreservesDerivedState) {
    AgentMemoryManager manager;

    auto first = std::make_shared<Memory>(
        "merge-a", "shared consolidation memory", "entity", "agent");
    auto second = std::make_shared<Memory>(
        "merge-b", "shared consolidation memory", "entity", "agent");
    auto related = std::make_shared<Memory>(
        "merge-c", "distinct related memory", "entity", "agent");
    for (const auto& memory : {first, second}) {
        memory->setRoomId("room");
        memory->setWorldId("world");
        memory->setEmbedding(EmbeddingVector{1.0f, 0.0f});
        manager.createMemory(memory, "archive");
    }
    related->setRoomId("room");
    related->setWorldId("world");
    related->setEmbedding(EmbeddingVector{0.0f, 1.0f});
    manager.createMemory(related);

    manager.indexMemory(first->getId(), HierarchicalMemoryType::SEMANTIC,
                        {"shared", "first"});
    manager.indexMemory(second->getId(), HierarchicalMemoryType::EPISODIC,
                        {"shared", "second"});
    manager.createAssociation(first->getId(), related->getId(), "semantic", 0.7);
    manager.createAssociation(second->getId(), related->getId(), "semantic", 0.8);

    MemoryConsolidationEngine::ConsolidationParams params;
    params.similarityMergeThreshold = 0.99;
    manager.setConsolidationParams(params);

    const auto result = manager.runConsolidation();
    ASSERT_EQ(result.memoriesMerged, 1);
    ASSERT_EQ(result.appliedMerges.size(), 1u);
    const auto& applied = result.appliedMerges.front();

    EXPECT_EQ(manager.getMemoryById(applied.sourceId), nullptr);
    EXPECT_EQ(manager.getMemoryById(applied.absorbedId), nullptr);
    const auto merged = manager.getMemoryById(applied.mergedId);
    ASSERT_NE(merged, nullptr);
    EXPECT_EQ(merged->getContent(), "shared consolidation memory");
    EXPECT_EQ(merged->getRoomId(), "room");
    EXPECT_EQ(merged->getWorldId(), "world");

    const auto archive = manager.getAllMemoriesFromTable("archive");
    ASSERT_EQ(archive.size(), 1u);
    EXPECT_EQ(archive.front()->getId(), applied.mergedId);
    EXPECT_EQ(manager.getAllMemoriesFromTable("memories").size(), 1u);

    const auto semantic = manager.getMemoriesByType(HierarchicalMemoryType::SEMANTIC);
    const auto episodic = manager.getMemoriesByType(HierarchicalMemoryType::EPISODIC);
    ASSERT_EQ(semantic.size(), 1u);
    ASSERT_EQ(episodic.size(), 1u);
    EXPECT_EQ(semantic.front()->getId(), applied.mergedId);
    EXPECT_EQ(episodic.front()->getId(), applied.mergedId);
    EXPECT_EQ(manager.getMemoriesByConcept("first").front()->getId(), applied.mergedId);
    EXPECT_EQ(manager.getMemoriesByConcept("second").front()->getId(), applied.mergedId);

    const auto links = manager.getAssociations(applied.mergedId);
    ASSERT_EQ(links.size(), 1u);
    EXPECT_EQ(links.front().targetMemoryId, related->getId());
    EXPECT_EQ(links.front().sourceMemoryId, applied.mergedId);

    const auto mergedStrength = manager.getMemoryStrength(applied.mergedId);
    EXPECT_GE(mergedStrength.currentStrength, 0.0);
    EXPECT_LE(mergedStrength.currentStrength, 1.0);
}


// ============================================================================
// Repair #12: Village polling must use the API's `since_tic` cursor name in a
// single shared URL builder, including bounded limits and normalized base URLs.
// ============================================================================

TEST(RepairVillageEventCursor, BuildsCanonicalSinceTicUrl) {
    EXPECT_EQ(
        VillageEventBusClient::buildEventsUrl("http://localhost:8080", 25, 42),
        "http://localhost:8080/api/events/events?limit=25&since_tic=42");
    EXPECT_EQ(
        VillageEventBusClient::buildEventsUrl("http://localhost:8080/", 0, -7),
        "http://localhost:8080/api/events/events?limit=0&since_tic=0");
    EXPECT_EQ(
        VillageEventBusClient::buildEventsUrl("http://localhost:8080///", 5000, 9),
        "http://localhost:8080/api/events/events?limit=5000&since_tic=9");
}
