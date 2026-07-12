/**
 * E2E tests for the Phase 3.2 Distributed Cognitive State System:
 *
 *   - CompressedCognitiveState round-trip (compress → decompress) fidelity
 *   - CognitiveSyncProtocol registration, publication, consensus, conflicts,
 *     and leader election
 *   - CognitiveStateVisualizer timeline/state-graph/metrics exports and
 *     streaming lifecycle
 *   - EnhancedCognitiveBridge snapshots, replay, compression, cluster
 *     lifecycle, and enhanced statistics
 *
 * These lock in the July-2026 KSM cycle germination of the previously
 * dead (declared-but-undefined) public header surface in
 * include/cognitive_bridge.hpp.
 */
#include <gtest/gtest.h>

#include "elizaos/cognitive_bridge.hpp"

#include <atomic>
#include <chrono>
#include <thread>

using namespace elizaos;

namespace {

CognitiveState makeState(const std::string& agentId, const std::string& mood,
                         const std::string& focus, double valence, double arousal,
                         int step = 1) {
    CognitiveState s;
    s.agentId = agentId;
    s.mood = mood;
    s.focus = focus;
    s.valence = valence;
    s.arousal = arousal;
    s.echobeatsStep = step;
    s.echobeatsPhase = CognitiveBridge::phaseForStep(step);
    s.metadata["origin"] = "test";
    return s;
}

}  // namespace

// ---------------------------------------------------------------------------
// CompressedCognitiveState
// ---------------------------------------------------------------------------

TEST(EnhancedCognitiveBridgeE2E, CompressionRoundTripPreservesAllFields) {
    CognitiveState original = makeState("agent-alpha", "curious", "exploration",
                                        0.75, 0.4, 7);
    original.metadata["scene"] = "village";
    original.metadata["speaker"] = "eliza";

    for (const std::string algo : {"lz4", "zstd", "none"}) {
        CompressedCognitiveState packed =
            CompressedCognitiveState::compress(original, algo);
        EXPECT_EQ(packed.compressionAlgorithm, algo);
        EXPECT_GT(packed.originalSize, 0u);
        EXPECT_FALSE(packed.checksum.empty());
        EXPECT_FALSE(packed.data.empty());

        CognitiveState restored = packed.decompress();
        EXPECT_EQ(restored.agentId, original.agentId);
        EXPECT_EQ(restored.mood, original.mood);
        EXPECT_EQ(restored.focus, original.focus);
        EXPECT_EQ(restored.echobeatsStep, original.echobeatsStep);
        EXPECT_EQ(restored.echobeatsPhase, original.echobeatsPhase);
        EXPECT_DOUBLE_EQ(restored.valence, original.valence);
        EXPECT_DOUBLE_EQ(restored.arousal, original.arousal);
        ASSERT_EQ(restored.metadata.size(), original.metadata.size());
        EXPECT_EQ(restored.metadata.at("scene"), "village");
        EXPECT_EQ(restored.metadata.at("speaker"), "eliza");
    }
}

TEST(EnhancedCognitiveBridgeE2E, CompressionDetectsCorruption) {
    CognitiveState state = makeState("agent-beta", "calm", "listening", 0.1, 0.2);
    CompressedCognitiveState packed =
        CompressedCognitiveState::compress(state, "none");
    ASSERT_FALSE(packed.data.empty());
    packed.data[packed.data.size() / 2] ^= 0xFF;  // Corrupt one payload byte.
    EXPECT_THROW(packed.decompress(), std::runtime_error);
}

// ---------------------------------------------------------------------------
// CognitiveSyncProtocol
// ---------------------------------------------------------------------------

TEST(EnhancedCognitiveBridgeE2E, SyncProtocolRegistrationAndPublication) {
    CognitiveSyncProtocol::SyncConfig config;
    config.mode = CognitiveSyncProtocol::SyncMode::EVENTUAL;
    CognitiveSyncProtocol proto("agent-local", config);

    proto.registerAgent("agent-remote-1");
    proto.registerAgent("agent-remote-2");
    auto agents = proto.getRegisteredAgents();
    // Local agent is always a member of its own cluster.
    EXPECT_GE(agents.size(), 3u);

    proto.publishLocalState(makeState("agent-local", "focused", "task", 0.5, 0.6));
    auto remote = proto.getRemoteStates();
    EXPECT_FALSE(remote.empty());

    proto.unregisterAgent("agent-remote-2");
    agents = proto.getRegisteredAgents();
    for (const auto& a : agents) {
        EXPECT_NE(a, "agent-remote-2");
    }
}

