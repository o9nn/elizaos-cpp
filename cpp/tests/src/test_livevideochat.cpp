#include <gtest/gtest.h>

#include "elizaos/livevideochat.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <limits>
#include <mutex>
#include <string>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace elizaos;

namespace {

constexpr const char* kCandidate =
    "candidate:1 1 UDP 2130706431 127.0.0.1 54400 typ host";

VideoFrame makeVideoFrame(std::uint8_t value = 7) {
    VideoFrame frame;
    frame.width = 2;
    frame.height = 2;
    frame.stride = 6;
    frame.format = "RGB24";
    frame.timestamp_ms = 1;
    frame.data.assign(12, value);
    return frame;
}

AudioFrame makeAudioFrame(float value = 0.25F) {
    AudioFrame frame;
    frame.sample_rate = 48000;
    frame.channels = 2;
    frame.timestamp_ms = 1;
    frame.samples.assign(960, value);
    return frame;
}

bool negotiate(WebRTCPeerConnection& offerer, WebRTCPeerConnection& answerer) {
    const std::string offer = offerer.createOffer();
    if (offer.empty()) return false;
    const std::string answer = answerer.createAnswer(offer);
    return !answer.empty() && offerer.setRemoteDescription(answer);
}

class RecordingCallbacks final : public MediaStreamCallbacks {
public:
    void onVideoFrame(const std::string& peer_id, const VideoFrame&) override {
        std::lock_guard<std::mutex> lock(mutex_);
        ++video_frames_;
        last_peer_ = peer_id;
        condition_.notify_all();
    }
    void onAudioFrame(const std::string& peer_id, const AudioFrame&) override {
        std::lock_guard<std::mutex> lock(mutex_);
        ++audio_frames_;
        last_peer_ = peer_id;
        condition_.notify_all();
    }
    void onPeerConnectionStateChange(const std::string& peer_id,
                                     PeerConnectionState) override {
        std::lock_guard<std::mutex> lock(mutex_);
        last_peer_ = peer_id;
        condition_.notify_all();
    }
    void onError(const std::string& peer_id, const std::string&) override {
        std::lock_guard<std::mutex> lock(mutex_);
        ++errors_;
        last_peer_ = peer_id;
        condition_.notify_all();
    }
    bool waitForMedia(int video_frames, int audio_frames) {
        std::unique_lock<std::mutex> lock(mutex_);
        return condition_.wait_for(lock, std::chrono::seconds(1), [&] {
            return video_frames_ >= video_frames && audio_frames_ >= audio_frames;
        });
    }
    int errors() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return errors_;
    }
    std::string lastPeer() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return last_peer_;
    }
private:
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    int video_frames_ = 0;
    int audio_frames_ = 0;
    int errors_ = 0;
    std::string last_peer_;
};

class BlockingCallbacks final : public MediaStreamCallbacks {
public:
    void onVideoFrame(const std::string&, const VideoFrame&) override {
        std::unique_lock<std::mutex> lock(mutex_);
        entered_ = true;
        condition_.notify_all();
        condition_.wait(lock, [this] { return released_; });
        ++completed_;
    }
    void onAudioFrame(const std::string&, const AudioFrame&) override {}
    void onPeerConnectionStateChange(const std::string&, PeerConnectionState) override {}
    void onError(const std::string&, const std::string&) override {}
    bool waitUntilEntered() {
        std::unique_lock<std::mutex> lock(mutex_);
        return condition_.wait_for(lock, std::chrono::seconds(1), [this] { return entered_; });
    }
    void release() {
        std::lock_guard<std::mutex> lock(mutex_);
        released_ = true;
        condition_.notify_all();
    }
    int completed() const { return completed_.load(); }
private:
    std::mutex mutex_;
    std::condition_variable condition_;
    bool entered_ = false;
    bool released_ = false;
    std::atomic<int> completed_{0};
};

