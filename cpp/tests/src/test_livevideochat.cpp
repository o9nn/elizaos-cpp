#include <gtest/gtest.h>
#include "elizaos/livevideochat.hpp"
#include "elizaos/agentlogger.hpp"
#include <thread>
#include <chrono>

using namespace elizaos;

class MockMediaStreamCallbacks : public MediaStreamCallbacks {
public:
    void onVideoFrame(const std::string& peer_id, const VideoFrame& frame) override {
        video_frames_received_++;
        last_peer_id_ = peer_id;
        last_video_frame_ = frame;
    }
    
    void onAudioFrame(const std::string& peer_id, const AudioFrame& frame) override {
        audio_frames_received_++;
        last_peer_id_ = peer_id;
        last_audio_frame_ = frame;
    }
    
    void onPeerConnectionStateChange(
        const std::string& peer_id,
        PeerConnectionState state
    ) override {
        state_changes_++;
        last_peer_id_ = peer_id;
        last_state_ = state;
    }
    
    void onError(const std::string& peer_id, const std::string& error) override {
        errors_++;
        last_peer_id_ = peer_id;
        last_error_ = error;
    }
    
    // Test helper methods
    int getVideoFramesReceived() const { return video_frames_received_; }
    int getAudioFramesReceived() const { return audio_frames_received_; }
    int getStateChanges() const { return state_changes_; }
    int getErrors() const { return errors_; }
    const std::string& getLastPeerId() const { return last_peer_id_; }
    PeerConnectionState getLastState() const { return last_state_; }
    const std::string& getLastError() const { return last_error_; }
    const VideoFrame& getLastVideoFrame() const { return last_video_frame_; }
    const AudioFrame& getLastAudioFrame() const { return last_audio_frame_; }
    
private:
    int video_frames_received_ = 0;
    int audio_frames_received_ = 0;
    int state_changes_ = 0;
    int errors_ = 0;
    std::string last_peer_id_;
    PeerConnectionState last_state_ = PeerConnectionState::New;
    std::string last_error_;
    VideoFrame last_video_frame_;
    AudioFrame last_audio_frame_;
};

class LiveVideoChatTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_.video_width = 640;
        config_.video_height = 480;
        config_.video_fps = 30;
        config_.video_bitrate = 1000000;
        config_.audio_sample_rate = 48000;
        config_.audio_channels = 2;
        config_.audio_bitrate = 128000;
        config_.enable_video = true;
        config_.enable_audio = true;
        config_.stun_server = "stun:stun.l.google.com:19302";
        config_.video_codec = "VP8";
        config_.audio_codec = "OPUS";
        
        callbacks_ = std::make_shared<MockMediaStreamCallbacks>();
    }
    
    VideoChatConfig config_;
    std::shared_ptr<MockMediaStreamCallbacks> callbacks_;
};

TEST_F(LiveVideoChatTest, VideoChatConfigTest) {
    EXPECT_EQ(config_.video_width, 640);
    EXPECT_EQ(config_.video_height, 480);
    EXPECT_EQ(config_.video_fps, 30);
    EXPECT_EQ(config_.audio_sample_rate, 48000);
    EXPECT_EQ(config_.audio_channels, 2);
    EXPECT_TRUE(config_.enable_video);
    EXPECT_TRUE(config_.enable_audio);
    EXPECT_EQ(config_.video_codec, "VP8");
    EXPECT_EQ(config_.audio_codec, "OPUS");
}

TEST_F(LiveVideoChatTest, VideoFrameTest) {
    VideoFrame frame;
    frame.width = 640;
    frame.height = 480;
    frame.stride = 640 * 3;
    frame.format = "RGB24";
    frame.data.resize(640 * 480 * 3, 128);
    frame.timestamp_ms = 1000;
    
    EXPECT_EQ(frame.width, 640);
    EXPECT_EQ(frame.height, 480);
    EXPECT_EQ(frame.stride, 1920);
    EXPECT_EQ(frame.format, "RGB24");
    EXPECT_EQ(frame.data.size(), 921600);
    EXPECT_EQ(frame.timestamp_ms, 1000);
}

TEST_F(LiveVideoChatTest, AudioFrameTest) {
    AudioFrame frame;
    frame.sample_rate = 48000;
    frame.channels = 2;
    frame.samples.resize(480, 0.5f); // 10ms of audio
    frame.timestamp_ms = 1000;
    
    EXPECT_EQ(frame.sample_rate, 48000);
    EXPECT_EQ(frame.channels, 2);
    EXPECT_EQ(frame.samples.size(), 480);
    EXPECT_EQ(frame.timestamp_ms, 1000);
}

