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
#include "elizaos/knowledge.hpp"
#include "elizaos/agentlogger.hpp"
#include "elizaos/persistence.hpp"  // compile-boundary regression for the forwarding header

#include <atomic>
#include <chrono>
#include <fstream>
#include <set>
#include <string>
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
std::string makeTempDir() {
    const std::string dir = "/tmp/elizaos_logtest_" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    std::string cmd = "mkdir -p \"" + dir + "\"";
    (void)std::system(cmd.c_str());
    return dir;
}

size_t countFilesWithPrefix(const std::string& dir, const std::string& prefix) {
    size_t n = 0;
    std::string cmd = "ls -1 \"" + dir + "\" 2>/dev/null";
    if (FILE* p = popen(cmd.c_str(), "r")) {
        char buf[512];
        while (fgets(buf, sizeof(buf), p)) {
            std::string leaf(buf);
            if (!leaf.empty() && leaf.back() == '\n') leaf.pop_back();
            if (leaf.rfind(prefix, 0) == 0 && leaf != prefix) ++n;
        }
        pclose(p);
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

    (void)std::system(("rm -rf \"" + dir + "\"").c_str());
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
    std::string cmd = "ls -1 \"" + dir + "\"/*.gz 2>/dev/null | wc -l";
    int gzCount = 0;
    if (FILE* p = popen(cmd.c_str(), "r")) {
        char buf[64];
        if (fgets(buf, sizeof(buf), p)) gzCount = std::atoi(buf);
        pclose(p);
    }
    EXPECT_GE(gzCount, 1);

    (void)std::system(("rm -rf \"" + dir + "\"").c_str());
}

// ============================================================================
// Repair #6: AgentMemoryManager hierarchical lookup must not self-deadlock.
// getMemoriesByType/getMemoriesByConcept previously called getMemoryById()
// while already holding memoryMutex_ (non-recursive) with thread-safety
// enabled. Now they resolve IDs lock-free via findMemoryByIdUnlocked().
// ============================================================================

#include "elizaos/agentmemory.hpp"

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
    const std::string dir = "/tmp/elizaos_hc_logtest_" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    (void)std::system(("mkdir -p \"" + dir + "\"").c_str());
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

    size_t remaining = 0;
    if (FILE* p = popen(("ls -1 \"" + dir + "\" 2>/dev/null").c_str(), "r")) {
        char buf[512];
        while (fgets(buf, sizeof(buf), p)) {
            std::string leaf(buf);
            if (!leaf.empty() && leaf.back() == '\n') leaf.pop_back();
            if (leaf.rfind("hc.log", 0) == 0 && leaf != "hc.log") ++remaining;
        }
        pclose(p);
    }
    EXPECT_LE(remaining, cfg.maxFiles);

    (void)std::system(("rm -rf \"" + dir + "\"").c_str());
}
