#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct AudioConfig {
    bool enabled;
    double transcriptionInterval;
    std::optional<std::string> device;
    std::optional<double> sampleRate;
    std::optional<double> channels;
};

class AudioCaptureService {
public:
    AudioCaptureService(IAgentRuntime runtime, AudioConfig config);
    std::future<void> initialize();
    Promise< checkAudioTools();
    void catch(auto _error);
    void startTranscriptionLoop();
    std::variant<Promise<string, null>> recordAndTranscribe();
    std::future<void> recordAudio(const std::string& outputPath, double duration);
    std::future<void> createAudioMemory(const std::string& transcription);
    std::future<std::vector<std::string>> listAudioDevices();
    bool isActive();
    std::future<void> stop();

private:
    IAgentRuntime runtime_;
    AudioConfig config_;
};


} // namespace elizaos
