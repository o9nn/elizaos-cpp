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



struct StreamingAudioConfig {
    bool enabled;
    std::optional<std::string> device;
    std::optional<double> sampleRate;
    std::optional<double> channels;
    std::optional<double> vadThreshold;
    std::optional<double> silenceTimeout;
    std::optional<double> responseDelay;
    std::optional<double> chunkSize;
};

struct AudioChunk {
    Buffer data;
    double timestamp;
    double energy;
};

class StreamingAudioCaptureService {
public:
    StreamingAudioCaptureService(IAgentRuntime runtime, StreamingAudioConfig config);
    std::future<void> initialize();
    std::future<void> startContinuousCapture();
    void processAudioChunk(Buffer chunk);
    double calculateEnergy(Buffer chunk);
    std::future<void> startStreamingTranscription();
    void endSpeech();
    std::future<void> processFinalTranscription();
    Buffer getRecentAudioData();
    std::variant<Promise<string, null>> transcribeAudio(Buffer audioData);
    Buffer rawToWav(Buffer rawData);
    std::future<void> generateResponse(const std::string& transcription);
    std::future<void> createAudioMemory(const std::string& transcription);
    std::future<void> stop();
    bool isActive();
    std::string getCurrentTranscription();
    bool isSpeechActive();

private:
    IAgentRuntime runtime_;
    StreamingAudioConfig config_;
};


} // namespace elizaos