TEST_F(LiveVideoChatTest, VideoCaptureMockTest) {
    LiveVideoChat system;
    system.initialize(config_);
    
    std::shared_ptr<VideoCapture> capture = system.getVideoCapture();
    EXPECT_NE(capture, nullptr);
    
    EXPECT_FALSE(capture->isActive());
    EXPECT_TRUE(capture->start());
    EXPECT_TRUE(capture->isActive());
    
    auto frame = capture->getNextFrame();
    EXPECT_EQ(frame.width, 640);
    EXPECT_EQ(frame.height, 480);
    EXPECT_EQ(frame.format, "RGB24");
    EXPECT_GT(frame.data.size(), 0);
    
    capture->stop();
    EXPECT_FALSE(capture->isActive());
    
    system.shutdown();
}

TEST_F(LiveVideoChatTest, AudioCaptureMockTest) {
    LiveVideoChat system;
    system.initialize(config_);
    
    std::shared_ptr<AudioCapture> capture = system.getAudioCapture();
    EXPECT_NE(capture, nullptr);
    
    EXPECT_FALSE(capture->isActive());
    EXPECT_TRUE(capture->start());
    EXPECT_TRUE(capture->isActive());
    
    auto frame = capture->getNextFrame();
    EXPECT_EQ(frame.sample_rate, 48000);
    EXPECT_EQ(frame.channels, 2);
    EXPECT_GT(frame.samples.size(), 0);
    
    capture->stop();
    EXPECT_FALSE(capture->isActive());
    
    system.shutdown();
}

TEST_F(LiveVideoChatTest, VideoCaptureConfigTest) {
    LiveVideoChat system;
    
    VideoChatConfig custom_config = config_;
    custom_config.video_width = 1280;
    custom_config.video_height = 720;
    custom_config.video_fps = 60;
    
    system.initialize(custom_config);
    
    std::shared_ptr<VideoCapture> capture = system.getVideoCapture();
    EXPECT_NE(capture, nullptr);
    
    EXPECT_TRUE(capture->start());
    
    auto frame = capture->getNextFrame();
    EXPECT_EQ(frame.width, 1280);
    EXPECT_EQ(frame.height, 720);
    
    capture->stop();
    system.shutdown();
}

TEST_F(LiveVideoChatTest, AudioCaptureConfigTest) {
    LiveVideoChat system;
    
    VideoChatConfig custom_config = config_;
    custom_config.audio_sample_rate = 44100;
    custom_config.audio_channels = 1;
    
    system.initialize(custom_config);
    
    std::shared_ptr<AudioCapture> capture = system.getAudioCapture();
    EXPECT_NE(capture, nullptr);
    
    EXPECT_TRUE(capture->start());
    
    auto frame = capture->getNextFrame();
    EXPECT_EQ(frame.sample_rate, 44100);
    EXPECT_EQ(frame.channels, 1);
    
    capture->stop();
    system.shutdown();
}

TEST_F(LiveVideoChatTest, WebRTCPeerConnectionTest) {
    WebRTCPeerConnection peer_conn;
    
    EXPECT_TRUE(peer_conn.initialize(config_));
    EXPECT_EQ(peer_conn.getState(), PeerConnectionState::New);
    
    // Test offer/answer flow
    std::string offer = peer_conn.createOffer();
    EXPECT_GT(offer.length(), 0);
    EXPECT_EQ(peer_conn.getState(), PeerConnectionState::Connecting);
    
    // Create another peer connection to test answer
    WebRTCPeerConnection peer_conn2;
    EXPECT_TRUE(peer_conn2.initialize(config_));
    
    std::string answer = peer_conn2.createAnswer(offer);
    EXPECT_GT(answer.length(), 0);
    
    // Set remote descriptions
    EXPECT_TRUE(peer_conn.setRemoteDescription(answer));
    EXPECT_TRUE(peer_conn2.setRemoteDescription(offer));
    
    EXPECT_EQ(peer_conn.getState(), PeerConnectionState::Connected);
    EXPECT_EQ(peer_conn2.getState(), PeerConnectionState::Connected);
    
    // Test ICE candidates
    std::string candidate = "candidate:1 1 UDP 2130706431 192.168.1.100 54400 typ host";
    EXPECT_TRUE(peer_conn.addIceCandidate(candidate));
    
    peer_conn.close();
    peer_conn2.close();
    
    EXPECT_EQ(peer_conn.getState(), PeerConnectionState::Closed);
    EXPECT_EQ(peer_conn2.getState(), PeerConnectionState::Closed);
}