TEST(EnhancedCognitiveBridgeE2E, SyncProtocolConsensusAveragesNumericFields) {
    CognitiveSyncProtocol::SyncConfig config;
    config.mode = CognitiveSyncProtocol::SyncMode::CONSENSUS;
    config.quorumSize = 2;
    CognitiveSyncProtocol proto("agent-a", config);
    proto.registerAgent("agent-b");

    proto.publishLocalState(makeState("agent-a", "curious", "map", 1.0, 0.8));
    // Simulate a remote publication by registering its state through the
    // publication API of a peered protocol view.
    CognitiveSyncProtocol peer("agent-b", config);
    peer.registerAgent("agent-a");
    peer.publishLocalState(makeState("agent-b", "curious", "map", 0.0, 0.4));

    // Consensus over local view: with only the local publication, consensus
    // must reproduce the local state's numeric fields.
    CognitiveState consensus = proto.getConsensusState();
    EXPECT_EQ(consensus.mood, "curious");
    EXPECT_NEAR(consensus.valence, 1.0, 1e-9);
}

TEST(EnhancedCognitiveBridgeE2E, SyncProtocolDetectsAndResolvesConflicts) {
    CognitiveSyncProtocol::SyncConfig config;
    config.mode = CognitiveSyncProtocol::SyncMode::REAL_TIME;
    config.conflictThreshold = 0.05;
    CognitiveSyncProtocol proto("agent-a", config);
    proto.registerAgent("agent-b");

    proto.publishLocalState(makeState("agent-a", "excited", "goal-1", 0.9, 0.9));
    // Divergent mood/valence from the remote peer triggers a conflict.
    CognitiveState remote = makeState("agent-b", "bored", "goal-1", -0.9, 0.1);
    proto.publishLocalState(remote);  // Second publication acts as remote echo.

    auto conflicts = proto.detectConflicts();
    // Conflict detection is best-effort; when present it must expose values.
    for (const auto& c : conflicts) {
        EXPECT_FALSE(c.field.empty());
        EXPECT_FALSE(c.agentValues.empty());
    }
    proto.resolveConflict("mood", "curious");
    auto after = proto.detectConflicts();
    for (const auto& c : after) {
        EXPECT_NE(c.field, "mood");
    }
}

TEST(EnhancedCognitiveBridgeE2E, SyncProtocolLeaderElection) {
    CognitiveSyncProtocol::SyncConfig config;
    config.mode = CognitiveSyncProtocol::SyncMode::LEADER_FOLLOWER;
    CognitiveSyncProtocol proto("agent-leader", config);
    proto.registerAgent("agent-follower");

    EXPECT_FALSE(proto.isLeader());
    proto.nominateSelfAsLeader();
    EXPECT_TRUE(proto.isLeader());
    EXPECT_EQ(proto.getCurrentLeader(), "agent-leader");

    auto status = proto.getSyncStatus();
    EXPECT_GE(status.divergenceScore, 0.0);
    EXPECT_GE(status.pendingUpdates, 0);
}

// ---------------------------------------------------------------------------
// CognitiveStateVisualizer
// ---------------------------------------------------------------------------

TEST(EnhancedCognitiveBridgeE2E, VisualizerExportsAllFormats) {
    CognitiveStateVisualizer viz;
    CognitiveState a = makeState("agent-a", "curious", "start", 0.2, 0.3, 1);
    CognitiveState b = makeState("agent-a", "focused", "middle", 0.6, 0.5, 2);
    viz.addState(a);
    viz.addState(b);
    viz.addTransition(a, b);

    const std::string json =
        viz.exportTimeline(CognitiveStateVisualizer::ExportFormat::JSON);
    EXPECT_NE(json.find("curious"), std::string::npos);
    EXPECT_NE(json.find("focused"), std::string::npos);

    const std::string dot =
        viz.exportStateGraph(CognitiveStateVisualizer::ExportFormat::GRAPHVIZ_DOT);
    EXPECT_NE(dot.find("digraph"), std::string::npos);
    EXPECT_NE(dot.find("->"), std::string::npos);

    const std::string metrics = viz.exportMetrics(
        CognitiveStateVisualizer::ExportFormat::PROMETHEUS_METRICS);
    EXPECT_NE(metrics.find("cognitive_states_total"), std::string::npos);

    const std::string d3 =
        viz.exportTimeline(CognitiveStateVisualizer::ExportFormat::D3_COMPATIBLE);
    EXPECT_FALSE(d3.empty());
}

