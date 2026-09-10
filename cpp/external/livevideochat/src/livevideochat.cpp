#include "elizaos/livevideochat.hpp"

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <deque>
#include <limits>
#include <map>
#include <mutex>
#include <set>
#include <sstream>
#include <thread>
#include <utility>

namespace elizaos {
namespace {

constexpr int kMaxDimension = 8192;
constexpr int kMaxFps = 240;
constexpr int kMaxSampleRate = 384000;
constexpr int kMaxChannels = 8;
constexpr int kMaxBitrate = 1000000000;
constexpr std::size_t kMaxMediaQueueCapacity = 4096;
constexpr std::size_t kMaxSdpBytes = 16384;
constexpr std::size_t kMaxCandidateBytes = 2048;
constexpr std::size_t kMaxBackendNameBytes = 128;
constexpr std::size_t kMaxServerUriBytes = 2048;
constexpr std::size_t kMaxTurnServers = 32;

std::int64_t nowMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

bool checkedProduct(std::size_t first, std::size_t second, std::size_t* result) {
    if (first != 0 && second > std::numeric_limits<std::size_t>::max() / first) {
        return false;
    }
    *result = first * second;
    return true;
}

bool validIdentifier(const std::string& value) {
    if (value.empty() || value.size() > 128) return false;
    return std::all_of(value.begin(), value.end(), [](unsigned char ch) {
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
               (ch >= '0' && ch <= '9') || ch == '.' || ch == '_' || ch == '-';
    });
}

bool validBackendName(const std::string& value) {
    return value.size() <= kMaxBackendNameBytes && validIdentifier(value);
}

bool validCodec(const std::string& value) {
    if (value.empty() || value.size() > 32) return false;
    return std::all_of(value.begin(), value.end(), [](unsigned char ch) {
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
               (ch >= '0' && ch <= '9') || ch == '-' || ch == '_';
    });
}

bool validServerUri(const std::string& value, bool turn) {
    if (value.empty()) return true;
    if (value.size() > kMaxServerUriBytes ||
        value.find_first_of("\r\n\t ") != std::string::npos) return false;
    if (turn) return value.rfind("turn:", 0) == 0 || value.rfind("turns:", 0) == 0;
    return value.rfind("stun:", 0) == 0 || value.rfind("stuns:", 0) == 0;
}

bool validConfig(const VideoChatConfig& config) {
    if (config.video_width <= 0 || config.video_width > kMaxDimension ||
        config.video_height <= 0 || config.video_height > kMaxDimension ||
        config.video_fps <= 0 || config.video_fps > kMaxFps ||
        config.video_bitrate <= 0 || config.video_bitrate > kMaxBitrate ||
        config.audio_sample_rate < 8000 || config.audio_sample_rate > kMaxSampleRate ||
        config.audio_channels <= 0 || config.audio_channels > kMaxChannels ||
        config.audio_bitrate <= 0 || config.audio_bitrate > kMaxBitrate ||
        !validCodec(config.video_codec) || !validCodec(config.audio_codec) ||
        !validServerUri(config.stun_server, false) ||
        config.turn_servers.size() > kMaxTurnServers) return false;
    return std::all_of(config.turn_servers.begin(), config.turn_servers.end(),
                       [](const std::string& uri) {
                           return !uri.empty() && validServerUri(uri, true);
                       });
}

bool validVideoShape(int width, int height, int fps) {
    std::size_t pixels = 0;
    return width > 0 && width <= kMaxDimension && height > 0 &&
           height <= kMaxDimension && fps > 0 && fps <= kMaxFps &&
           checkedProduct(static_cast<std::size_t>(width),
                          static_cast<std::size_t>(height), &pixels) &&
           pixels <= static_cast<std::size_t>(kMaxDimension) * kMaxDimension;
}

bool validAudioShape(int sample_rate, int channels) {
    return sample_rate >= 8000 && sample_rate <= kMaxSampleRate &&
           channels > 0 && channels <= kMaxChannels;
}

bool validVideoFrame(const VideoFrame& frame) {
    if (!validVideoShape(frame.width, frame.height, 1) || frame.timestamp_ms < 0 ||
        frame.stride <= 0) return false;
    std::size_t expected = 0;
    if (frame.format == "RGB24" || frame.format == "RGBA") {
        const int bytes_per_pixel = frame.format == "RGB24" ? 3 : 4;
        if (frame.width > std::numeric_limits<int>::max() / bytes_per_pixel ||
            frame.stride != frame.width * bytes_per_pixel ||
            !checkedProduct(static_cast<std::size_t>(frame.stride),
                            static_cast<std::size_t>(frame.height), &expected)) return false;
    } else if (frame.format == "I420") {
        if ((frame.width % 2) != 0 || (frame.height % 2) != 0 ||
            frame.stride != frame.width) return false;
        std::size_t pixels = 0;
        if (!checkedProduct(static_cast<std::size_t>(frame.width),
                            static_cast<std::size_t>(frame.height), &pixels) ||
            pixels > std::numeric_limits<std::size_t>::max() / 3) return false;
        expected = pixels * 3 / 2;
    } else {
        return false;
    }
    return frame.data.size() == expected;
}

bool validAudioFrame(const AudioFrame& frame) {
    if (!validAudioShape(frame.sample_rate, frame.channels) || frame.timestamp_ms < 0 ||
        frame.samples.empty() ||
        frame.samples.size() % static_cast<std::size_t>(frame.channels) != 0) return false;
    return std::all_of(frame.samples.begin(), frame.samples.end(), [](float sample) {
        return std::isfinite(sample) && sample >= -1.0F && sample <= 1.0F;
    });
}

class DeterministicSyntheticVideoCapture final : public VideoCapture {
public:
    bool start() override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!configured_ || active_) return false;
        active_ = true;
        frame_count_ = 0;
        return true;
    }
    void stop() override { std::lock_guard<std::mutex> lock(mutex_); active_ = false; }
    VideoFrame getNextFrame() override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!active_) return {};
        VideoFrame frame;
        frame.width = width_;
        frame.height = height_;
        frame.stride = width_ * 3;
        frame.format = "RGB24";
        frame.timestamp_ms = static_cast<std::int64_t>(frame_count_) * 1000 / fps_;
        std::size_t frame_size = 0;
        checkedProduct(static_cast<std::size_t>(frame.stride),
                       static_cast<std::size_t>(frame.height), &frame_size);
        frame.data.resize(frame_size);
        const std::uint8_t value = static_cast<std::uint8_t>(frame_count_ % 256);
        for (std::size_t index = 0; index < frame.data.size(); index += 3) {
            frame.data[index] = value;
            frame.data[index + 1] = static_cast<std::uint8_t>(255U - value);
            frame.data[index + 2] = static_cast<std::uint8_t>((value * 17U) % 256U);
        }
        ++frame_count_;
        return frame;
    }
    bool setConfig(int width, int height, int fps) override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (active_ || !validVideoShape(width, height, fps) ||
            width > std::numeric_limits<int>::max() / 3) return false;
        width_ = width; height_ = height; fps_ = fps; configured_ = true;
        return true;
    }
    bool isActive() const override { std::lock_guard<std::mutex> lock(mutex_); return active_; }
