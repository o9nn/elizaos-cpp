// test_cognitive_bridge_protocol.cpp - WebSocket protocol conformance tests
// Validates that CognitiveBridge produces state conforming to the ElizaOS
// communication protocol specification.

#include <gtest/gtest.h>
#include "elizaos/cognitive_bridge.hpp"
#include "elizaos/core.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <mutex>

using namespace elizaos;
using namespace std::chrono_literals;

class CognitiveBridgeProtocolTest : public ::testing::Test {
protected:
    void SetUp() override {
        bridge_ = std::make_unique<CognitiveBridge>("protocol-test-bridge");
    }
    void TearDown() override {
        if (bridge_->isEchobeatsRunning()) {
            bridge_->stopEchobeats();
        }
        bridge_.reset();
    }
    std::unique_ptr<CognitiveBridge> bridge_;
};

// Protocol: cognitive_state_update message format
TEST_F(CognitiveBridgeProtocolTest, CognitiveStateUpdateFormat) {
    CognitiveState state;
    state.agentId = "eliza-001";
    state.mood = "reflective";
    state.focus = "user_emotion";
    state.echobeatsStep = 5;
    state.echobeatsPhase = 1;
    state.valence = 0.3;
    state.arousal = 0.5;
    state.metadata["cognitiveMode"] = "REFLECTIVE";

    std::vector<CognitiveState> received;
    std::mutex mu;
    bridge_->subscribeCognitiveState([&](const CognitiveState& s) {
        std::lock_guard<std::mutex> lock(mu);
        received.push_back(s);
    });

    bridge_->publishCognitiveState(state);
    std::this_thread::sleep_for(10ms);

    std::lock_guard<std::mutex> lock(mu);
    ASSERT_EQ(received.size(), 1u);
    EXPECT_EQ(received[0].agentId, "eliza-001");
    EXPECT_EQ(received[0].mood, "reflective");
    EXPECT_EQ(received[0].focus, "user_emotion");
    EXPECT_EQ(received[0].echobeatsStep, 5);
    EXPECT_EQ(received[0].echobeatsPhase, 1);
    EXPECT_DOUBLE_EQ(received[0].valence, 0.3);
    EXPECT_DOUBLE_EQ(received[0].arousal, 0.5);
    EXPECT_EQ(received[0].metadata["cognitiveMode"], "REFLECTIVE");
}

// Protocol: sensory_input message format
TEST_F(CognitiveBridgeProtocolTest, SensoryInputFormat) {
    SensoryInput input;
    input.sourceId = "frontend-001";
    input.modality = "text";
    input.payload = "I feel anxious about the deadline.";
    input.metadata["language"] = "en";
    input.metadata["confidence"] = "0.95";

    std::vector<SensoryInput> received;
    std::mutex mu;
    bridge_->subscribeSensoryInput([&](const SensoryInput& s) {
        std::lock_guard<std::mutex> lock(mu);
        received.push_back(s);
    });

    bridge_->publishSensoryInput(input);
    std::this_thread::sleep_for(10ms);

    std::lock_guard<std::mutex> lock(mu);
    ASSERT_EQ(received.size(), 1u);
    EXPECT_EQ(received[0].sourceId, "frontend-001");
    EXPECT_EQ(received[0].modality, "text");
    EXPECT_EQ(received[0].payload, "I feel anxious about the deadline.");
    EXPECT_EQ(received[0].metadata["language"], "en");
    EXPECT_EQ(received[0].metadata["confidence"], "0.95");
}

// Protocol: speech_output message format
TEST_F(CognitiveBridgeProtocolTest, SpeechOutputFormat) {
    SpeechOutput speech;
    speech.agentId = "eliza-001";
    speech.text = "How does that make you feel?";
    speech.voice = "eliza-default";
    speech.prosodyRate = 1.0;
    speech.prosodyPitch = 1.05;
    speech.metadata["emotion"] = "empathetic";

    std::vector<SpeechOutput> received;
    std::mutex mu;
    bridge_->subscribeSpeechOutput([&](const SpeechOutput& s) {
        std::lock_guard<std::mutex> lock(mu);
        received.push_back(s);
    });

    bridge_->publishSpeechOutput(speech);
    std::this_thread::sleep_for(10ms);

    std::lock_guard<std::mutex> lock(mu);
    ASSERT_EQ(received.size(), 1u);
    EXPECT_EQ(received[0].text, "How does that make you feel?");
    EXPECT_EQ(received[0].voice, "eliza-default");
    EXPECT_DOUBLE_EQ(received[0].prosodyRate, 1.0);
    EXPECT_DOUBLE_EQ(received[0].prosodyPitch, 1.05);
    EXPECT_EQ(received[0].metadata["emotion"], "empathetic");
}