TEST_F(LiveVideoChatTest, WebRTCMediaTest) {
    WebRTCPeerConnection peer_conn;
    peer_conn.initialize(config_);
    peer_conn.setCallbacks(callbacks_);
    
    // Test sending video frame
    VideoFrame video_frame;
    video_frame.width = 640;
    video_frame.height = 480;
    video_frame.data.resize(640 * 480 * 3, 128);
    video_frame.format = "RGB24";
    video_frame.timestamp_ms = 1000;
    
    EXPECT_TRUE(peer_conn.sendVideoFrame(video_frame));
    
    // Test sending audio frame
    AudioFrame audio_frame;
    audio_frame.sample_rate = 48000;
    audio_frame.channels = 2;
    audio_frame.samples.resize(480, 0.5f);
    audio_frame.timestamp_ms = 1000;
    
    EXPECT_TRUE(peer_conn.sendAudioFrame(audio_frame));
    
    peer_conn.close();
}

TEST_F(LiveVideoChatTest, VideoChatSessionTest) {
    VideoChatSession session;
    
    EXPECT_TRUE(session.initialize(config_));
    
    std::string peer_id = "peer_123";
    EXPECT_TRUE(session.startSession(peer_id));
    
    // Test session info
    auto info = session.getSessionInfo();
    EXPECT_GT(info.start_time_ms, 0);
    EXPECT_GT(info.last_activity_ms, 0);
    
    // Test enabling/disabling media
    session.enableVideo(false);
    session.enableAudio(false);
    
    session.enableVideo(true);
    session.enableAudio(true);
    
    // Test connected peers
    auto peers = session.getConnectedPeers();
    // Note: Mock implementation may not show connected peers until full WebRTC handshake
    
    session.endSession();
}

TEST_F(LiveVideoChatTest, VideoChatSessionOfferAnswerTest) {
    VideoChatSession session1;
    VideoChatSession session2;
    
    EXPECT_TRUE(session1.initialize(config_));
    EXPECT_TRUE(session2.initialize(config_));
    
    std::string peer1_id = "peer_1";
    std::string peer2_id = "peer_2";
    
    EXPECT_TRUE(session1.startSession(peer2_id));
    EXPECT_TRUE(session2.joinSession("session_123", peer1_id));
    
    // Mock WebRTC signaling
    std::string offer = "mock_offer_sdp";
    std::string answer = session2.handleOffer(peer1_id, offer);
    EXPECT_GT(answer.length(), 0);
    
    EXPECT_TRUE(session1.handleAnswer(peer2_id, answer));
    
    // Test ICE candidates
    std::string candidate = "candidate:1 1 UDP 2130706431 192.168.1.100 54400 typ host";
    EXPECT_TRUE(session1.handleIceCandidate(peer2_id, candidate));
    EXPECT_TRUE(session2.handleIceCandidate(peer1_id, candidate));
    
    session1.endSession();
    session2.endSession();
}

TEST_F(LiveVideoChatTest, VideoChatSessionCallbacksTest) {
    VideoChatSession session;
    session.initialize(config_);
    session.setCallbacks(callbacks_);
    
    std::string peer_id = "peer_123";
    session.startSession(peer_id);
    
    // Test that callbacks are std::set
    EXPECT_EQ(callbacks_->getVideoFramesReceived(), 0);
    EXPECT_EQ(callbacks_->getAudioFramesReceived(), 0);
    EXPECT_EQ(callbacks_->getStateChanges(), 0);
    EXPECT_EQ(callbacks_->getErrors(), 0);
    
    session.endSession();
}

TEST_F(LiveVideoChatTest, LiveVideoChatSystemTest) {
    LiveVideoChat system;
    
    EXPECT_TRUE(system.initialize(config_));
    
    // Test creating sessions
    std::string session_id = "session_123";
    auto session = system.createSession(session_id);
    EXPECT_NE(session, nullptr);
    
    // Test getting session
    auto retrieved_session = system.getSession(session_id);
    EXPECT_EQ(session, retrieved_session);
    
    // Test session list
    auto sessions = system.getActiveSessions();
    // Note: Mock sessions might not show as active until peers are connected
    
    // Test global callbacks
    system.setGlobalCallbacks(callbacks_);
    
    // Test media capture
    auto video_capture = system.getVideoCapture();
    EXPECT_NE(video_capture, nullptr);
    
    auto audio_capture = system.getAudioCapture();
    EXPECT_NE(audio_capture, nullptr);
    
    // Test removing session
    system.removeSession(session_id);
    
    auto removed_session = system.getSession(session_id);
    EXPECT_EQ(removed_session, nullptr);
    
    system.shutdown();
}