private:
    mutable std::mutex mutex_;
    bool configured_ = false;
    bool active_ = false;
    int width_ = 0;
    int height_ = 0;
    int fps_ = 0;
    std::uint64_t frame_count_ = 0;
};

class DeterministicSyntheticAudioCapture final : public AudioCapture {
public:
    bool start() override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!configured_ || active_) return false;
        active_ = true;
        frame_count_ = 0;
        return true;
    }
    void stop() override { std::lock_guard<std::mutex> lock(mutex_); active_ = false; }
    AudioFrame getNextFrame() override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!active_) return {};
        AudioFrame frame;
        frame.sample_rate = sample_rate_;
        frame.channels = channels_;
        frame.timestamp_ms = static_cast<std::int64_t>(frame_count_) * 10;
        const std::size_t per_channel = static_cast<std::size_t>(sample_rate_) / 100;
        std::size_t count = 0;
        checkedProduct(per_channel, static_cast<std::size_t>(channels_), &count);
        frame.samples.resize(count);
        for (std::size_t sample = 0; sample < per_channel; ++sample) {
            const float value = static_cast<float>((sample + frame_count_) % 101) / 500.0F - 0.1F;
            for (int channel = 0; channel < channels_; ++channel) {
                frame.samples[sample * static_cast<std::size_t>(channels_) +
                              static_cast<std::size_t>(channel)] = value;
            }
        }
        ++frame_count_;
        return frame;
    }
    bool setConfig(int sample_rate, int channels) override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (active_ || !validAudioShape(sample_rate, channels)) return false;
        sample_rate_ = sample_rate; channels_ = channels; configured_ = true;
        return true;
    }
    bool isActive() const override { std::lock_guard<std::mutex> lock(mutex_); return active_; }
private:
    mutable std::mutex mutex_;
    bool configured_ = false;
    bool active_ = false;
    int sample_rate_ = 0;
    int channels_ = 0;
    std::uint64_t frame_count_ = 0;
};

struct ParsedSdp {
    std::string session;
    std::string from;
    std::string to;
    std::string type;
    bool video = false;
    bool audio = false;
};

bool parseSdp(const std::string& sdp, ParsedSdp* parsed) {
    if (!parsed || sdp.empty() || sdp.size() > kMaxSdpBytes ||
        sdp.rfind("v=0\r\n", 0) != 0 || sdp.size() < 2 ||
        sdp.compare(sdp.size() - 2, 2, "\r\n") != 0) return false;
    ParsedSdp result;
    bool has_version = false;
    bool has_origin = false;
    bool has_session_name = false;
    bool has_timing = false;
    std::istringstream stream(sdp);
    std::string line;
    while (std::getline(stream, line)) {
        if (line.empty() || line.back() != '\r') return false;
        line.pop_back();
        if (line == "v=0") {
            if (has_version) return false;
            has_version = true;
        } else if (line == "s=eliza-loopback") {
            if (has_session_name) return false;
            has_session_name = true;
        } else if (line == "t=0 0") {
            if (has_timing) return false;
            has_timing = true;
        } else if (line.rfind("o=eliza-loopback ", 0) == 0) {
            if (has_origin) return false;
            std::istringstream origin(line.substr(17));
            std::string extra;
            if (!(origin >> result.session >> result.from >> result.to >> result.type) ||
                (origin >> extra)) return false;
            has_origin = true;
        } else if (line.rfind("m=video ", 0) == 0) {
            if (result.video) return false;
            result.video = true;
        } else if (line.rfind("m=audio ", 0) == 0) {
            if (result.audio) return false;
            result.audio = true;
        }
    }
    if (!has_version || !has_origin || !has_session_name || !has_timing ||
        !validIdentifier(result.session) || !validIdentifier(result.from) ||
        !validIdentifier(result.to) ||
        (result.type != "offer" && result.type != "answer") ||
        (!result.video && !result.audio)) return false;
    *parsed = std::move(result);
    return true;
}

std::string makeSdp(const VideoChatConfig& config, const std::string& session,
                    const std::string& from, const std::string& to,
                    const std::string& type) {
    std::ostringstream sdp;
    sdp << "v=0\r\n" << "o=eliza-loopback " << session << ' ' << from << ' '
        << to << ' ' << type << "\r\n" << "s=eliza-loopback\r\n" << "t=0 0\r\n";
    if (config.enable_video) {
        sdp << "m=video 9 UDP/LOOPBACK 96\r\n"
            << "a=rtpmap:96 " << config.video_codec << "/90000\r\n";
    }
    if (config.enable_audio) {
        sdp << "m=audio 9 UDP/LOOPBACK 111\r\n"
            << "a=rtpmap:111 " << config.audio_codec << '/'
            << config.audio_sample_rate << '/' << config.audio_channels << "\r\n";
    }
    return sdp.str();
}

bool parseUnsigned(const std::string& value, std::uint64_t maximum) {
    if (value.empty()) return false;
    std::uint64_t parsed = 0;
    const auto result = std::from_chars(value.data(), value.data() + value.size(), parsed);
    return result.ec == std::errc{} && result.ptr == value.data() + value.size() &&
           parsed <= maximum;
}

bool validCandidateToken(const std::string& token, bool allow_colon) {
    if (token.empty() || token.size() > 255) return false;
    return std::all_of(token.begin(), token.end(), [allow_colon](unsigned char ch) {
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
               (ch >= '0' && ch <= '9') || ch == '.' || ch == '-' || ch == '_' ||
               (allow_colon && ch == ':');
    });
}

bool validCandidate(const std::string& candidate) {
    if (candidate.size() < 12 || candidate.size() > kMaxCandidateBytes ||
        candidate.rfind("candidate:", 0) != 0 ||
        candidate.find_first_of("\r\n\t") != std::string::npos) return false;
    std::istringstream stream(candidate.substr(10));
    std::vector<std::string> fields;
    std::string field;
    while (stream >> field) fields.push_back(field);
    if (fields.size() < 8 || ((fields.size() - 8) % 2) != 0 ||
        !validCandidateToken(fields[0], false) ||
        !parseUnsigned(fields[1], 256) ||
        (fields[2] != "UDP" && fields[2] != "udp" &&
         fields[2] != "TCP" && fields[2] != "tcp") ||
        !parseUnsigned(fields[3], std::numeric_limits<std::uint32_t>::max()) ||
        !validCandidateToken(fields[4], true) ||
        !parseUnsigned(fields[5], 65535) || fields[5] == "0" ||
        fields[6] != "typ" ||
        (fields[7] != "host" && fields[7] != "srflx" &&
         fields[7] != "prflx" && fields[7] != "relay")) return false;
    for (std::size_t index = 8; index < fields.size(); ++index) {
        if (!validCandidateToken(fields[index], true)) return false;
    }
    return true;
}

} // namespace

