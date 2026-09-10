#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace elizaos {

/** A decoded video frame. Supported formats are RGB24, RGBA, and I420. */
struct VideoFrame {
    std::vector<std::uint8_t> data;
    int width = 0;
    int height = 0;
    int stride = 0;
    std::string format;
    std::int64_t timestamp_ms = -1;
};

/** Interleaved floating-point PCM samples. */
struct AudioFrame {
    std::vector<float> samples;
    int sample_rate = 0;
    int channels = 0;
    std::int64_t timestamp_ms = -1;
};

struct VideoChatConfig {
    int video_width = 640;
    int video_height = 480;
    int video_fps = 30;
    int video_bitrate = 1000000;

    int audio_sample_rate = 48000;
    int audio_channels = 2;
    int audio_bitrate = 128000;

    bool enable_video = true;
    bool enable_audio = true;

    std::string stun_server = "stun:stun.l.google.com:19302";
    std::vector<std::string> turn_servers;

    std::string video_codec = "VP8";
    std::string audio_codec = "OPUS";
};

enum class PeerConnectionState {
    New,
    Connecting,
    Connected,
    Disconnected,
    Failed,
    Closed
};

struct SessionInfo {
    std::string session_id;
    std::string peer_id;
    PeerConnectionState state = PeerConnectionState::New;
    std::string local_sdp;
    std::string remote_sdp;
    std::int64_t start_time_ms = 0;
    std::int64_t last_activity_ms = 0;
};

class MediaStreamCallbacks {
public:
    virtual ~MediaStreamCallbacks() = default;
    virtual void onVideoFrame(const std::string& peer_id, const VideoFrame& frame) = 0;
    virtual void onAudioFrame(const std::string& peer_id, const AudioFrame& frame) = 0;
    virtual void onPeerConnectionStateChange(
        const std::string& peer_id,
        PeerConnectionState state) = 0;
    virtual void onError(const std::string& peer_id, const std::string& error) = 0;
};

/** Injectable video source. Production callers must provide a real implementation. */
class VideoCapture {
public:
    virtual ~VideoCapture() = default;
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual VideoFrame getNextFrame() = 0;
    virtual bool setConfig(int width, int height, int fps) = 0;
    virtual bool isActive() const = 0;
};

/** Injectable audio source. Production callers must provide a real implementation. */
class AudioCapture {
public:
    virtual ~AudioCapture() = default;
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual AudioFrame getNextFrame() = 0;
    virtual bool setConfig(int sample_rate, int channels) = 0;
    virtual bool isActive() const = 0;
};

/**
 * Signaling seam used by a peer connection. Implementations own SDP and ICE
 * validation; returning success means that the adapter accepted the operation.
 */
class SignalingAdapter {
public:
    virtual ~SignalingAdapter() = default;
    virtual bool initialize(const VideoChatConfig& config) = 0;
    virtual std::string createOffer() = 0;
    virtual std::string createAnswer(const std::string& offer) = 0;
    virtual bool setRemoteDescription(const std::string& sdp) = 0;
    virtual bool setLocalDescription(const std::string& sdp) = 0;
    virtual bool addIceCandidate(const std::string& candidate) = 0;
    virtual PeerConnectionState getState() const = 0;
    virtual std::string getLocalDescription() const = 0;
    virtual std::string getRemoteDescription() const = 0;
    virtual void disconnect() = 0;
    virtual void close() = 0;
};

struct PeerTransportCallbacks {
    std::function<void(PeerConnectionState)> on_state_change;
    std::function<void(const VideoFrame&)> on_video_frame;
    std::function<void(const AudioFrame&)> on_audio_frame;
    std::function<void(const std::string&)> on_error;
};

/**
 * Injectable media transport plus signaling adapter. send* returns true only
 * after the adapter has acknowledged a bounded enqueue to a live peer.
 */
class PeerTransport : public SignalingAdapter {
public:
    ~PeerTransport() override = default;
    virtual std::string sessionId() const = 0;
    virtual std::string localPeerId() const = 0;
    virtual std::string remotePeerId() const = 0;
    virtual bool sendVideoFrame(const VideoFrame& frame) = 0;
    virtual bool sendAudioFrame(const AudioFrame& frame) = 0;
    virtual void setCallbacks(PeerTransportCallbacks callbacks) = 0;
    virtual std::size_t pendingMediaFrames() const = 0;
    virtual std::size_t mediaQueueCapacity() const = 0;
};

using PeerTransportFactory = std::function<std::shared_ptr<PeerTransport>(
    const std::string& session_id,
    const std::string& remote_peer_id)>;

using VideoCaptureFactory = std::function<std::shared_ptr<VideoCapture>()>;
using AudioCaptureFactory = std::function<std::shared_ptr<AudioCapture>()>;

struct LiveVideoChatDependencies {
    std::shared_ptr<VideoCapture> video_capture;
    std::shared_ptr<AudioCapture> audio_capture;
    PeerTransportFactory peer_transport_factory;
};

/** Factories registered for an explicitly selected production or test backend. */
struct LiveVideoChatBackendFactories {
    VideoCaptureFactory video_capture_factory;
    AudioCaptureFactory audio_capture_factory;
    PeerTransportFactory peer_transport_factory;
};

/**
 * Thread-safe process registry for named LiveVideoChat backends. Nothing is
 * registered implicitly. Duplicate names are rejected and resolving a backend
 * creates fresh capture instances for the requesting LiveVideoChat object.
 */
