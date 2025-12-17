#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".hyperfy/src/core/systems/System.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct LiveKitInitOptions {
    std::string wsUrl;
    std::string token;
};

class AgentLiveKit {
public:
    AgentLiveKit(const std::any& world);
    std::future<void> deserialize(LiveKitInitOptions opts);
    std::future<void> stop();
    void setupRoomEvents();
    void init();
    void preTick();
    void preFixedUpdate();
    void fixedUpdate();
    void postFixedUpdate();
    void preUpdate();
    void update();
    void postUpdate();
    void lateUpdate();
    void postLateUpdate();
    void commit();
    void postTick();
    void start();
    std::future<void> publishAudioStream(Buffer audioBuffer);
    std::future<Int16Array> convertToPcm(Buffer buffer, auto sampleRate);
    std::string detectAudioFormat(Buffer buffer);
};

} // namespace elizaos
