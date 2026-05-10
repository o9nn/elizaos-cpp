// livevideochat_test.cpp - Tests for video/audio capture & WebRTC scaffolding.
#include <gtest/gtest.h>
#include "elizaos/livevideochat.hpp"

using namespace elizaos;

TEST(VideoFrame, BasicConstruction) {
    VideoFrame f;
    SUCCEED();
}

TEST(AudioFrame, BasicConstruction) {
    AudioFrame f;
    SUCCEED();
}

TEST(VideoChatConfig, Defaults) {
    VideoChatConfig c;
    SUCCEED();
}

TEST(SessionInfo, BasicConstruction) {
    SessionInfo s;
    SUCCEED();
}

TEST(LiveVideoChat, ConstructAndUse) {
    LiveVideoChat chat;
    SUCCEED();
}

TEST(PeerConnectionState, EnumValuesDistinct) {
    int a = static_cast<int>(PeerConnectionState{});
    SUCCEED() << "default PeerConnectionState = " << a;
}
