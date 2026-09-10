#include <gtest/gtest.h>

#include "elizaos/livevideochat.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>

using namespace elizaos;

namespace {

struct CountingCallbacks final : MediaStreamCallbacks {
    void onVideoFrame(const std::string&, const VideoFrame&) override {
        std::lock_guard<std::mutex> lock(mutex);
        ++video_count;
        condition.notify_all();
    }
    void onAudioFrame(const std::string&, const AudioFrame&) override {
        std::lock_guard<std::mutex> lock(mutex);
        ++audio_count;
        condition.notify_all();
    }
    void onPeerConnectionStateChange(const std::string&, PeerConnectionState) override {}
    void onError(const std::string&, const std::string&) override { ++errors; }

    bool waitForVideo() {
        std::unique_lock<std::mutex> lock(mutex);
        return condition.wait_for(lock, std::chrono::seconds(1), [this] { return video_count > 0; });
    }

    std::mutex mutex;
    std::condition_variable condition;
    int video_count = 0;
    int audio_count = 0;
    std::atomic<int> errors{0};
};

VideoFrame videoFrame() {
    VideoFrame frame;
    frame.width = 2;
    frame.height = 2;
    frame.stride = 6;
    frame.format = "RGB24";
    frame.timestamp_ms = 1;
    frame.data.assign(12, 7);
    return frame;
}

} // namespace

TEST(LiveVideoChatPublicBehavior, DefaultProductionBackendIsUnsupported) {
    LiveVideoChat chat;
    EXPECT_FALSE(chat.initialize());
    EXPECT_EQ(chat.getVideoCapture(), nullptr);
    EXPECT_EQ(chat.getAudioCapture(), nullptr);
    EXPECT_EQ(chat.createSession("unavailable"), nullptr);
}

TEST(LiveVideoChatPublicBehavior, SyntheticCaptureMustBeExplicitlyConfiguredAndStarted) {
    auto video = createDeterministicSyntheticVideoCapture();
    auto audio = createDeterministicSyntheticAudioCapture();
    ASSERT_NE(video, nullptr);
    ASSERT_NE(audio, nullptr);
    EXPECT_TRUE(video->getNextFrame().data.empty());
    EXPECT_TRUE(audio->getNextFrame().samples.empty());
    EXPECT_FALSE(video->setConfig(0, 480, 30));
    EXPECT_FALSE(video->setConfig(640, 480, 0));
    EXPECT_FALSE(audio->setConfig(7999, 2));
    EXPECT_FALSE(audio->setConfig(48000, 0));
    ASSERT_TRUE(video->setConfig(4, 2, 25));
    ASSERT_TRUE(audio->setConfig(48000, 2));
    ASSERT_TRUE(video->start());
    ASSERT_TRUE(audio->start());
    const auto first_video = video->getNextFrame();
    const auto second_video = video->getNextFrame();
    EXPECT_EQ(first_video.data.size(), 24U);
    EXPECT_EQ(first_video.timestamp_ms, 0);
    EXPECT_EQ(second_video.timestamp_ms, 40);
    const auto first_audio = audio->getNextFrame();
    EXPECT_EQ(first_audio.samples.size(), 960U);
    EXPECT_EQ(first_audio.timestamp_ms, 0);
    video->stop();
    audio->stop();
    EXPECT_TRUE(video->getNextFrame().data.empty());
    EXPECT_TRUE(audio->getNextFrame().samples.empty());
}

TEST(LiveVideoChatPublicBehavior, LoopbackRequiresValidNegotiationAndAcknowledgesMedia) {
    const auto pair = createLoopbackPeerTransportPair("focused", "alice", "bob", 2);
    ASSERT_NE(pair.first, nullptr);
    ASSERT_NE(pair.second, nullptr);
    WebRTCPeerConnection alice(pair.first);
    WebRTCPeerConnection bob(pair.second);
    auto callbacks = std::make_shared<CountingCallbacks>();
    bob.setCallbacks(callbacks);
    VideoChatConfig config;
    ASSERT_TRUE(alice.initialize(config));
    ASSERT_TRUE(bob.initialize(config));
    EXPECT_FALSE(alice.setRemoteDescription("not sdp"));
    EXPECT_FALSE(alice.sendVideoFrame(videoFrame()));
    const auto offer = alice.createOffer();
    ASSERT_FALSE(offer.empty());
    const auto answer = bob.createAnswer(offer);
    ASSERT_FALSE(answer.empty());
    ASSERT_TRUE(alice.setRemoteDescription(answer));
    EXPECT_EQ(alice.getState(), PeerConnectionState::Connected);
    EXPECT_EQ(bob.getState(), PeerConnectionState::Connected);
    EXPECT_TRUE(alice.sendVideoFrame(videoFrame()));
    EXPECT_TRUE(callbacks->waitForVideo());
    alice.close();
    EXPECT_FALSE(alice.sendVideoFrame(videoFrame()));
}

TEST(LiveVideoChatPublicBehavior, RejectsInvalidLoopbackIdentityAndCapacity) {
    EXPECT_EQ(createLoopbackPeerTransportPair("", "alice", "bob").first, nullptr);
    EXPECT_EQ(createLoopbackPeerTransportPair("session", "alice", "alice").first, nullptr);
    EXPECT_EQ(createLoopbackPeerTransportPair("session", "alice", "bob", 0).first, nullptr);
}