std::shared_ptr<VideoCapture> createDeterministicSyntheticVideoCapture() {
    return std::make_shared<DeterministicSyntheticVideoCapture>();
}

std::shared_ptr<AudioCapture> createDeterministicSyntheticAudioCapture() {
    return std::make_shared<DeterministicSyntheticAudioCapture>();
}

namespace {

thread_local std::vector<const void*> g_active_callback_states;

bool isActiveCallbackFor(const void* state) {
    return std::find(g_active_callback_states.begin(),
                     g_active_callback_states.end(), state) !=
           g_active_callback_states.end();
}

class LoopbackPeerTransport final : public PeerTransport {
public:
    enum class MessageKind { Video, Audio };
    struct Message { MessageKind kind; VideoFrame video; AudioFrame audio; };

    struct State {
        State(std::string session_value, std::string local_value,
              std::string remote_value, std::size_t capacity_value)
            : session(std::move(session_value)), local_peer(std::move(local_value)),
              remote_peer(std::move(remote_value)), capacity(capacity_value) {}

        const std::string session;
        const std::string local_peer;
        const std::string remote_peer;
        const std::size_t capacity;
        mutable std::mutex mutex;
        std::condition_variable callbacks_finished;
        std::weak_ptr<State> peer;
        VideoChatConfig config;
        PeerTransportCallbacks callbacks;
        std::deque<Message> queue;
        PeerConnectionState connection_state = PeerConnectionState::New;
        bool initialized = false;
        std::string local_sdp;
        std::string remote_sdp;
        bool dispatching = false;
        std::size_t active_callbacks = 0;
    };

    LoopbackPeerTransport(std::string session, std::string local_peer,
                          std::string remote_peer, std::size_t capacity)
        : state_(std::make_shared<State>(std::move(session), std::move(local_peer),
                                         std::move(remote_peer), capacity)) {}
    ~LoopbackPeerTransport() override { close(); }

    void link(const std::shared_ptr<LoopbackPeerTransport>& peer) {
        std::lock_guard<std::mutex> lock(state_->mutex);
        state_->peer = peer->state_;
    }

    bool initialize(const VideoChatConfig& config) override {
        {
            std::lock_guard<std::mutex> lock(state_->mutex);
            if (!validConfig(config) || (!config.enable_video && !config.enable_audio) ||
                state_->connection_state == PeerConnectionState::Closed ||
                state_->connection_state == PeerConnectionState::Connecting ||
                state_->connection_state == PeerConnectionState::Connected) return false;
            state_->config = config;
            state_->initialized = true;
            state_->queue.clear();
            state_->connection_state = PeerConnectionState::New;
        }
        dispatchState(state_, PeerConnectionState::New);
        return true;
    }

    std::string createOffer() override {
        std::string offer;
        {
            std::lock_guard<std::mutex> lock(state_->mutex);
            if (!state_->initialized ||
                (state_->connection_state != PeerConnectionState::New &&
                 state_->connection_state != PeerConnectionState::Disconnected)) return {};
            offer = makeSdp(state_->config, state_->session, state_->local_peer,
                            state_->remote_peer, "offer");
            state_->local_sdp = offer;
            state_->remote_sdp.clear();
            state_->connection_state = PeerConnectionState::Connecting;
        }
        dispatchState(state_, PeerConnectionState::Connecting);
        return offer;
    }

    std::string createAnswer(const std::string& offer) override {
        ParsedSdp parsed;
        if (!parseSdp(offer, &parsed)) {
            dispatchError(state_, "invalid loopback SDP offer");
            return {};
        }
        std::string answer;
        {
            std::lock_guard<std::mutex> lock(state_->mutex);
            if (!state_->initialized ||
                (state_->connection_state != PeerConnectionState::New &&
                 state_->connection_state != PeerConnectionState::Disconnected) ||
                parsed.type != "offer" || parsed.session != state_->session ||
                parsed.from != state_->remote_peer || parsed.to != state_->local_peer ||
                offer != makeSdp(state_->config, state_->session, state_->remote_peer,
                                 state_->local_peer, "offer")) {
                return {};
            }
            state_->remote_sdp = offer;
            answer = makeSdp(state_->config, state_->session, state_->local_peer,
                             state_->remote_peer, "answer");
            state_->local_sdp = answer;
            state_->connection_state = PeerConnectionState::Connecting;
        }
        dispatchState(state_, PeerConnectionState::Connecting);
        maybeConnect(state_);
        return answer;
    }

    bool setRemoteDescription(const std::string& sdp) override {
        ParsedSdp parsed;
        if (!parseSdp(sdp, &parsed)) {
            dispatchError(state_, "invalid loopback remote description");
            return false;
        }
        {
            std::lock_guard<std::mutex> lock(state_->mutex);
            if (!state_->initialized || state_->connection_state == PeerConnectionState::Closed ||
                state_->connection_state == PeerConnectionState::Failed ||
                parsed.session != state_->session || parsed.from != state_->remote_peer ||
                parsed.to != state_->local_peer ||
                sdp != makeSdp(state_->config, state_->session, state_->remote_peer,
                               state_->local_peer, parsed.type)) return false;
            ParsedSdp local;
            if (!parseSdp(state_->local_sdp, &local) || local.type == parsed.type) return false;
            state_->remote_sdp = sdp;
        }
        maybeConnect(state_);
        return true;
    }

    bool setLocalDescription(const std::string& sdp) override {
        ParsedSdp parsed;
        if (!parseSdp(sdp, &parsed)) {
            dispatchError(state_, "invalid loopback local description");
            return false;
        }
        {
            std::lock_guard<std::mutex> lock(state_->mutex);
            if (!state_->initialized || state_->connection_state == PeerConnectionState::Closed ||
                state_->connection_state == PeerConnectionState::Failed ||
                parsed.session != state_->session || parsed.from != state_->local_peer ||
                parsed.to != state_->remote_peer ||
                sdp != makeSdp(state_->config, state_->session, state_->local_peer,
                               state_->remote_peer, parsed.type) ||
                (!state_->local_sdp.empty() && state_->local_sdp != sdp)) return false;
            state_->local_sdp = sdp;
            if (state_->connection_state == PeerConnectionState::New)
                state_->connection_state = PeerConnectionState::Connecting;
        }
        dispatchState(state_, PeerConnectionState::Connecting);
        maybeConnect(state_);
        return true;
    }

