// embodiment_test.cpp
// Tests for elizaos embodiment data types and PerceptionActionLoop lifecycle.

#include "elizaos/embodiment.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <string>

using namespace elizaos;

TEST(SensoryDataTypes, TextualDataConstruction) {
    TextualData d("hello world");
    EXPECT_EQ(d.text, "hello world");
    EXPECT_EQ(d.type, SensoryDataType::TEXTUAL);
}

TEST(SensoryDataTypes, VisualDataDefaults) {
    VisualData d;
    EXPECT_EQ(d.type, SensoryDataType::VISUAL);
}

TEST(SensoryDataTypes, AudioDataDefaults) {
    AudioData d;
    EXPECT_EQ(d.type, SensoryDataType::AUDITORY);
}

TEST(MotorActionTypes, SpeechActionConstruction) {
    SpeechAction a("hello");
    EXPECT_EQ(a.text, "hello");
    EXPECT_EQ(a.type, MotorActionType::SPEECH);
}

TEST(MotorActionTypes, MovementActionDefaults) {
    MovementAction a;
    EXPECT_EQ(a.type, MotorActionType::MOVEMENT);
}

TEST(MotorActionTypes, GestureActionConstruction) {
    GestureAction a("wave");
    EXPECT_EQ(a.gestureName, "wave");
}

TEST(MotorActionTypes, CommunicationActionConstruction) {
    CommunicationAction a("hello", "alice");
    EXPECT_EQ(a.message, "hello");
    EXPECT_EQ(a.recipient, "alice");
}

// PerceptionActionLoop requires shared_ptr<State> and AgentMemoryManager; covered by integration tests.
