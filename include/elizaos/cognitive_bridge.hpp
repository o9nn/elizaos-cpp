#pragma once

/**
 * ElizaOS C++ — CognitiveBridge module.
 *
 * Implements the in-process cognitive state / sensory input / speech output
 * pub/sub bus required by the eliza-os skill. The bridge exposes the same
 * surface as a WebSocket-based bridge would, but does so without any
 * networking so it is fully reproducible in CI environments.
 *
 * The bridge supports the three logical channels described in the eliza-os
 * skill:
 *
 *   1. cognitive_state_update — emitted by the cognitive engine to renderers
 *   2. sensory_input          — emitted by renderers/sensors to the engine
 *   3. speech_output          — emitted by the engine to speech subsystems
 *
 * It also tracks an Echobeats 12-step cycle with three concurrently phased
 * threads (steps {1,5,9}, {2,6,10}, {3,7,11}, {4,8,12}) so downstream
 * components can subscribe to phase-locked tick events in the way the
 * Echobeats architecture knowledge prescribes.
 */

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace elizaos {

/**
 * A single cognitive state snapshot emitted on the cognitive_state_update
 * channel. The bridge is intentionally schemaless — it lets downstream
 * consumers attach any fields they want via the metadata map.
 */
struct CognitiveState {
    std::string agentId;
    std::string mood;          ///< e.g. "curious", "excited", "calm"
    std::string focus;         ///< what the agent is attending to
    int echobeatsStep = 0;     ///< 1..12 within the Echobeats cycle
    int echobeatsPhase = 0;    ///< 0..3, the dyadic-pair this step belongs to
    double valence = 0.0;      ///< -1.0 .. +1.0 emotional valence
    double arousal = 0.0;      ///<  0.0 .. +1.0 arousal level
    std::unordered_map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
};

/**
 * A sensory input event flowing into the cognitive engine.
 */
struct SensoryInput {
    std::string sourceId;
    std::string modality;      ///< "text", "audio", "vision", "telemetry"
    std::string payload;
    std::unordered_map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
};

/**
 * A speech output event flowing from the cognitive engine to a TTS / avatar.
 */
struct SpeechOutput {
    std::string agentId;
    std::string text;
    std::string voice;
    double prosodyRate = 1.0;
    double prosodyPitch = 1.0;
    std::unordered_map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
};

/// Subscriber callbacks for each of the three channels.
using CognitiveStateHandler = std::function<void(const CognitiveState&)>;
using SensoryInputHandler   = std::function<void(const SensoryInput&)>;
using SpeechOutputHandler   = std::function<void(const SpeechOutput&)>;

/// A subscription token returned by the subscribe* APIs.
using SubscriptionId = std::uint64_t;

/**
 * In-process implementation of the cognitive bridge described above.
 *
 * Thread-safety: all public methods are safe to call from multiple threads
 * concurrently. Handlers are dispatched on the publishing thread (sensory,
 * cognitive, speech) so subscribers should keep their work short.
 */
class CognitiveBridge {
public:
    explicit CognitiveBridge(std::string bridgeId = "cognitive-bridge");
    ~CognitiveBridge();

    CognitiveBridge(const CognitiveBridge&) = delete;
    CognitiveBridge& operator=(const CognitiveBridge&) = delete;

    /// Identity of this bridge instance.
    const std::string& id() const { return bridgeId_; }

    // -------------------- Publishing --------------------
    /// Emit a cognitive state update to all subscribed renderers.
    void publishCognitiveState(const CognitiveState& state);
    /// Emit a sensory input to all subscribed cognitive engines.
    void publishSensoryInput(const SensoryInput& input);
    /// Emit a speech output to all subscribed speech subsystems.
    void publishSpeechOutput(const SpeechOutput& speech);

    // -------------------- Subscriptions --------------------
    SubscriptionId subscribeCognitiveState(CognitiveStateHandler handler);
    SubscriptionId subscribeSensoryInput(SensoryInputHandler handler);
    SubscriptionId subscribeSpeechOutput(SpeechOutputHandler handler);
    void unsubscribeCognitiveState(SubscriptionId id);
    void unsubscribeSensoryInput(SubscriptionId id);
    void unsubscribeSpeechOutput(SubscriptionId id);

    // -------------------- Echobeats cycle --------------------
    /**
     * Start the Echobeats clock at the given step interval (default 100 ms
     * per step → ~1.2 s per full cycle). Each tick emits a default
     * CognitiveState carrying the current step/phase. Subscribers can use
     * this as the system heartbeat.
     */
    void startEchobeats(std::chrono::milliseconds stepInterval = std::chrono::milliseconds(100),
                        std::string agentId = "default");
    void stopEchobeats();
    bool isEchobeatsRunning() const { return echobeatsRunning_.load(); }

    /// Return the most recent step (1..12), or 0 if not yet started.
    int currentStep() const { return currentStep_.load(); }

    /// Return the phase index (0..3) for the most recent step.
    int currentPhase() const;

    // -------------------- History (for tests / debugging) --------------------
    std::vector<CognitiveState> recentCognitiveStates(size_t limit = 16) const;
    std::vector<SensoryInput>   recentSensoryInputs(size_t limit = 16) const;
    std::vector<SpeechOutput>   recentSpeechOutputs(size_t limit = 16) const;

    /// Total publishes since construction.
    struct Stats {
        std::uint64_t cognitivePublished = 0;
        std::uint64_t sensoryPublished = 0;
        std::uint64_t speechPublished = 0;
        std::uint64_t echobeatsTicks = 0;
    };
    Stats stats() const;

    /**
     * Compute the Echobeats phase (0..3) for a step. Step 1..12 maps to
     * phases by ((step-1) mod 4); step 0 maps to 0. Inputs outside 1..12
     * are first reduced modulo 12.
     */
    static int phaseForStep(int step);

private:
    void echobeatsLoop(std::chrono::milliseconds stepInterval, std::string agentId);

    // Channel-specific subscriber registries.
    template <typename Handler>
    struct Subscribers {
        std::mutex mu;
        std::unordered_map<SubscriptionId, Handler> map;
    };

    std::string bridgeId_;

    Subscribers<CognitiveStateHandler> cognitiveSubs_;
    Subscribers<SensoryInputHandler>   sensorySubs_;
    Subscribers<SpeechOutputHandler>   speechSubs_;
    std::atomic<SubscriptionId>        nextId_{1};

    // Bounded ring buffers for inspection.
    mutable std::mutex historyMu_;
    std::deque<CognitiveState> cognitiveHistory_;
    std::deque<SensoryInput>   sensoryHistory_;
    std::deque<SpeechOutput>   speechHistory_;
    static constexpr size_t kHistoryCap = 256;

    // Echobeats clock state.
    std::atomic<bool> echobeatsRunning_{false};
    std::atomic<int>  currentStep_{0};
    std::thread echobeatsThread_;

    // Stats.
    mutable std::mutex statsMu_;
    Stats stats_;
};

} // namespace elizaos