    bool addIceCandidate(const std::string& candidate) override {
        if (!validCandidate(candidate)) {
            dispatchError(state_, "invalid ICE candidate");
            return false;
        }
        std::lock_guard<std::mutex> lock(state_->mutex);
        return state_->initialized &&
               (state_->connection_state == PeerConnectionState::Connecting ||
                state_->connection_state == PeerConnectionState::Connected);
    }

    PeerConnectionState getState() const override {
        std::lock_guard<std::mutex> lock(state_->mutex);
        return state_->connection_state;
    }
    std::string getLocalDescription() const override {
        std::lock_guard<std::mutex> lock(state_->mutex);
        return state_->local_sdp;
    }
    std::string getRemoteDescription() const override {
        std::lock_guard<std::mutex> lock(state_->mutex);
        return state_->remote_sdp;
    }
    std::string sessionId() const override { return state_->session; }
    std::string localPeerId() const override { return state_->local_peer; }
    std::string remotePeerId() const override { return state_->remote_peer; }

    bool sendVideoFrame(const VideoFrame& frame) override {
        if (!validVideoFrame(frame)) {
            dispatchError(state_, "invalid video frame");
            return false;
        }
        return enqueueOnPeer(state_, Message{MessageKind::Video, frame, {}});
    }
    bool sendAudioFrame(const AudioFrame& frame) override {
        if (!validAudioFrame(frame)) {
            dispatchError(state_, "invalid audio frame");
            return false;
        }
        return enqueueOnPeer(state_, Message{MessageKind::Audio, {}, frame});
    }

    void setCallbacks(PeerTransportCallbacks callbacks) override {
        std::lock_guard<std::mutex> lock(state_->mutex);
        if (state_->connection_state != PeerConnectionState::Closed)
            state_->callbacks = std::move(callbacks);
    }
    std::size_t pendingMediaFrames() const override {
        std::lock_guard<std::mutex> lock(state_->mutex);
        return state_->queue.size();
    }
    std::size_t mediaQueueCapacity() const override { return state_->capacity; }

    void disconnect() override {
        std::shared_ptr<State> peer;
        bool changed = false;
        {
            std::lock_guard<std::mutex> lock(state_->mutex);
            if (state_->connection_state == PeerConnectionState::Connected ||
                state_->connection_state == PeerConnectionState::Connecting) {
                state_->connection_state = PeerConnectionState::Disconnected;
                state_->local_sdp.clear();
                state_->remote_sdp.clear();
                state_->queue.clear();
                changed = true;
                peer = state_->peer.lock();
            }
        }
        if (changed) dispatchState(state_, PeerConnectionState::Disconnected);
        if (peer) remoteDisconnected(peer);
    }

    void close() override {
        std::shared_ptr<State> peer;
        std::function<void(PeerConnectionState)> closed_callback;
        {
            std::unique_lock<std::mutex> lock(state_->mutex);
            if (state_->connection_state == PeerConnectionState::Closed) return;
            state_->connection_state = PeerConnectionState::Closed;
            state_->initialized = false;
            state_->queue.clear();
            state_->dispatching = false;
            peer = state_->peer.lock();
            state_->peer.reset();
            closed_callback = std::move(state_->callbacks.on_state_change);
            state_->callbacks = {};
            if (!isActiveCallbackFor(state_.get())) {
                state_->callbacks_finished.wait(lock, [this] {
                    return state_->active_callbacks == 0;
                });
            }
        }
        if (closed_callback) {
            try { closed_callback(PeerConnectionState::Closed); } catch (...) {}
        }
        if (peer) remoteDisconnected(peer);
    }

private:
    struct ActiveCallback {
        explicit ActiveCallback(const std::shared_ptr<State>& value) : state(value) {
            g_active_callback_states.push_back(state.get());
        }
        ~ActiveCallback() {
            g_active_callback_states.pop_back();
            std::lock_guard<std::mutex> lock(state->mutex);
            --state->active_callbacks;
            state->callbacks_finished.notify_all();
        }
        std::shared_ptr<State> state;
    };

    static bool enqueueOnPeer(const std::shared_ptr<State>& state, Message message) {
        std::shared_ptr<State> peer;
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            const bool enabled = message.kind == MessageKind::Video
                                     ? state->config.enable_video
                                     : state->config.enable_audio;
            if (state->connection_state != PeerConnectionState::Connected || !enabled)
                return false;
            peer = state->peer.lock();
        }
        return peer && accept(peer, std::move(message));
    }

    static bool accept(const std::shared_ptr<State>& state, Message message) {
        bool should_drain = false;
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            if (state->connection_state != PeerConnectionState::Connected ||
                state->queue.size() >= state->capacity) return false;
            state->queue.push_back(std::move(message));
            if (!state->dispatching) {
                state->dispatching = true;
                should_drain = true;
            }
        }
        if (should_drain) drain(state);
        return true;
    }

    static void drain(const std::shared_ptr<State>& state) {
        for (;;) {
            Message message;
            std::function<void()> callback;
            {
                std::lock_guard<std::mutex> lock(state->mutex);
                if (state->connection_state == PeerConnectionState::Closed ||
                    state->queue.empty()) {
                    state->queue.clear();
                    state->dispatching = false;
                    state->callbacks_finished.notify_all();
                    return;
                }
                message = std::move(state->queue.front());
                state->queue.pop_front();
                if (message.kind == MessageKind::Video && state->callbacks.on_video_frame) {
                    const auto cb = state->callbacks.on_video_frame;
                    const auto frame = std::move(message.video);
                    callback = [cb, frame] { cb(frame); };
                } else if (message.kind == MessageKind::Audio && state->callbacks.on_audio_frame) {
                    const auto cb = state->callbacks.on_audio_frame;
                    const auto frame = std::move(message.audio);
                    callback = [cb, frame] { cb(frame); };
                }
                if (callback) ++state->active_callbacks;
            }
            if (callback) {
                ActiveCallback active(state);
                try { callback(); } catch (...) {}
            }
        }
    }

    static void maybeConnect(const std::shared_ptr<State>& state) {
        std::shared_ptr<State> peer;
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            peer = state->peer.lock();
        }
        if (!peer) return;
        bool changed_here = false;
        bool changed_there = false;
        {
            std::scoped_lock lock(state->mutex, peer->mutex);
            if (!state->initialized || !peer->initialized ||
                state->connection_state == PeerConnectionState::Closed ||
                peer->connection_state == PeerConnectionState::Closed ||
                state->config.enable_video != peer->config.enable_video ||
                state->config.enable_audio != peer->config.enable_audio ||
                state->local_sdp.empty() || state->remote_sdp.empty() ||
                state->local_sdp != peer->remote_sdp ||
                state->remote_sdp != peer->local_sdp) return;
            ParsedSdp local;
            ParsedSdp remote;
            if (!parseSdp(state->local_sdp, &local) ||
                !parseSdp(state->remote_sdp, &remote) || local.type == remote.type) return;
            if (state->connection_state != PeerConnectionState::Connected) {
                state->connection_state = PeerConnectionState::Connected;
                changed_here = true;
            }
            if (peer->connection_state != PeerConnectionState::Connected) {
                peer->connection_state = PeerConnectionState::Connected;
                changed_there = true;
            }
        }
        if (changed_here) dispatchState(state, PeerConnectionState::Connected);
        if (changed_there) dispatchState(peer, PeerConnectionState::Connected);
    }

    static void remoteDisconnected(const std::shared_ptr<State>& state) {
        bool changed = false;
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            if (state->connection_state == PeerConnectionState::Connected ||
                state->connection_state == PeerConnectionState::Connecting) {
                state->connection_state = PeerConnectionState::Disconnected;
                state->local_sdp.clear();
                state->remote_sdp.clear();
                state->queue.clear();
                changed = true;
            }
        }
        if (changed) dispatchState(state, PeerConnectionState::Disconnected);
    }

    static void dispatchState(const std::shared_ptr<State>& state,
                              PeerConnectionState value) {
        std::function<void(PeerConnectionState)> callback;
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            if (state->connection_state == PeerConnectionState::Closed) return;
            callback = state->callbacks.on_state_change;
            if (callback) ++state->active_callbacks;
        }
        if (callback) {
            ActiveCallback active(state);
            try { callback(value); } catch (...) {}
        }
    }

    static void dispatchError(const std::shared_ptr<State>& state,
                              const std::string& error) {
        std::function<void(const std::string&)> callback;
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            if (state->connection_state == PeerConnectionState::Closed) return;
            callback = state->callbacks.on_error;
            if (callback) ++state->active_callbacks;
        }
        if (callback) {
            ActiveCallback active(state);
            try { callback(error); } catch (...) {}
        }
    }

    std::shared_ptr<State> state_;
};

} // namespace

