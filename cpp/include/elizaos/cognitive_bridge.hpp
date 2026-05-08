#pragma once

/**
 * ElizaOS C++ - CognitiveBridge Module
 *
 * Publish/subscribe hub for cognitive state, sensory inputs, and speech
 * outputs.  Also drives the 12-step Echobeats cycle.
 */

#include "elizaos.hpp"
#include <atomic>
#include <chrono>
#include <deque>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace elizaos {

// ============================================================================
// Data structures
// ============================================================================

struct CognitiveState {
    std::string agentId;
    std::string mood       = "neutral";
    std::string focus;
    std::string reasoning;
    int         echobeatsStep  = 0;
    int         echobeatsPhase = 0;
    double      confidenceLevel = 0.0;
    std::unordered_map<std::string, std::string> context;
};

struct SensoryInput {
    std::string type;      // e.g. "visual", "auditory", "text"
    std::string content;
    std::string sourceId;
    double      confidence = 1.0;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
};

struct SpeechOutput {
    std::string text;
    std::string emotion    = "neutral";
    int         priority   = 0;
    std::string recipientId;
    std::unordered_map<std::string, std::string> metadata;
};

// ============================================================================
// Handler and subscription types
// ============================================================================

using SubscriptionId         = uint64_t;
using CognitiveStateHandler  = std::function<void(const CognitiveState&)>;
using SensoryInputHandler    = std::function<void(const SensoryInput&)>;
using SpeechOutputHandler    = std::function<void(const SpeechOutput&)>;

// ============================================================================
// CognitiveBridge
// ============================================================================

class CognitiveBridge {
public:
    struct Stats {
        uint64_t cognitivePublished = 0;
        uint64_t sensoryPublished   = 0;
        uint64_t speechPublished    = 0;
    };

    static constexpr size_t kHistoryCap = 200;

    explicit CognitiveBridge(std::string bridgeId = "default");
    ~CognitiveBridge();

    // Publishing
    void publishCognitiveState(const CognitiveState& state);
    void publishSensoryInput(const SensoryInput& input);
    void publishSpeechOutput(const SpeechOutput& speech);

    // Subscriptions
    SubscriptionId subscribeCognitiveState(CognitiveStateHandler handler);
    SubscriptionId subscribeSensoryInput(SensoryInputHandler    handler);
    SubscriptionId subscribeSpeechOutput(SpeechOutputHandler    handler);

    void unsubscribeCognitiveState(SubscriptionId id);
    void unsubscribeSensoryInput(SubscriptionId id);
    void unsubscribeSpeechOutput(SubscriptionId id);

    // Echobeats (12-step heartbeat cycle)
    void startEchobeats(std::chrono::milliseconds stepInterval = std::chrono::milliseconds(100),
                        std::string agentId = "default");
    void stopEchobeats();
    int  currentPhase() const;

    // History
    std::vector<CognitiveState>  recentCognitiveStates(size_t limit = 10) const;
    std::vector<SensoryInput>    recentSensoryInputs(size_t limit = 10) const;
    std::vector<SpeechOutput>    recentSpeechOutputs(size_t limit = 10) const;

    // Statistics
    Stats stats() const;

    const std::string& bridgeId() const { return bridgeId_; }

private:
    static int phaseForStep(int step);
    void echobeatsLoop(std::chrono::milliseconds stepInterval, std::string agentId);

    template<typename Map>
    struct SubGroup {
        std::mutex mu;
        Map        map;
    };

    std::string bridgeId_;

    SubGroup<std::unordered_map<SubscriptionId, CognitiveStateHandler>> cognitiveSubs_;
    SubGroup<std::unordered_map<SubscriptionId, SensoryInputHandler>>   sensorySubs_;
    SubGroup<std::unordered_map<SubscriptionId, SpeechOutputHandler>>   speechSubs_;

    mutable std::mutex          historyMu_;
    std::deque<CognitiveState>  cognitiveHistory_;
    std::deque<SensoryInput>    sensoryHistory_;
    std::deque<SpeechOutput>    speechHistory_;

    mutable std::mutex statsMu_;
    Stats              stats_;

    std::thread          echobeatsThread_;
    std::atomic<bool>    echobeatsRunning_{false};
    std::atomic<int>     currentStep_{0};
    std::atomic<uint64_t> nextId_{1};
};

} // namespace elizaos