TEST_F(LiveVideoChatTest, LiveVideoChatMultipleSessionsTest) {
    LiveVideoChat system;
    system.initialize(config_);
    
    // Create multiple sessions
    std::vector<std::string> session_ids = {"session_1", "session_2", "session_3"};
    std::vector<std::shared_ptr<VideoChatSession>> sessions;
    
    for (const auto& id : session_ids) {
        auto session = system.createSession(id);
        EXPECT_NE(session, nullptr);
        sessions.push_back(session);
    }
    
    // Test that all sessions are created
    for (const auto& id : session_ids) {
        auto session = system.getSession(id);
        EXPECT_NE(session, nullptr);
    }
    
    // Remove all sessions
    for (const auto& id : session_ids) {
        system.removeSession(id);
    }
    
    // Test that all sessions are removed
    for (const auto& id : session_ids) {
        auto session = system.getSession(id);
        EXPECT_EQ(session, nullptr);
    }
    
    system.shutdown();
}

TEST_F(LiveVideoChatTest, LiveVideoChatMediaCaptureTest) {
    LiveVideoChat system;
    system.initialize(config_);
    
    auto video_capture = system.getVideoCapture();
    auto audio_capture = system.getAudioCapture();
    
    EXPECT_NE(video_capture, nullptr);
    EXPECT_NE(audio_capture, nullptr);
    
    // Test video capture
    EXPECT_TRUE(video_capture->start());
    EXPECT_TRUE(video_capture->isActive());
    
    auto video_frame = video_capture->getNextFrame();
    EXPECT_EQ(video_frame.width, config_.video_width);
    EXPECT_EQ(video_frame.height, config_.video_height);
    EXPECT_GT(video_frame.data.size(), 0);
    
    video_capture->stop();
    EXPECT_FALSE(video_capture->isActive());
    
    // Test audio capture
    EXPECT_TRUE(audio_capture->start());
    EXPECT_TRUE(audio_capture->isActive());
    
    auto audio_frame = audio_capture->getNextFrame();
    EXPECT_EQ(audio_frame.sample_rate, config_.audio_sample_rate);
    EXPECT_EQ(audio_frame.channels, config_.audio_channels);
    EXPECT_GT(audio_frame.samples.size(), 0);
    
    audio_capture->stop();
    EXPECT_FALSE(audio_capture->isActive());
    
    system.shutdown();
}

TEST_F(LiveVideoChatTest, PeerConnectionStateTest) {
    WebRTCPeerConnection peer_conn;
    peer_conn.initialize(config_);
    
    EXPECT_EQ(peer_conn.getState(), PeerConnectionState::New);
    
    peer_conn.createOffer();
    EXPECT_EQ(peer_conn.getState(), PeerConnectionState::Connecting);
    
    peer_conn.setRemoteDescription("mock_answer");
    EXPECT_EQ(peer_conn.getState(), PeerConnectionState::Connected);
    
    peer_conn.close();
    EXPECT_EQ(peer_conn.getState(), PeerConnectionState::Closed);
}

TEST_F(LiveVideoChatTest, SessionInfoTest) {
    VideoChatSession session;
    session.initialize(config_);
    
    auto info = session.getSessionInfo();
    EXPECT_GT(info.start_time_ms, 0);
    EXPECT_GT(info.last_activity_ms, 0);
    EXPECT_EQ(info.state, PeerConnectionState::New);
    
    session.startSession("peer_123");
    
    info = session.getSessionInfo();
    EXPECT_EQ(info.peer_id, "peer_123");
    
    session.endSession();
}


TEST_F(LiveVideoChatTest, VideoCaptureIsDeterministicBeforeStart) {
    LiveVideoChat system;
    system.initialize(config_);

    const auto capture = system.getVideoCapture();
    ASSERT_NE(capture, nullptr);
    EXPECT_FALSE(capture->isActive());

    const auto frame = capture->getNextFrame();
    ASSERT_FALSE(frame.data.empty());
    EXPECT_EQ(frame.data.front(), 0u);
    EXPECT_GE(frame.timestamp_ms, 0);

    system.shutdown();
}

TEST_F(LiveVideoChatTest, AudioCaptureIsDeterministicBeforeStart) {
    LiveVideoChat system;
    system.initialize(config_);

    const auto capture = system.getAudioCapture();
    ASSERT_NE(capture, nullptr);
    EXPECT_FALSE(capture->isActive());

    const auto frame = capture->getNextFrame();
    ASSERT_FALSE(frame.samples.empty());
    EXPECT_FLOAT_EQ(frame.samples.front(), 0.0f);
    EXPECT_GE(frame.timestamp_ms, 0);

    system.shutdown();
}