struct SessionLoopbackFactory {
    std::mutex mutex;
    std::shared_ptr<PeerTransport> remote;
    PeerTransportFactory factory() {
        return [this](const std::string& session_id, const std::string& remote_peer_id) {
            const auto pair = createLoopbackPeerTransportPair(
                session_id, "local", remote_peer_id, 8);
            std::lock_guard<std::mutex> lock(mutex);
            remote = pair.second;
            return pair.first;
        };
    }
};

class ScopedBackendRegistration {
public:
    ScopedBackendRegistration(std::string name, LiveVideoChatBackendFactories factories)
        : name_(std::move(name)), registered_(
              LiveVideoChatBackendRegistry::instance().registerBackend(
                  name_, std::move(factories))) {}
    ~ScopedBackendRegistration() {
        if (registered_)
            LiveVideoChatBackendRegistry::instance().unregisterBackend(name_);
    }
    bool registered() const { return registered_; }
private:
    std::string name_;
    bool registered_;
};

} // namespace

TEST(LiveVideoChatComprehensive, DefaultObjectsRejectWithoutBackends) {
    VideoChatConfig config;
    WebRTCPeerConnection connection;
    EXPECT_FALSE(connection.initialize(config));
    EXPECT_TRUE(connection.createOffer().empty());
    EXPECT_FALSE(connection.sendVideoFrame(makeVideoFrame()));

    VideoChatSession session;
    ASSERT_TRUE(session.initialize(config));
    EXPECT_FALSE(session.startSession("peer"));
    EXPECT_TRUE(session.createOffer("peer").empty());

    LiveVideoChat chat;
    EXPECT_FALSE(chat.initialize(config));
    EXPECT_FALSE(chat.initialize(config, "missing-backend"));
    EXPECT_EQ(chat.createSession("session"), nullptr);
}

TEST(LiveVideoChatComprehensive, DeterministicCaptureIsExplicitAndRestartable) {
    const auto video = createDeterministicSyntheticVideoCapture();
    const auto audio = createDeterministicSyntheticAudioCapture();
    ASSERT_NE(video, nullptr);
    ASSERT_NE(audio, nullptr);
    EXPECT_TRUE(video->getNextFrame().data.empty());
    EXPECT_TRUE(audio->getNextFrame().samples.empty());
    EXPECT_FALSE(video->start());
    EXPECT_FALSE(audio->start());
    EXPECT_FALSE(video->setConfig(0, 2, 30));
    EXPECT_FALSE(audio->setConfig(7999, 2));
    ASSERT_TRUE(video->setConfig(4, 2, 25));
    ASSERT_TRUE(audio->setConfig(48000, 2));
    for (int cycle = 0; cycle < 3; ++cycle) {
        ASSERT_TRUE(video->start());
        ASSERT_TRUE(audio->start());
        EXPECT_FALSE(video->start());
        EXPECT_FALSE(audio->start());
        const auto first_video = video->getNextFrame();
        const auto second_video = video->getNextFrame();
        EXPECT_EQ(first_video.timestamp_ms, 0);
        EXPECT_EQ(second_video.timestamp_ms, 40);
        EXPECT_EQ(first_video.data.size(), 24U);
        const auto first_audio = audio->getNextFrame();
        EXPECT_EQ(first_audio.timestamp_ms, 0);
        EXPECT_EQ(first_audio.samples.size(), 960U);
        video->stop();
        audio->stop();
        EXPECT_TRUE(video->getNextFrame().data.empty());
        EXPECT_TRUE(audio->getNextFrame().samples.empty());
    }
}

