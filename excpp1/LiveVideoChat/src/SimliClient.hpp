#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct SimliClientConfig {
    std::string apiKey;
    std::string faceID;
    bool handleSilence;
    std::shared_ptr<HTMLVideoElement> videoRef;
    std::shared_ptr<HTMLAudioElement> audioRef;
};

class SimliClient {
public:
    SimliClient();
    void Initialize(SimliClientConfig config);
    void createPeerConnection();
    void setupPeerConnectionListeners();
    void start();
    void setupDataChannelListeners();
    void startDataChannelInterval();
    void stopDataChannelInterval();
    void sendPingMessage();
    void initializeSession();
    void negotiate();
    std::future<void> waitForIceGathering();
    void sendAudioData(Uint8Array audioData);
    void close();
    void if(auto event.persisted);
};


} // namespace elizaos