// Protocol: Echobeats 12-step cycle phases
TEST_F(CognitiveBridgeProtocolTest, EchobeatsPhaseMapping) {
    EXPECT_EQ(CognitiveBridge::phaseForStep(1), 0);
    EXPECT_EQ(CognitiveBridge::phaseForStep(2), 1);
    EXPECT_EQ(CognitiveBridge::phaseForStep(3), 2);
    EXPECT_EQ(CognitiveBridge::phaseForStep(4), 3);
    EXPECT_EQ(CognitiveBridge::phaseForStep(5), 0);
    EXPECT_EQ(CognitiveBridge::phaseForStep(6), 1);
    EXPECT_EQ(CognitiveBridge::phaseForStep(7), 2);
    EXPECT_EQ(CognitiveBridge::phaseForStep(8), 3);
    EXPECT_EQ(CognitiveBridge::phaseForStep(9), 0);
    EXPECT_EQ(CognitiveBridge::phaseForStep(10), 1);
    EXPECT_EQ(CognitiveBridge::phaseForStep(11), 2);
    EXPECT_EQ(CognitiveBridge::phaseForStep(12), 3);
    EXPECT_EQ(CognitiveBridge::phaseForStep(0), 0);
}

// Protocol: Echobeats emits cognitive state updates at tick rate
TEST_F(CognitiveBridgeProtocolTest, EchobeatsEmitsCognitiveUpdates) {
    std::atomic<int> updateCount{0};
    bridge_->subscribeCognitiveState([&](const CognitiveState&) {
        updateCount.fetch_add(1);
    });

    bridge_->startEchobeats(50ms, "echobeats-agent");
    std::this_thread::sleep_for(300ms);
    bridge_->stopEchobeats();

    EXPECT_GE(updateCount.load(), 4);
    EXPECT_LE(updateCount.load(), 8);
}

// Protocol: Echobeats step advances within valid range
TEST_F(CognitiveBridgeProtocolTest, EchobeatsStepAdvancesInRange) {
    std::vector<int> steps;
    std::mutex mu;
    bridge_->subscribeCognitiveState([&](const CognitiveState& s) {
        std::lock_guard<std::mutex> lock(mu);
        steps.push_back(s.echobeatsStep);
    });

    bridge_->startEchobeats(20ms, "step-range-agent");
    std::this_thread::sleep_for(300ms);
    bridge_->stopEchobeats();

    std::lock_guard<std::mutex> lock(mu);
    ASSERT_GE(steps.size(), 5u);
    for (int s : steps) {
        EXPECT_GE(s, 1);
        EXPECT_LE(s, 12);
    }
}

// Protocol: Multiple subscribers receive same state
TEST_F(CognitiveBridgeProtocolTest, MultipleSubscribersReceiveSameState) {
    std::atomic<int> sub1Count{0};
    std::atomic<int> sub2Count{0};

    bridge_->subscribeCognitiveState([&](const CognitiveState&) {
        sub1Count.fetch_add(1);
    });
    bridge_->subscribeCognitiveState([&](const CognitiveState&) {
        sub2Count.fetch_add(1);
    });

    CognitiveState state;
    state.agentId = "multi-test";
    bridge_->publishCognitiveState(state);
    std::this_thread::sleep_for(10ms);

    EXPECT_EQ(sub1Count.load(), 1);
    EXPECT_EQ(sub2Count.load(), 1);
}