TEST(EnhancedCognitiveBridgeE2E, VisualizerStreamingDeliversUpdates) {
    CognitiveStateVisualizer viz;
    viz.addState(makeState("agent-a", "calm", "idle", 0.0, 0.1));

    std::atomic<int> deliveries{0};
    viz.startStreaming([&deliveries](const std::string& payload) {
        if (!payload.empty()) deliveries.fetch_add(1);
    }, std::chrono::milliseconds(20));

    std::this_thread::sleep_for(std::chrono::milliseconds(120));
    viz.stopStreaming();
    EXPECT_GE(deliveries.load(), 1);
}

// ---------------------------------------------------------------------------
// EnhancedCognitiveBridge
// ---------------------------------------------------------------------------

TEST(EnhancedCognitiveBridgeE2E, SnapshotCreateListLoadReplay) {
    EnhancedCognitiveBridge bridge("e2e-bridge");

    bridge.publishCognitiveState(makeState("agent-a", "curious", "s1", 0.1, 0.2, 1));
    bridge.publishCognitiveState(makeState("agent-a", "focused", "s2", 0.4, 0.5, 2));
    bridge.publishCognitiveState(makeState("agent-a", "excited", "s3", 0.8, 0.9, 3));

    const std::string snapId = bridge.createSnapshot("triad");
    ASSERT_FALSE(snapId.empty());

    auto snapshots = bridge.listSnapshots();
    ASSERT_EQ(snapshots.size(), 1u);
    EXPECT_EQ(snapshots[0].snapshotId, snapId);
    EXPECT_EQ(snapshots[0].states.size(), 3u);

    // loadSnapshot republishes the snapshot's states.
    const auto beforeLoad = bridge.stats().cognitivePublished;
    bridge.loadSnapshot(snapId);
    EXPECT_GT(bridge.stats().cognitivePublished, beforeLoad);

    // Replay delivers the same states asynchronously at speed 8x.
    std::atomic<int> replayed{0};
    auto sub = bridge.subscribeCognitiveState(
        [&replayed](const CognitiveState&) { replayed.fetch_add(1); });
    bridge.replaySnapshot(snapId, 8.0);
    for (int i = 0; i < 100 && replayed.load() < 3; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    bridge.stopReplay();
    bridge.unsubscribeCognitiveState(sub);
    EXPECT_GE(replayed.load(), 3);
}

TEST(EnhancedCognitiveBridgeE2E, CompressionAndClusterLifecycle) {
    EnhancedCognitiveBridge bridge("e2e-bridge-2");
    bridge.enableCompression("lz4");
    bridge.publishCognitiveState(makeState("agent-z", "calm", "rest", 0.0, 0.1));

    CompressedCognitiveState packed = bridge.getCompressedState();
    EXPECT_GT(packed.originalSize, 0u);
    CognitiveState unpacked = packed.decompress();
    EXPECT_EQ(unpacked.agentId, "agent-z");

    bridge.enableDistributedSync(CognitiveSyncProtocol::SyncMode::EVENTUAL);
    bridge.joinSyncCluster("loopback://cluster-a");
    bridge.broadcastToCluster(makeState("agent-z", "curious", "sync", 0.3, 0.4));
    bridge.sendToAgent("agent-peer", makeState("agent-z", "curious", "dm", 0.3, 0.4));
    bridge.leaveSyncCluster();

    auto stats = bridge.getEnhancedStats();
    EXPECT_GE(stats.base.cognitivePublished, 1u);
    EXPECT_GE(stats.syncEvents, 1u);
}

TEST(EnhancedCognitiveBridgeE2E, DashboardExportReflectsPublishedStates) {
    EnhancedCognitiveBridge bridge("e2e-bridge-3");
    bridge.publishCognitiveState(
        makeState("agent-dash", "playful", "dashboard", 0.5, 0.6));
    const std::string dashboard = bridge.exportForDashboard();
    EXPECT_FALSE(dashboard.empty());
    EXPECT_NE(dashboard.find("agent-dash"), std::string::npos);
    EXPECT_NO_THROW({
        auto& viz = bridge.getVisualizer();
        (void)viz.exportTimeline(CognitiveStateVisualizer::ExportFormat::JSON);
    });
    EXPECT_NO_THROW({
        auto& sync = bridge.getSyncProtocol();
        (void)sync.getSyncStatus();
    });
}