LoopbackPeerTransportPair createLoopbackPeerTransportPair(
    const std::string& session_id, const std::string& first_peer_id,
    const std::string& second_peer_id, std::size_t media_queue_capacity) {
    if (!validIdentifier(session_id) || !validIdentifier(first_peer_id) ||
        !validIdentifier(second_peer_id) || first_peer_id == second_peer_id ||
        media_queue_capacity == 0 || media_queue_capacity > kMaxMediaQueueCapacity) return {};
    auto first = std::make_shared<LoopbackPeerTransport>(
        session_id, first_peer_id, second_peer_id, media_queue_capacity);
    auto second = std::make_shared<LoopbackPeerTransport>(
        session_id, second_peer_id, first_peer_id, media_queue_capacity);
    first->link(second);
    second->link(first);
    return {first, second};
}

class LiveVideoChatBackendRegistry::Impl {
public:
    mutable std::mutex mutex;
    std::map<std::string, LiveVideoChatBackendFactories> factories;
};

LiveVideoChatBackendRegistry::LiveVideoChatBackendRegistry()
    : impl_(std::make_unique<Impl>()) {}
LiveVideoChatBackendRegistry::~LiveVideoChatBackendRegistry() = default;

LiveVideoChatBackendRegistry& LiveVideoChatBackendRegistry::instance() {
    static LiveVideoChatBackendRegistry registry;
    return registry;
}

bool LiveVideoChatBackendRegistry::registerBackend(
    const std::string& backend_name, LiveVideoChatBackendFactories factories) {
    if (!validBackendName(backend_name) || !factories.peer_transport_factory) return false;
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->factories.emplace(backend_name, std::move(factories)).second;
}

bool LiveVideoChatBackendRegistry::unregisterBackend(const std::string& backend_name) {
    if (!validBackendName(backend_name)) return false;
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->factories.erase(backend_name) == 1;
}

bool LiveVideoChatBackendRegistry::contains(const std::string& backend_name) const {
    if (!validBackendName(backend_name)) return false;
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->factories.count(backend_name) != 0;
}

std::vector<std::string> LiveVideoChatBackendRegistry::registeredBackends() const {
    std::vector<std::string> result;
    std::lock_guard<std::mutex> lock(impl_->mutex);
    result.reserve(impl_->factories.size());
    for (const auto& entry : impl_->factories) result.push_back(entry.first);
    return result;
}

bool LiveVideoChatBackendRegistry::resolve(
    const std::string& backend_name, const VideoChatConfig& config,
    LiveVideoChatDependencies* dependencies) const {
    if (!dependencies || !validBackendName(backend_name) || !validConfig(config)) return false;
    LiveVideoChatBackendFactories factories;
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        const auto it = impl_->factories.find(backend_name);
        if (it == impl_->factories.end()) return false;
        factories = it->second;
    }
    LiveVideoChatDependencies result;
    try {
        if (config.enable_video) {
            if (!factories.video_capture_factory) return false;
            result.video_capture = factories.video_capture_factory();
            if (!result.video_capture) return false;
        }
        if (config.enable_audio) {
            if (!factories.audio_capture_factory) return false;
            result.audio_capture = factories.audio_capture_factory();
            if (!result.audio_capture) return false;
        }
    } catch (...) {
        return false;
    }
    result.peer_transport_factory = std::move(factories.peer_transport_factory);
    *dependencies = std::move(result);
    return true;
}

class WebRTCPeerConnection::Impl {
public:
    struct CallbackBridge {
        std::mutex mutex;
        std::condition_variable finished;
        std::shared_ptr<MediaStreamCallbacks> callbacks;
        bool closed = false;
        std::size_t active = 0;
    };

    explicit Impl(std::shared_ptr<PeerTransport> transport)
        : transport_(std::move(transport)), bridge_(std::make_shared<CallbackBridge>()) {}
    ~Impl() { close(); }

