// test_embodiment_cognitive_integration.cpp
// Tests the integration between the Embodiment subsystem and the CognitiveBridge.
// Validates sensory data flows through the perception-action loop and
// produces cognitive state updates via the bridge.

#include <gtest/gtest.h>
#include "elizaos/embodiment.hpp"
#include "elizaos/cognitive_bridge.hpp"
#include "elizaos/core.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <vector>
#include <fstream>
#include <filesystem>

using namespace elizaos;
using namespace std::chrono_literals;

class EmbodimentCognitiveIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        bridge_ = std::make_unique<CognitiveBridge>("embodiment-integration");
    }
    void TearDown() override {
        if (bridge_->isEchobeatsRunning()) {
            bridge_->stopEchobeats();
        }
        bridge_.reset();
    }
    std::unique_ptr<CognitiveBridge> bridge_;
};

// Test: MockMotorInterface executes actions and reports completion
TEST_F(EmbodimentCognitiveIntegrationTest, MockMotorExecutesActions) {
    MockMotorInterface motor(MotorActionType::MOVEMENT);
    EXPECT_TRUE(motor.initialize());
    EXPECT_TRUE(motor.isActive());

    auto action = std::make_shared<MotorAction>(MotorActionType::MOVEMENT, "target-001");
    action->parameters["direction"] = "forward";
    action->parameters["speed"] = "1.0";

    EXPECT_TRUE(motor.canExecute(action));
    EXPECT_TRUE(motor.executeAction(action));

    auto executed = motor.getExecutedActions();
    ASSERT_EQ(executed.size(), 1u);
    EXPECT_EQ(executed[0]->target, "target-001");

    motor.clearExecutedActions();
    EXPECT_EQ(motor.getExecutedActions().size(), 0u);

    motor.shutdown();
    EXPECT_FALSE(motor.isActive());
}

// Test: FileSensoryInterface reads data when file exists
TEST_F(EmbodimentCognitiveIntegrationTest, FileSensoryReadsData) {
    auto tmpPath = std::filesystem::temp_directory_path() / "elizaos_sensory_test.txt";
    {
        std::ofstream f(tmpPath);
        f << "visual:object_detected:confidence=0.95\n";
        f << "audio:speech_detected:confidence=0.88\n";
    }

    FileSensoryInterface sensor(SensoryDataType::VISUAL, tmpPath.string());
    EXPECT_TRUE(sensor.initialize());
    EXPECT_TRUE(sensor.isActive());

    if (sensor.hasData()) {
        auto data = sensor.readData();
        EXPECT_NE(data, nullptr);
        if (data) {
            EXPECT_EQ(data->type, SensoryDataType::VISUAL);
        }
    }

    sensor.shutdown();
    EXPECT_FALSE(sensor.isActive());
    std::filesystem::remove(tmpPath);
}

// Test: ConsoleTextOutput interface initializes and executes
TEST_F(EmbodimentCognitiveIntegrationTest, ConsoleTextOutputInitializes) {
    ConsoleTextOutput textOutput;
    EXPECT_TRUE(textOutput.initialize());
    EXPECT_TRUE(textOutput.isActive());
    EXPECT_EQ(textOutput.getType(), MotorActionType::COMMUNICATION);
    EXPECT_EQ(textOutput.getName(), "ConsoleTextOutput");

    // Execute a communication action
    auto action = std::make_shared<CommunicationAction>("Hello world", "console");
    EXPECT_TRUE(textOutput.canExecute(action));
    EXPECT_TRUE(textOutput.executeAction(action));

    textOutput.shutdown();
    EXPECT_FALSE(textOutput.isActive());
}

// Test: MockMotorInterface configuration
TEST_F(EmbodimentCognitiveIntegrationTest, MockMotorConfiguration) {
    MockMotorInterface motor(MotorActionType::GESTURE);
    motor.initialize();

    std::unordered_map<std::string, std::string> config;
    config["speed_limit"] = "2.0";
    config["precision"] = "high";
    motor.setConfiguration(config);

    auto retrieved = motor.getConfiguration();
    EXPECT_EQ(retrieved["speed_limit"], "2.0");
    EXPECT_EQ(retrieved["precision"], "high");

    motor.shutdown();
}

// Test: Embodiment feeds sensory data that drives cognitive state via bridge
TEST_F(EmbodimentCognitiveIntegrationTest, SensoryDrivesCognitiveState) {
    std::atomic<int> cognitiveUpdates{0};

    bridge_->subscribeCognitiveState([&](const CognitiveState&) {
        cognitiveUpdates.fetch_add(1);
    });

    // Simulate embodiment producing sensory data that triggers cognitive update
    bridge_->subscribeSensoryInput([&](const SensoryInput& input) {
        CognitiveState response;
        response.agentId = "eliza";
        response.mood = "perceiving";
        response.focus = input.payload;
        response.arousal = 0.6;
        bridge_->publishCognitiveState(response);
    });

    SensoryInput sensory;
    sensory.sourceId = "embodiment-visual";
    sensory.modality = "vision";
    sensory.payload = "object_detected";
    bridge_->publishSensoryInput(sensory);

    std::this_thread::sleep_for(50ms);
    EXPECT_GE(cognitiveUpdates.load(), 1);
}

