#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".events.hpp"
#include ".service.hpp"
#include ".utils.hpp"
#include "elizaos/core.hpp"
#include "guards.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using LiveKitAudioData = {

class VoiceManager {
public:
    VoiceManager(IAgentRuntime runtime);
    void start();
    void handleUserBuffer(auto playerId, auto buffer);
    void debouncedProcessTranscription(UUID playerId);
    void processTranscription(UUID playerId);
    void handleMessage(const std::string& message, UUID playerId);
    void playAudio(auto audioBuffer);
    void getService();

private:
    IAgentRuntime runtime_;
    std::vector<Buffer> buffers_;
    double totalLength_;
    double lastActive_;
    std::string transcriptionText_;
};

} // namespace elizaos