    bool initialize(const VideoChatConfig& config) {
        std::shared_ptr<PeerTransport> transport;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (closed_ || initialized_ || !validConfig(config) || !transport_) return false;
            transport = transport_;
        }
        installTransportCallbacks(transport, bridge_);
        bool initialized = false;
        try { initialized = transport->initialize(config); } catch (...) { initialized = false; }
        if (!initialized) transport->setCallbacks({});
        {
            std::lock_guard<std::mutex> lock(mutex_);
            initialized_ = initialized;
        }
        return initialized;
    }

    std::string createOffer() { auto transport = snapshot(); return transport ? transport->createOffer() : std::string{}; }
    std::string createAnswer(const std::string& offer) { auto transport = snapshot(); return transport ? transport->createAnswer(offer) : std::string{}; }
    bool setRemoteDescription(const std::string& sdp) { auto transport = snapshot(); return transport && transport->setRemoteDescription(sdp); }
    bool setLocalDescription(const std::string& sdp) { auto transport = snapshot(); return transport && transport->setLocalDescription(sdp); }
    bool addIceCandidate(const std::string& candidate) { auto transport = snapshot(); return transport && transport->addIceCandidate(candidate); }
    PeerConnectionState getState() const {
        std::shared_ptr<PeerTransport> transport;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (closed_) return PeerConnectionState::Closed;
            transport = transport_;
        }
        return transport ? transport->getState() : PeerConnectionState::New;
    }
    std::string getLocalDescription() const { auto transport = snapshot(); return transport ? transport->getLocalDescription() : std::string{}; }
    std::string getRemoteDescription() const { auto transport = snapshot(); return transport ? transport->getRemoteDescription() : std::string{}; }
    bool sendVideoFrame(const VideoFrame& frame) { auto transport = snapshot(); return transport && transport->sendVideoFrame(frame); }
    bool sendAudioFrame(const AudioFrame& frame) { auto transport = snapshot(); return transport && transport->sendAudioFrame(frame); }

    void setCallbacks(std::shared_ptr<MediaStreamCallbacks> callbacks) {
        std::lock_guard<std::mutex> lock(bridge_->mutex);
        if (!bridge_->closed) bridge_->callbacks = std::move(callbacks);
    }
    void disconnect() { auto transport = snapshot(); if (transport) transport->disconnect(); }

    void close() {
        std::shared_ptr<PeerTransport> transport;
        std::shared_ptr<MediaStreamCallbacks> callbacks;
        std::string peer_id;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (closed_) return;
            closed_ = true;
            transport = std::move(transport_);
            if (transport) peer_id = transport->remotePeerId();
        }
        {
            std::unique_lock<std::mutex> lock(bridge_->mutex);
            bridge_->closed = true;
            callbacks = std::move(bridge_->callbacks);
            if (!isActiveCallbackFor(bridge_.get())) {
                bridge_->finished.wait(lock, [this] { return bridge_->active == 0; });
            }
        }
        if (transport) {
            transport->setCallbacks({});
            transport->close();
        }
        if (callbacks) {
            try {
                callbacks->onPeerConnectionStateChange(peer_id, PeerConnectionState::Closed);
            } catch (...) {}
        }
    }

private:
    struct ActiveBridgeCallback {
        explicit ActiveBridgeCallback(const std::shared_ptr<CallbackBridge>& value)
            : bridge(value) {
            g_active_callback_states.push_back(bridge.get());
        }
        ~ActiveBridgeCallback() {
            g_active_callback_states.pop_back();
            std::lock_guard<std::mutex> lock(bridge->mutex);
            --bridge->active;
            bridge->finished.notify_all();
        }
        std::shared_ptr<CallbackBridge> bridge;
    };

    std::shared_ptr<PeerTransport> snapshot() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (closed_ || !initialized_) return {};
        return transport_;
    }

    template <typename Function>
    static void invoke(const std::shared_ptr<CallbackBridge>& bridge, Function function) {
        std::shared_ptr<MediaStreamCallbacks> callbacks;
        {
            std::lock_guard<std::mutex> lock(bridge->mutex);
            if (bridge->closed) return;
            callbacks = bridge->callbacks;
            if (callbacks) ++bridge->active;
        }
        if (callbacks) {
            ActiveBridgeCallback active(bridge);
            try { function(*callbacks); } catch (...) {}
        }
    }

    static void installTransportCallbacks(
        const std::shared_ptr<PeerTransport>& transport,
        const std::shared_ptr<CallbackBridge>& bridge) {
        const std::string peer_id = transport->remotePeerId();
        transport->setCallbacks({
            [bridge, peer_id](PeerConnectionState state) {
                invoke(bridge, [&](MediaStreamCallbacks& callback) {
                    callback.onPeerConnectionStateChange(peer_id, state);
                });
            },
            [bridge, peer_id](const VideoFrame& frame) {
                invoke(bridge, [&](MediaStreamCallbacks& callback) {
                    callback.onVideoFrame(peer_id, frame);
                });
            },
            [bridge, peer_id](const AudioFrame& frame) {
                invoke(bridge, [&](MediaStreamCallbacks& callback) {
                    callback.onAudioFrame(peer_id, frame);
                });
            },
            [bridge, peer_id](const std::string& error) {
                invoke(bridge, [&](MediaStreamCallbacks& callback) {
                    callback.onError(peer_id, error);
                });
            }
        });
    }

    mutable std::mutex mutex_;
    std::shared_ptr<PeerTransport> transport_;
    std::shared_ptr<CallbackBridge> bridge_;
    bool initialized_ = false;
    bool closed_ = false;
};

WebRTCPeerConnection::WebRTCPeerConnection() : impl_(std::make_unique<Impl>(nullptr)) {}
WebRTCPeerConnection::WebRTCPeerConnection(std::shared_ptr<PeerTransport> transport)
    : impl_(std::make_unique<Impl>(std::move(transport))) {}
WebRTCPeerConnection::~WebRTCPeerConnection() = default;
bool WebRTCPeerConnection::initialize(const VideoChatConfig& config) { return impl_->initialize(config); }
std::string WebRTCPeerConnection::createOffer() { return impl_->createOffer(); }
std::string WebRTCPeerConnection::createAnswer(const std::string& offer) { return impl_->createAnswer(offer); }
bool WebRTCPeerConnection::setRemoteDescription(const std::string& sdp) { return impl_->setRemoteDescription(sdp); }
bool WebRTCPeerConnection::setLocalDescription(const std::string& sdp) { return impl_->setLocalDescription(sdp); }
bool WebRTCPeerConnection::addIceCandidate(const std::string& candidate) { return impl_->addIceCandidate(candidate); }
PeerConnectionState WebRTCPeerConnection::getState() const { return impl_->getState(); }
std::string WebRTCPeerConnection::getLocalDescription() const { return impl_->getLocalDescription(); }
std::string WebRTCPeerConnection::getRemoteDescription() const { return impl_->getRemoteDescription(); }
bool WebRTCPeerConnection::sendVideoFrame(const VideoFrame& frame) { return impl_->sendVideoFrame(frame); }
bool WebRTCPeerConnection::sendAudioFrame(const AudioFrame& frame) { return impl_->sendAudioFrame(frame); }
void WebRTCPeerConnection::setCallbacks(std::shared_ptr<MediaStreamCallbacks> callbacks) { impl_->setCallbacks(std::move(callbacks)); }
void WebRTCPeerConnection::disconnect() { impl_->disconnect(); }
void WebRTCPeerConnection::close() { impl_->close(); }