TEST(LiveVideoChatComprehensive, LoopbackUsesCanonicalIdentityBoundSdp) {
    const auto pair = createLoopbackPeerTransportPair("session", "alice", "bob", 4);
    ASSERT_NE(pair.first, nullptr);
    ASSERT_NE(pair.second, nullptr);
    WebRTCPeerConnection alice(pair.first);
    WebRTCPeerConnection bob(pair.second);
    VideoChatConfig config;
    ASSERT_TRUE(alice.initialize(config));
    ASSERT_TRUE(bob.initialize(config));
    EXPECT_TRUE(bob.createAnswer("mock_offer_sdp").empty());
    EXPECT_FALSE(alice.setRemoteDescription("mock_answer"));
    EXPECT_FALSE(alice.addIceCandidate("candidate:not enough fields"));
    const std::string offer = alice.createOffer();
    ASSERT_FALSE(offer.empty());
    EXPECT_NE(offer.find("o=eliza-loopback session alice bob offer"), std::string::npos);
    EXPECT_TRUE(bob.createAnswer(
        "v=0\r\no=eliza-loopback other alice bob offer\r\n"
        "s=eliza-loopback\r\nt=0 0\r\nm=video 9 UDP/LOOPBACK 96\r\n").empty());
    const std::string answer = bob.createAnswer(offer);
    ASSERT_FALSE(answer.empty());
    EXPECT_FALSE(alice.setRemoteDescription(answer + "x"));
    ASSERT_TRUE(alice.setRemoteDescription(answer));
    EXPECT_TRUE(alice.addIceCandidate(kCandidate));
    EXPECT_TRUE(bob.addIceCandidate(kCandidate));
    EXPECT_EQ(alice.getState(), PeerConnectionState::Connected);
    EXPECT_EQ(bob.getState(), PeerConnectionState::Connected);
}

TEST(LiveVideoChatComprehensive, FrameValidationIsStrict) {
    const auto pair = createLoopbackPeerTransportPair("frames", "sender", "receiver", 4);
    WebRTCPeerConnection sender(pair.first);
    WebRTCPeerConnection receiver(pair.second);
    VideoChatConfig config;
    ASSERT_TRUE(sender.initialize(config));
    ASSERT_TRUE(receiver.initialize(config));
    ASSERT_TRUE(negotiate(sender, receiver));
    auto video = makeVideoFrame();
    video.stride = 5;
    EXPECT_FALSE(sender.sendVideoFrame(video));
    video = makeVideoFrame();
    video.format = "BGR24";
    EXPECT_FALSE(sender.sendVideoFrame(video));
    video = makeVideoFrame();
    video.data.pop_back();
    EXPECT_FALSE(sender.sendVideoFrame(video));
    video = makeVideoFrame();
    video.timestamp_ms = -1;
    EXPECT_FALSE(sender.sendVideoFrame(video));
    auto audio = makeAudioFrame();
    audio.samples.pop_back();
    EXPECT_FALSE(sender.sendAudioFrame(audio));
    audio = makeAudioFrame();
    audio.samples.front() = std::numeric_limits<float>::infinity();
    EXPECT_FALSE(sender.sendAudioFrame(audio));
    audio = makeAudioFrame();
    audio.samples.front() = 1.1F;
    EXPECT_FALSE(sender.sendAudioFrame(audio));
    EXPECT_TRUE(sender.sendVideoFrame(makeVideoFrame()));
    EXPECT_TRUE(sender.sendAudioFrame(makeAudioFrame()));
}

TEST(LiveVideoChatComprehensive, MediaIsAcknowledgedAndDeliveredOutsideLocks) {
    const auto pair = createLoopbackPeerTransportPair("media", "alice", "bob", 2);
    WebRTCPeerConnection alice(pair.first);
    WebRTCPeerConnection bob(pair.second);
    auto callbacks = std::make_shared<RecordingCallbacks>();
    bob.setCallbacks(callbacks);
    VideoChatConfig config;
    ASSERT_TRUE(alice.initialize(config));
    ASSERT_TRUE(bob.initialize(config));
    ASSERT_TRUE(negotiate(alice, bob));
    EXPECT_TRUE(alice.sendVideoFrame(makeVideoFrame(19)));
    EXPECT_TRUE(alice.sendAudioFrame(makeAudioFrame(0.5F)));
    ASSERT_TRUE(callbacks->waitForMedia(1, 1));
    EXPECT_EQ(callbacks->lastPeer(), "alice");
    EXPECT_EQ(pair.second->pendingMediaFrames(), 0U);
    EXPECT_EQ(pair.second->mediaQueueCapacity(), 2U);
}