// Test: Motor actions triggered by cognitive state changes
TEST_F(EmbodimentCognitiveIntegrationTest, CognitiveTriggersMotorAction) {
    MockMotorInterface motor(MotorActionType::COMMUNICATION);
    motor.initialize();

    std::atomic<bool> actionTriggered{false};

    bridge_->subscribeCognitiveState([&](const CognitiveState& state) {
        if (state.mood == "speaking") {
            auto action = std::make_shared<CommunicationAction>("Hello world", "user");
            motor.executeAction(action);
            actionTriggered.store(true);
        }
    });

    CognitiveState speakState;
    speakState.agentId = "eliza";
    speakState.mood = "speaking";
    bridge_->publishCognitiveState(speakState);

    std::this_thread::sleep_for(50ms);
    EXPECT_TRUE(actionTriggered.load());
    EXPECT_EQ(motor.getExecutedActions().size(), 1u);

    motor.shutdown();
}

// Test: Full embodiment loop with Echobeats driving the cycle
TEST_F(EmbodimentCognitiveIntegrationTest, EchobeatsDrivesEmbodimentCycle) {
    std::atomic<int> perceptionCycles{0};
    MockMotorInterface motor(MotorActionType::MOVEMENT);
    motor.initialize();

    bridge_->subscribeCognitiveState([&](const CognitiveState& state) {
        if (state.echobeatsStep % 3 == 0) {
            perceptionCycles.fetch_add(1);
        }
    });

    bridge_->startEchobeats(30ms, "embodiment-agent");
    std::this_thread::sleep_for(200ms);
    bridge_->stopEchobeats();

    EXPECT_GE(perceptionCycles.load(), 1);
    motor.shutdown();
}

// Test: Bidirectional embodiment-cognitive loop
TEST_F(EmbodimentCognitiveIntegrationTest, BidirectionalEmbodimentCognitiveLoop) {
    MockMotorInterface motor(MotorActionType::SPEECH);
    motor.initialize();

    std::atomic<int> speechActions{0};

    // Sensory -> Cognitive -> Speech pipeline
    bridge_->subscribeSensoryInput([&](const SensoryInput& input) {
        CognitiveState cs;
        cs.agentId = "eliza";
        cs.mood = "responding";
        cs.focus = input.payload;
        cs.valence = 0.5;
        bridge_->publishCognitiveState(cs);
    });

    bridge_->subscribeCognitiveState([&](const CognitiveState& state) {
        if (state.mood == "responding") {
            SpeechOutput speech;
            speech.agentId = "eliza";
            speech.text = "Processing: " + state.focus;
            bridge_->publishSpeechOutput(speech);
        }
    });

    bridge_->subscribeSpeechOutput([&](const SpeechOutput& speech) {
        auto action = std::make_shared<SpeechAction>(speech.text);
        motor.executeAction(action);
        speechActions.fetch_add(1);
    });

    // Trigger the pipeline
    SensoryInput input;
    input.sourceId = "user-text";
    input.modality = "text";
    input.payload = "hello";
    bridge_->publishSensoryInput(input);

    std::this_thread::sleep_for(100ms);
    EXPECT_GE(speechActions.load(), 1);
    EXPECT_GE(motor.getExecutedActions().size(), 1u);

    motor.shutdown();
}

// Test: Multiple motor interfaces coordinated
TEST_F(EmbodimentCognitiveIntegrationTest, MultipleMotorsCoordinated) {
    MockMotorInterface speechMotor(MotorActionType::SPEECH);
    MockMotorInterface gestureMotor(MotorActionType::GESTURE);
    speechMotor.initialize();
    gestureMotor.initialize();

    // Cognitive state triggers both speech and gesture
    bridge_->subscribeCognitiveState([&](const CognitiveState& state) {
        if (state.mood == "greeting") {
            auto speech = std::make_shared<SpeechAction>("Hello!");
            speechMotor.executeAction(speech);
            auto gesture = std::make_shared<GestureAction>("wave");
            gestureMotor.executeAction(gesture);
        }
    });

    CognitiveState greetState;
    greetState.agentId = "eliza";
    greetState.mood = "greeting";
    bridge_->publishCognitiveState(greetState);

    std::this_thread::sleep_for(50ms);
    EXPECT_EQ(speechMotor.getExecutedActions().size(), 1u);
    EXPECT_EQ(gestureMotor.getExecutedActions().size(), 1u);

    speechMotor.shutdown();
    gestureMotor.shutdown();
}