class VideoChatSession::Impl {
public:
    bool initialize(const VideoChatConfig& config, PeerTransportFactory factory,
                    const std::string& session_id) {
        if (!validConfig(config) || (!session_id.empty() && !validIdentifier(session_id))) return false;
        std::lock_guard<std::mutex> lock(mutex_);
        if (initialized_ && !ended_) return false;
        config_ = config;
        factory_ = std::move(factory);
        session_id_ = session_id;
        connections_.clear();
        start_time_ms_ = nowMs();
        last_activity_ms_ = start_time_ms_;
        initialized_ = true;
        ended_ = false;
        return true;
    }

    bool start(const std::string& session_id, const std::string& peer_id) {
        if (!validIdentifier(peer_id)) return false;
        PeerTransportFactory factory;
        VideoChatConfig config;
        std::shared_ptr<MediaStreamCallbacks> callbacks;
        std::shared_ptr<WebRTCPeerConnection> old;
        std::string effective_session;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!initialized_ || ended_ || !factory_) return false;
            if (!session_id.empty()) {
                if (!validIdentifier(session_id) || (!session_id_.empty() && session_id_ != session_id)) return false;
                session_id_ = session_id;
            }
            if (session_id_.empty()) return false;
            effective_session = session_id_;
            factory = factory_;
            config = config_;
            callbacks = callbacks_;
            auto existing = connections_.find(peer_id);
            if (existing != connections_.end()) old = existing->second;
        }
        if (old) old->close();
        std::shared_ptr<PeerTransport> transport;
        try { transport = factory(effective_session, peer_id); } catch (...) { return false; }
        if (!transport || transport->sessionId() != effective_session ||
            transport->remotePeerId() != peer_id ||
            !validIdentifier(transport->localPeerId()) ||
            transport->localPeerId() == peer_id) return false;
        auto connection = std::make_shared<WebRTCPeerConnection>(std::move(transport));
        connection->setCallbacks(callbacks);
        if (!connection->initialize(config)) return false;
        bool accepted = false;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!ended_) {
                connections_[peer_id] = connection;
                last_activity_ms_ = nowMs();
                accepted = true;
            }
        }
        if (!accepted) connection->close();
        return accepted;
    }

    std::shared_ptr<WebRTCPeerConnection> connection(const std::string& peer_id) const {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = connections_.find(peer_id);
        return it == connections_.end() ? nullptr : it->second;
    }
    void touch() { std::lock_guard<std::mutex> lock(mutex_); last_activity_ms_ = nowMs(); }

    void setCallbacks(std::shared_ptr<MediaStreamCallbacks> callbacks) {
        std::vector<std::shared_ptr<WebRTCPeerConnection>> connections;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            callbacks_ = callbacks;
            for (const auto& entry : connections_) connections.push_back(entry.second);
        }
        for (const auto& connection_ptr : connections) connection_ptr->setCallbacks(callbacks);
    }

    SessionInfo info() const {
        std::string session_id;
        std::int64_t start;
        std::int64_t activity;
        std::string peer_id;
        std::shared_ptr<WebRTCPeerConnection> peer;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            session_id = session_id_;
            start = start_time_ms_;
            activity = last_activity_ms_;
            if (!connections_.empty()) {
                peer_id = connections_.begin()->first;
                peer = connections_.begin()->second;
            }
        }
        SessionInfo result;
        result.session_id = std::move(session_id);
        result.start_time_ms = start;
        result.last_activity_ms = activity;
        result.peer_id = std::move(peer_id);
        if (peer) {
            result.state = peer->getState();
            result.local_sdp = peer->getLocalDescription();
            result.remote_sdp = peer->getRemoteDescription();
        }
        return result;
    }

    std::vector<std::string> connectedPeers() const {
        std::vector<std::pair<std::string, std::shared_ptr<WebRTCPeerConnection>>> snapshot;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (const auto& entry : connections_) snapshot.push_back(entry);
        }
        std::vector<std::string> result;
        for (const auto& entry : snapshot) {
            if (entry.second->getState() == PeerConnectionState::Connected) result.push_back(entry.first);
        }
        return result;
    }

    void end() {
        std::vector<std::shared_ptr<WebRTCPeerConnection>> connections;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (ended_) return;
            ended_ = true;
            for (const auto& entry : connections_) connections.push_back(entry.second);
            connections_.clear();
            last_activity_ms_ = nowMs();
        }
        for (const auto& connection_ptr : connections) connection_ptr->close();
    }

    mutable std::mutex mutex_;
    VideoChatConfig config_;
    PeerTransportFactory factory_;
    std::string session_id_;
    std::map<std::string, std::shared_ptr<WebRTCPeerConnection>> connections_;
    std::shared_ptr<MediaStreamCallbacks> callbacks_;
    std::int64_t start_time_ms_ = 0;
    std::int64_t last_activity_ms_ = 0;
    bool initialized_ = false;
    bool ended_ = false;
};

VideoChatSession::VideoChatSession() : impl_(std::make_unique<Impl>()) {}
VideoChatSession::~VideoChatSession() { impl_->end(); }
bool VideoChatSession::initialize(const VideoChatConfig& config) { return impl_->initialize(config, {}, {}); }
bool VideoChatSession::initialize(const VideoChatConfig& config, PeerTransportFactory factory,
                                  const std::string& session_id) {
    return impl_->initialize(config, std::move(factory), session_id);
}
bool VideoChatSession::startSession(const std::string& peer_id) { return impl_->start({}, peer_id); }
bool VideoChatSession::joinSession(const std::string& session_id, const std::string& peer_id) { return impl_->start(session_id, peer_id); }
std::string VideoChatSession::createOffer(const std::string& peer_id) {
    auto connection = impl_->connection(peer_id);
    if (!connection) return {};
    auto result = connection->createOffer();
    if (!result.empty()) impl_->touch();
    return result;
}
std::string VideoChatSession::handleOffer(const std::string& peer_id, const std::string& offer) {
    auto connection = impl_->connection(peer_id);
    if (!connection) return {};
    auto result = connection->createAnswer(offer);
    if (!result.empty()) impl_->touch();
    return result;
}
bool VideoChatSession::handleAnswer(const std::string& peer_id, const std::string& answer) {
    auto connection = impl_->connection(peer_id);
    const bool result = connection && connection->setRemoteDescription(answer);
    if (result) impl_->touch();
    return result;
}
bool VideoChatSession::handleIceCandidate(const std::string& peer_id, const std::string& candidate) {
    auto connection = impl_->connection(peer_id);
    const bool result = connection && connection->addIceCandidate(candidate);
    if (result) impl_->touch();
    return result;
}
bool VideoChatSession::sendVideoFrame(const std::string& peer_id, const VideoFrame& frame) {
    auto connection = impl_->connection(peer_id);
    const bool result = connection && connection->sendVideoFrame(frame);
    if (result) impl_->touch();
    return result;
}
bool VideoChatSession::sendAudioFrame(const std::string& peer_id, const AudioFrame& frame) {
    auto connection = impl_->connection(peer_id);
    const bool result = connection && connection->sendAudioFrame(frame);
    if (result) impl_->touch();
    return result;
}
void VideoChatSession::disconnectPeer(const std::string& peer_id) { auto connection = impl_->connection(peer_id); if (connection) connection->disconnect(); }
void VideoChatSession::setCallbacks(std::shared_ptr<MediaStreamCallbacks> callbacks) { impl_->setCallbacks(std::move(callbacks)); }
void VideoChatSession::enableVideo(bool enabled) { std::lock_guard<std::mutex> lock(impl_->mutex_); impl_->config_.enable_video = enabled; }
void VideoChatSession::enableAudio(bool enabled) { std::lock_guard<std::mutex> lock(impl_->mutex_); impl_->config_.enable_audio = enabled; }
SessionInfo VideoChatSession::getSessionInfo() const { return impl_->info(); }
std::vector<std::string> VideoChatSession::getConnectedPeers() const { return impl_->connectedPeers(); }
void VideoChatSession::endSession() { impl_->end(); }