// ============================================================================
// Phase 3.2: Distributed Cognitive State System
// ============================================================================

namespace elizaos {

/**
 * Compressed cognitive state for network transfer
 */
struct CompressedCognitiveState {
    std::vector<uint8_t> data;
    std::string compressionAlgorithm;  // "lz4", "zstd", "none"
    size_t originalSize;
    std::string checksum;
    
    static CompressedCognitiveState compress(const CognitiveState& state, 
                                             const std::string& algorithm = "lz4");
    CognitiveState decompress() const;
};

/**
 * Cognitive state snapshot for replay/debugging
 */
struct CognitiveSnapshot {
    std::string snapshotId;
    std::vector<CognitiveState> states;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::string agentId;
    std::unordered_map<std::string, std::string> annotations;
};

/**
 * Multi-agent cognitive synchronization protocol
 */
class CognitiveSyncProtocol {
public:
    enum class SyncMode {
        LEADER_FOLLOWER,   // One agent leads, others follow
        CONSENSUS,         // Distributed consensus (Raft-like)
        EVENTUAL,          // Eventually consistent
        REAL_TIME          // Strong real-time sync
    };
    
    struct SyncConfig {
        SyncMode mode = SyncMode::EVENTUAL;
        std::chrono::milliseconds syncInterval{100};
        double conflictThreshold = 0.1;  // Max acceptable state divergence
        int quorumSize = 0;              // For consensus mode
    };
    
    CognitiveSyncProtocol(const std::string& agentId, const SyncConfig& config);
    ~CognitiveSyncProtocol();
    
    // Agent registration
    void registerAgent(const std::string& agentId);
    void unregisterAgent(const std::string& agentId);
    std::vector<std::string> getRegisteredAgents() const;
    
    // State synchronization
    void publishLocalState(const CognitiveState& state);
    std::vector<CognitiveState> getRemoteStates() const;
    CognitiveState getConsensusState() const;
    