class LiveVideoChatBackendRegistry {
public:
    static LiveVideoChatBackendRegistry& instance();

    bool registerBackend(
        const std::string& backend_name,
        LiveVideoChatBackendFactories factories);
    bool unregisterBackend(const std::string& backend_name);
    bool contains(const std::string& backend_name) const;
    std::vector<std::string> registeredBackends() const;

    bool resolve(
        const std::string& backend_name,
        const VideoChatConfig& config,
        LiveVideoChatDependencies* dependencies) const;

private:
    LiveVideoChatBackendRegistry();
    ~LiveVideoChatBackendRegistry();
    LiveVideoChatBackendRegistry(const LiveVideoChatBackendRegistry&) = delete;
    LiveVideoChatBackendRegistry& operator=(const LiveVideoChatBackendRegistry&) = delete;

    class Impl;
    std::unique_ptr<Impl> impl_;
};

/**
 * Explicit deterministic test sources. They never start implicitly and return
 * empty frames while inactive. Production initialization never selects them.
 */
std::shared_ptr<VideoCapture> createDeterministicSyntheticVideoCapture();
std::shared_ptr<AudioCapture> createDeterministicSyntheticAudioCapture();

struct LoopbackPeerTransportPair {
    std::shared_ptr<PeerTransport> first;
    std::shared_ptr<PeerTransport> second;
};

/**
 * Creates two truthful in-process endpoints. The identifiers and session ID
 * are encoded into, and validated against, generated SDP. A zero or excessive
 * queue capacity, empty identifier, or identical peer identifiers is rejected.
 */
LoopbackPeerTransportPair createLoopbackPeerTransportPair(
    const std::string& session_id,
    const std::string& first_peer_id,
    const std::string& second_peer_id,
    std::size_t media_queue_capacity = 8);

class WebRTCPeerConnection {
public:
    WebRTCPeerConnection();
    explicit WebRTCPeerConnection(std::shared_ptr<PeerTransport> transport);
    ~WebRTCPeerConnection();

    WebRTCPeerConnection(const WebRTCPeerConnection&) = delete;
    WebRTCPeerConnection& operator=(const WebRTCPeerConnection&) = delete;

    bool initialize(const VideoChatConfig& config);
    std::string createOffer();
    std::string createAnswer(const std::string& offer);
    bool setRemoteDescription(const std::string& sdp);
    bool setLocalDescription(const std::string& sdp);
    bool addIceCandidate(const std::string& candidate);
    PeerConnectionState getState() const;
    std::string getLocalDescription() const;
    std::string getRemoteDescription() const;
    bool sendVideoFrame(const VideoFrame& frame);
    bool sendAudioFrame(const AudioFrame& frame);
    void setCallbacks(std::shared_ptr<MediaStreamCallbacks> callbacks);
    void disconnect();
    void close();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

class VideoChatSession {
public:
    VideoChatSession();
    ~VideoChatSession();

    VideoChatSession(const VideoChatSession&) = delete;
    VideoChatSession& operator=(const VideoChatSession&) = delete;

    /** Initializes a manual-signaling session without fabricating a transport. */
    bool initialize(const VideoChatConfig& config);

    /** Initializes a session with an explicit transport/signaling factory. */
    bool initialize(
        const VideoChatConfig& config,
        PeerTransportFactory peer_transport_factory,
        const std::string& session_id = {});

    bool startSession(const std::string& peer_id);
    bool joinSession(const std::string& session_id, const std::string& peer_id);
    std::string createOffer(const std::string& peer_id);
    std::string handleOffer(const std::string& peer_id, const std::string& offer);
    bool handleAnswer(const std::string& peer_id, const std::string& answer);
    bool handleIceCandidate(const std::string& peer_id, const std::string& candidate);
    bool sendVideoFrame(const std::string& peer_id, const VideoFrame& frame);
    bool sendAudioFrame(const std::string& peer_id, const AudioFrame& frame);
    void disconnectPeer(const std::string& peer_id);
    void setCallbacks(std::shared_ptr<MediaStreamCallbacks> callbacks);
    void enableVideo(bool enabled);
    void enableAudio(bool enabled);
    SessionInfo getSessionInfo() const;
    std::vector<std::string> getConnectedPeers() const;
    void endSession();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

class LiveVideoChat {
public:
    LiveVideoChat();
    virtual ~LiveVideoChat();

    LiveVideoChat(const LiveVideoChat&) = delete;
    LiveVideoChat& operator=(const LiveVideoChat&) = delete;

    /**
     * Compatibility overload. It fails for the default enabled-media config
     * because this library has no built-in production capture or transport.
     */
    bool initialize(const VideoChatConfig& config = {});

    /** Initializes only with explicitly supplied, successfully configured dependencies. */
    bool initialize(
        const VideoChatConfig& config,
        const LiveVideoChatDependencies& dependencies);

    /** Resolves and initializes an explicitly named registered backend. */
    bool initialize(
        const VideoChatConfig& config,
        const std::string& backend_name);

    std::shared_ptr<VideoChatSession> createSession(const std::string& session_id);
    std::shared_ptr<VideoChatSession> getSession(const std::string& session_id);
    void removeSession(const std::string& session_id);
    std::vector<std::string> getActiveSessions() const;
    void setGlobalCallbacks(std::shared_ptr<MediaStreamCallbacks> callbacks);
    std::shared_ptr<VideoCapture> getVideoCapture();
    std::shared_ptr<AudioCapture> getAudioCapture();
    void shutdown();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace elizaos