class LiveVideoChat::Impl {
public:
    bool initialize(const VideoChatConfig& config, const LiveVideoChatDependencies& dependencies) {
        if (!validConfig(config)) return false;
        if (config.enable_video && !dependencies.video_capture) return false;
        if (config.enable_audio && !dependencies.audio_capture) return false;
        if (!dependencies.peer_transport_factory) return false;
        if (dependencies.video_capture &&
            !dependencies.video_capture->setConfig(config.video_width, config.video_height,
                                                    config.video_fps)) return false;
        if (dependencies.audio_capture &&
            !dependencies.audio_capture->setConfig(config.audio_sample_rate,
                                                    config.audio_channels)) return false;
        shutdown();
        std::lock_guard<std::mutex> lock(mutex_);
        config_ = config;
        dependencies_ = dependencies;
        initialized_ = true;
        return true;
    }

    std::shared_ptr<VideoChatSession> createSession(const std::string& session_id) {
        VideoChatConfig config;
        PeerTransportFactory factory;
        std::shared_ptr<MediaStreamCallbacks> callbacks;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!initialized_ || !validIdentifier(session_id) || sessions_.count(session_id) != 0) return {};
            config = config_;
            factory = dependencies_.peer_transport_factory;
            callbacks = global_callbacks_;
        }
        auto session = std::make_shared<VideoChatSession>();
        if (!session->initialize(config, std::move(factory), session_id)) return {};
        session->setCallbacks(callbacks);
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!initialized_ || sessions_.count(session_id) != 0) return {};
            sessions_.emplace(session_id, session);
        }
        return session;
    }

    std::shared_ptr<VideoChatSession> getSession(const std::string& session_id) const {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = sessions_.find(session_id);
        return it == sessions_.end() ? nullptr : it->second;
    }

    void removeSession(const std::string& session_id) {
        std::shared_ptr<VideoChatSession> removed;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            const auto it = sessions_.find(session_id);
            if (it == sessions_.end()) return;
            removed = std::move(it->second);
            sessions_.erase(it);
        }
        removed->endSession();
    }

    std::vector<std::string> activeSessions() const {
        std::vector<std::pair<std::string, std::shared_ptr<VideoChatSession>>> snapshot;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (const auto& entry : sessions_) snapshot.push_back(entry);
        }
        std::vector<std::string> result;
        for (const auto& entry : snapshot) {
            if (!entry.second->getConnectedPeers().empty()) result.push_back(entry.first);
        }
        return result;
    }

    void setCallbacks(std::shared_ptr<MediaStreamCallbacks> callbacks) {
        std::vector<std::shared_ptr<VideoChatSession>> sessions;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            global_callbacks_ = callbacks;
            for (const auto& entry : sessions_) sessions.push_back(entry.second);
        }
        for (const auto& session : sessions) session->setCallbacks(callbacks);
    }

    std::shared_ptr<VideoCapture> videoCapture() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return initialized_ ? dependencies_.video_capture : nullptr;
    }
    std::shared_ptr<AudioCapture> audioCapture() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return initialized_ ? dependencies_.audio_capture : nullptr;
    }

    void shutdown() {
        std::vector<std::shared_ptr<VideoChatSession>> sessions;
        std::shared_ptr<VideoCapture> video;
        std::shared_ptr<AudioCapture> audio;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (const auto& entry : sessions_) sessions.push_back(entry.second);
            sessions_.clear();
            video = dependencies_.video_capture;
            audio = dependencies_.audio_capture;
            dependencies_ = {};
            initialized_ = false;
        }
        for (const auto& session : sessions) session->endSession();
        if (video) video->stop();
        if (audio) audio->stop();
    }

    mutable std::mutex mutex_;
    VideoChatConfig config_;
    LiveVideoChatDependencies dependencies_;
    std::map<std::string, std::shared_ptr<VideoChatSession>> sessions_;
    std::shared_ptr<MediaStreamCallbacks> global_callbacks_;
    bool initialized_ = false;
};

LiveVideoChat::LiveVideoChat() : impl_(std::make_unique<Impl>()) {}
LiveVideoChat::~LiveVideoChat() { impl_->shutdown(); }
bool LiveVideoChat::initialize(const VideoChatConfig& config) {
    if (!validConfig(config) || config.enable_video || config.enable_audio) return false;
    return false; // No built-in production transport exists, even for media-disabled sessions.
}
bool LiveVideoChat::initialize(const VideoChatConfig& config,
                               const LiveVideoChatDependencies& dependencies) {
    return impl_->initialize(config, dependencies);
}
bool LiveVideoChat::initialize(const VideoChatConfig& config,
                               const std::string& backend_name) {
    LiveVideoChatDependencies dependencies;
    if (!LiveVideoChatBackendRegistry::instance().resolve(
            backend_name, config, &dependencies)) return false;
    return impl_->initialize(config, dependencies);
}
std::shared_ptr<VideoChatSession> LiveVideoChat::createSession(const std::string& session_id) { return impl_->createSession(session_id); }
std::shared_ptr<VideoChatSession> LiveVideoChat::getSession(const std::string& session_id) { return impl_->getSession(session_id); }
void LiveVideoChat::removeSession(const std::string& session_id) { impl_->removeSession(session_id); }
std::vector<std::string> LiveVideoChat::getActiveSessions() const { return impl_->activeSessions(); }
void LiveVideoChat::setGlobalCallbacks(std::shared_ptr<MediaStreamCallbacks> callbacks) { impl_->setCallbacks(std::move(callbacks)); }
std::shared_ptr<VideoCapture> LiveVideoChat::getVideoCapture() { return impl_->videoCapture(); }
std::shared_ptr<AudioCapture> LiveVideoChat::getAudioCapture() { return impl_->audioCapture(); }
void LiveVideoChat::shutdown() { impl_->shutdown(); }

} // namespace elizaos
