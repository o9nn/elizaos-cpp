#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct SamTTSOptions {
    std::optional<double> speed;
    std::optional<double> pitch;
    std::optional<double> throat;
    std::optional<double> mouth;
};

/**
 * SAM TTS Service
 *
 * Provides retro text-to-speech using the SAM Speech Synthesizer.
 * Generates 8-bit audio buffers that can be streamed through the hardware bridge.
 */
class SamTTSService {
public:
    SamTTSService(std::optional<IAgentRuntime> runtime);
    std::future<SamTTSService> start(IAgentRuntime runtime);
    std::future<void> stop();
    std::future<Uint8Array> generateAudio(const std::string& text, SamTTSOptions = {} options);
    std::future<Uint8Array> speakText(const std::string& text, SamTTSOptions = {} options);
    Uint8Array createWAVBuffer(Uint8Array audioData, number = 22050 sampleRate);
    std::string capabilityDescription() const;
    std::future<bool> testSynthesis();


} // namespace elizaos