    // Conflict handling
    struct ConflictInfo {
        std::string field;
        std::vector<std::pair<std::string, std::string>> agentValues;  // agentId -> value
        std::string resolvedValue;
        bool autoResolved;
    };
    std::vector<ConflictInfo> detectConflicts() const;
    void resolveConflict(const std::string& field, const std::string& resolution);
    
    // Leader election (for LEADER_FOLLOWER mode)
    void nominateSelfAsLeader();
    std::string getCurrentLeader() const;
    bool isLeader() const;
    
    // Synchronization status
    struct SyncStatus {
        bool synchronized;
        double divergenceScore;
        std::chrono::milliseconds lastSyncLatency;
        int pendingUpdates;
    };
    SyncStatus getSyncStatus() const;
    
private:
    std::string localAgentId_;
    SyncConfig config_;
    std::unordered_map<std::string, CognitiveState> remoteStates_;
    std::string currentLeader_;
    mutable std::mutex syncMutex_;
    std::thread syncThread_;
    std::atomic<bool> running_{false};
};

/**
 * State visualization export for dashboards
 */
class CognitiveStateVisualizer {
public:
    enum class ExportFormat {
        JSON,
        GRAPHVIZ_DOT,
        D3_COMPATIBLE,
        PROMETHEUS_METRICS
    };
    
    CognitiveStateVisualizer();
    
    // Add states for visualization
    void addState(const CognitiveState& state);
    void addTransition(const CognitiveState& from, const CognitiveState& to);
    
    // Export
    std::string exportTimeline(ExportFormat format = ExportFormat::JSON) const;
    std::string exportStateGraph(ExportFormat format = ExportFormat::GRAPHVIZ_DOT) const;
    std::string exportMetrics(ExportFormat format = ExportFormat::PROMETHEUS_METRICS) const;
    
    // Real-time streaming
    using StreamCallback = std::function<void(const std::string& data)>;
    void startStreaming(StreamCallback callback, std::chrono::milliseconds interval);
    void stopStreaming();
    
private:
    /// Render Prometheus metrics; caller must hold vizMutex_.
    std::string exportMetricsUnlocked() const;

    std::vector<CognitiveState> states_;
    std::vector<std::pair<size_t, size_t>> transitions_;
    StreamCallback streamCallback_;
    std::thread streamThread_;
    std::atomic<bool> streaming_{false};
    mutable std::mutex vizMutex_;
};

/**
 * Enhanced cognitive bridge with distributed capabilities
 */
class EnhancedCognitiveBridge : public CognitiveBridge {
public:
    EnhancedCognitiveBridge(const std::string& bridgeId = "enhanced-cognitive-bridge");
    ~EnhancedCognitiveBridge();
    
    // Distributed sync
    CognitiveSyncProtocol& getSyncProtocol();
    void enableDistributedSync(CognitiveSyncProtocol::SyncMode mode);
    void joinSyncCluster(const std::string& clusterUrl);
    void leaveSyncCluster();
    
    // State compression for network
    void enableCompression(const std::string& algorithm = "lz4");
    CompressedCognitiveState getCompressedState() const;
    
    // Snapshot/replay
    std::string createSnapshot(const std::string& name = "");
    void loadSnapshot(const std::string& snapshotId);
    std::vector<CognitiveSnapshot> listSnapshots() const;
    void replaySnapshot(const std::string& snapshotId, double speedMultiplier = 1.0);
    void stopReplay();
    
    // Visualization
    CognitiveStateVisualizer& getVisualizer();
    std::string exportForDashboard() const;
    
    // Multi-agent coordination
    void broadcastToCluster(const CognitiveState& state);
    void sendToAgent(const std::string& agentId, const CognitiveState& state);
    
    // Enhanced statistics
    struct EnhancedStats {
        Stats base;
        uint64_t syncEvents;
        uint64_t conflictsResolved;
        double averageSyncLatency;
        size_t compressedBytesTransferred;
    };
    EnhancedStats getEnhancedStats() const;
    
private:
    std::unique_ptr<CognitiveSyncProtocol> syncProtocol_;
    std::unique_ptr<CognitiveStateVisualizer> visualizer_;
    std::vector<CognitiveSnapshot> snapshots_;
    std::string compressionAlgorithm_;
    bool compressionEnabled_ = false;
    std::string clusterUrl_;
    bool joinedCluster_ = false;
    mutable std::uint64_t syncEvents_ = 0;
    mutable std::uint64_t conflictsResolved_ = 0;
    mutable std::uint64_t compressedBytesTransferred_ = 0;
    mutable std::mutex enhancedMutex_;
    
    std::thread replayThread_;
    std::atomic<bool> replaying_{false};
};

} // namespace elizaos