TEST(LiveVideoChatComprehensive, DisabledMediaCannotBeSent) {
    const auto pair = createLoopbackPeerTransportPair("audio-only", "alice", "bob", 2);
    WebRTCPeerConnection alice(pair.first);
    WebRTCPeerConnection bob(pair.second);
    VideoChatConfig config;
    config.enable_video = false;
    ASSERT_TRUE(alice.initialize(config));
    ASSERT_TRUE(bob.initialize(config));
    ASSERT_TRUE(negotiate(alice, bob));
    EXPECT_FALSE(alice.sendVideoFrame(makeVideoFrame()));
    EXPECT_TRUE(alice.sendAudioFrame(makeAudioFrame()));
}

TEST(LiveVideoChatComprehensive, DisconnectAllowsPromptRenegotiation) {
    const auto pair = createLoopbackPeerTransportPair("restart", "alice", "bob", 2);
    WebRTCPeerConnection alice(pair.first);
    WebRTCPeerConnection bob(pair.second);
    VideoChatConfig config;
    ASSERT_TRUE(alice.initialize(config));
    ASSERT_TRUE(bob.initialize(config));
    ASSERT_TRUE(negotiate(alice, bob));
    alice.disconnect();
    EXPECT_EQ(alice.getState(), PeerConnectionState::Disconnected);
    EXPECT_EQ(bob.getState(), PeerConnectionState::Disconnected);
    EXPECT_FALSE(alice.sendVideoFrame(makeVideoFrame()));
    ASSERT_TRUE(negotiate(alice, bob));
    EXPECT_TRUE(alice.sendVideoFrame(makeVideoFrame()));
}