// Protocol: Unsubscribe stops delivery
TEST_F(CognitiveBridgeProtocolTest, UnsubscribeStopsDelivery) {
    std::atomic<int> count{0};
    auto subId = bridge_->subscribeCognitiveState([&](const CognitiveState&) {
        count.fetch_add(1);
    });

    CognitiveState state;
    state.agentId = "unsub-test";
    bridge_->publishCognitiveState(state);
    std::this_thread::sleep_for(10ms);
    EXPECT_EQ(count.load(), 1);

    bridge_->unsubscribeCognitiveState(subId);
    bridge_->publishCognitiveState(state);
    std::this_thread::sleep_for(10ms);
    EXPECT_EQ(count.load(), 1);
}

// Protocol: Stats tracking is accurate
TEST_F(CognitiveBridgeProtocolTest, StatsTrackingAccurate) {
    CognitiveState cs;
    cs.agentId = "stats-test";
    SensoryInput si;
    si.sourceId = "stats-test";
    SpeechOutput so;
    so.agentId = "stats-test";

    bridge_->publishCognitiveState(cs);
    bridge_->publishCognitiveState(cs);
    bridge_->publishSensoryInput(si);
    bridge_->publishSpeechOutput(so);
    bridge_->publishSpeechOutput(so);
    bridge_->publishSpeechOutput(so);

    auto stats = bridge_->stats();
    EXPECT_EQ(stats.cognitivePublished, 2u);
    EXPECT_EQ(stats.sensoryPublished, 1u);
    EXPECT_EQ(stats.speechPublished, 3u);
}

// Protocol: History ring buffer respects capacity
TEST_F(CognitiveBridgeProtocolTest, HistoryRingBufferBounded) {
    for (int i = 0; i < 300; ++i) {
        CognitiveState cs;
        cs.agentId = "history-" + std::to_string(i);
        bridge_->publishCognitiveState(cs);
    }

    auto recent = bridge_->recentCognitiveStates(16);
    EXPECT_LE(recent.size(), 16u);
    if (!recent.empty()) {
        EXPECT_TRUE(recent.back().agentId.find("history-") != std::string::npos);
    }
}

// Protocol: Bidirectional flow - sensory triggers cognitive response
TEST_F(CognitiveBridgeProtocolTest, BidirectionalFlowPattern) {
    std::atomic<bool> gotCognitive{false};
    std::atomic<bool> gotSpeech{false};

    bridge_->subscribeSensoryInput([&](const SensoryInput& input) {
        CognitiveState response;
        response.agentId = "eliza";
        response.mood = "attentive";
        response.valence = 0.4;
        bridge_->publishCognitiveState(response);

        SpeechOutput speech;
        speech.agentId = "eliza";
        speech.text = "I understand: " + input.payload;
        bridge_->publishSpeechOutput(speech);
    });

    bridge_->subscribeCognitiveState([&](const CognitiveState& s) {
        if (s.mood == "attentive") gotCognitive.store(true);
    });
    bridge_->subscribeSpeechOutput([&](const SpeechOutput& s) {
        if (s.text.find("I understand") != std::string::npos) gotSpeech.store(true);
    });

    SensoryInput userInput;
    userInput.sourceId = "user";
    userInput.modality = "text";
    userInput.payload = "Hello Eliza";
    bridge_->publishSensoryInput(userInput);

    std::this_thread::sleep_for(50ms);
    EXPECT_TRUE(gotCognitive.load());
    EXPECT_TRUE(gotSpeech.load());
}

// Protocol: Concurrent publish from multiple threads is safe
TEST_F(CognitiveBridgeProtocolTest, ConcurrentPublishSafe) {
    std::atomic<int> totalReceived{0};
    bridge_->subscribeCognitiveState([&](const CognitiveState&) {
        totalReceived.fetch_add(1);
    });

    constexpr int numThreads = 4;
    constexpr int publishesPerThread = 50;
    std::vector<std::thread> threads;

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < publishesPerThread; ++i) {
                CognitiveState cs;
                cs.agentId = "thread-" + std::to_string(t) + "-" + std::to_string(i);
                bridge_->publishCognitiveState(cs);
            }
        });
    }

    for (auto& th : threads) th.join();
    std::this_thread::sleep_for(50ms);

    EXPECT_EQ(totalReceived.load(), numThreads * publishesPerThread);
}