TEST(LiveVideoChatComprehensive, CloseWaitsForInflightCallbackAndStopsFutureCallbacks) {
    const auto pair = createLoopbackPeerTransportPair("close", "alice", "bob", 2);
    WebRTCPeerConnection alice(pair.first);
    WebRTCPeerConnection bob(pair.second);
    auto callbacks = std::make_shared<BlockingCallbacks>();
    bob.setCallbacks(callbacks);
    VideoChatConfig config;
    ASSERT_TRUE(alice.initialize(config));
    ASSERT_TRUE(bob.initialize(config));
    ASSERT_TRUE(negotiate(alice, bob));
    std::thread sender([&] { EXPECT_TRUE(alice.sendVideoFrame(makeVideoFrame())); });
    ASSERT_TRUE(callbacks->waitUntilEntered());
    std::atomic<bool> close_returned{false};
    std::thread closer([&] {
        bob.close();
        close_returned.store(true);
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_FALSE(close_returned.load());
    callbacks->release();
    sender.join();
    closer.join();
    EXPECT_TRUE(close_returned.load());
    EXPECT_EQ(callbacks->completed(), 1);
    EXPECT_FALSE(alice.sendVideoFrame(makeVideoFrame()));
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    EXPECT_EQ(callbacks->completed(), 1);
}

TEST(LiveVideoChatComprehensive, SessionUsesInjectedFactoryAndRejectsMocks) {
    SessionLoopbackFactory factory;
    VideoChatConfig config;
    VideoChatSession session;
    ASSERT_TRUE(session.initialize(config, factory.factory(), "session"));
    ASSERT_TRUE(session.startSession("bob"));
    const std::string offer = session.createOffer("bob");
    EXPECT_NE(offer.find("o=eliza-loopback session local bob offer"), std::string::npos);
    EXPECT_TRUE(session.handleOffer("missing", "mock_offer_sdp").empty());
    EXPECT_FALSE(session.handleAnswer("missing", "mock_answer"));
    EXPECT_FALSE(session.handleIceCandidate("missing", kCandidate));
    EXPECT_FALSE(session.sendVideoFrame("missing", makeVideoFrame()));
    EXPECT_FALSE(session.startSession("bad peer"));
    session.endSession();
    EXPECT_FALSE(session.startSession("bob"));
}

TEST(LiveVideoChatComprehensive, RegistryIsExplicitValidatedAndFresh) {
    auto& registry = LiveVideoChatBackendRegistry::instance();
    EXPECT_FALSE(registry.registerBackend("bad name", {}));
    EXPECT_FALSE(registry.registerBackend("missing-transport", {}));
    std::atomic<int> video_instances{0};
    std::atomic<int> audio_instances{0};
    LiveVideoChatBackendFactories factories;
    factories.video_capture_factory = [&] {
        ++video_instances;
        return createDeterministicSyntheticVideoCapture();
    };
    factories.audio_capture_factory = [&] {
        ++audio_instances;
        return createDeterministicSyntheticAudioCapture();
    };
    factories.peer_transport_factory = [](
        const std::string& session_id, const std::string& peer_id) {
        return createLoopbackPeerTransportPair(session_id, "local", peer_id, 4).first;
    };
    ScopedBackendRegistration registration("test-backend", factories);
    ASSERT_TRUE(registration.registered());
    EXPECT_TRUE(registry.contains("test-backend"));
    EXPECT_FALSE(registry.registerBackend("test-backend", factories));
    const auto backends = registry.registeredBackends();
    EXPECT_NE(std::find(backends.begin(), backends.end(), "test-backend"), backends.end());
    VideoChatConfig config;
    LiveVideoChat first;
    LiveVideoChat second;
    ASSERT_TRUE(first.initialize(config, "test-backend"));
    ASSERT_TRUE(second.initialize(config, "test-backend"));
    EXPECT_NE(first.getVideoCapture(), second.getVideoCapture());
    EXPECT_NE(first.getAudioCapture(), second.getAudioCapture());
    EXPECT_EQ(video_instances.load(), 2);
    EXPECT_EQ(audio_instances.load(), 2);
    EXPECT_NE(first.createSession("session-one"), nullptr);
    EXPECT_EQ(first.createSession("session-one"), nullptr);
    EXPECT_EQ(first.createSession("bad session"), nullptr);
}

TEST(LiveVideoChatComprehensive, RegistryRespectsDisabledCapture) {
    LiveVideoChatBackendFactories transport_only;
    transport_only.peer_transport_factory = [](
        const std::string& session_id, const std::string& peer_id) {
        return createLoopbackPeerTransportPair(session_id, "local", peer_id, 1).first;
    };
    ScopedBackendRegistration registration("transport-only", transport_only);
    ASSERT_TRUE(registration.registered());
    VideoChatConfig config;
    LiveVideoChat chat;
    EXPECT_FALSE(chat.initialize(config, "transport-only"));
    config.enable_video = false;
    config.enable_audio = false;
    EXPECT_TRUE(chat.initialize(config, "transport-only"));
    EXPECT_EQ(chat.getVideoCapture(), nullptr);
    EXPECT_EQ(chat.getAudioCapture(), nullptr);
}

TEST(LiveVideoChatComprehensive, RegistryFactoryExceptionsAndCaptureFailureReject) {
    LiveVideoChatBackendFactories throwing;
    throwing.video_capture_factory = []() -> std::shared_ptr<VideoCapture> {
        throw std::runtime_error("capture unavailable");
    };
    throwing.audio_capture_factory = [] {
        return createDeterministicSyntheticAudioCapture();
    };
    throwing.peer_transport_factory = [](
        const std::string& session_id, const std::string& peer_id) {
        return createLoopbackPeerTransportPair(session_id, "local", peer_id, 2).first;
    };
    ScopedBackendRegistration registration("throwing-capture", throwing);
    ASSERT_TRUE(registration.registered());
    LiveVideoChat chat;
    EXPECT_FALSE(chat.initialize(VideoChatConfig{}, "throwing-capture"));
    EXPECT_EQ(chat.getVideoCapture(), nullptr);
    EXPECT_EQ(chat.getAudioCapture(), nullptr);
}

TEST(LiveVideoChatComprehensive, ConfigurationAndIdentityValidationAreStrict) {
    LiveVideoChatDependencies dependencies;
    dependencies.video_capture = createDeterministicSyntheticVideoCapture();
    dependencies.audio_capture = createDeterministicSyntheticAudioCapture();
    dependencies.peer_transport_factory = [](
        const std::string& session_id, const std::string& peer_id) {
        return createLoopbackPeerTransportPair(session_id, "local", peer_id, 2).first;
    };
    VideoChatConfig config;
    LiveVideoChat chat;
    config.video_width = 0;
    EXPECT_FALSE(chat.initialize(config, dependencies));
    config = {};
    config.audio_sample_rate = 7999;
    EXPECT_FALSE(chat.initialize(config, dependencies));
    config = {};
    config.video_codec = "VP8\ninvalid";
    EXPECT_FALSE(chat.initialize(config, dependencies));
    config = {};
    config.stun_server = "https://not-stun";
    EXPECT_FALSE(chat.initialize(config, dependencies));
    config = {};
    config.turn_servers = {"turn:localhost:3478", "bad:localhost"};
    EXPECT_FALSE(chat.initialize(config, dependencies));
    EXPECT_EQ(createLoopbackPeerTransportPair("", "alice", "bob").first, nullptr);
    EXPECT_EQ(createLoopbackPeerTransportPair("bad session", "alice", "bob").first, nullptr);
    EXPECT_EQ(createLoopbackPeerTransportPair("session", "alice", "alice").first, nullptr);
    EXPECT_EQ(createLoopbackPeerTransportPair("session", "alice", "bob", 0).first, nullptr);
    EXPECT_EQ(createLoopbackPeerTransportPair("session", "alice", "bob", 4097).first, nullptr);
}

TEST(LiveVideoChatComprehensive, LoopbackLifecycleStressSixtyCycles) {
    const auto started = std::chrono::steady_clock::now();
    for (int cycle = 0; cycle < 60; ++cycle) {
        const auto pair = createLoopbackPeerTransportPair(
            "stress-" + std::to_string(cycle), "alice", "bob", 2);
        ASSERT_NE(pair.first, nullptr);
        ASSERT_NE(pair.second, nullptr);
        WebRTCPeerConnection alice(pair.first);
        WebRTCPeerConnection bob(pair.second);
        auto callbacks = std::make_shared<RecordingCallbacks>();
        bob.setCallbacks(callbacks);
        VideoChatConfig config;
        ASSERT_TRUE(alice.initialize(config));
        ASSERT_TRUE(bob.initialize(config));
        ASSERT_TRUE(negotiate(alice, bob));
        ASSERT_TRUE(alice.sendVideoFrame(makeVideoFrame(
            static_cast<std::uint8_t>(cycle))));
        ASSERT_TRUE(callbacks->waitForMedia(1, 0));
        alice.disconnect();
        ASSERT_TRUE(negotiate(alice, bob));
        ASSERT_TRUE(alice.sendAudioFrame(makeAudioFrame()));
        ASSERT_TRUE(callbacks->waitForMedia(1, 1));
        alice.close();
        bob.close();
        EXPECT_FALSE(alice.sendVideoFrame(makeVideoFrame()));
    }
    EXPECT_LT(std::chrono::steady_clock::now() - started, std::chrono::seconds(10));
}
