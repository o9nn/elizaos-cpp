#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "types.hpp"

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
    static std::future<SamTTSService> start(IAgentRuntime runtime);
    std::future<void> stop();
    std::future<std::vector<uint8_t>> generateAudio(const std::string& text, SamTTSOptions options = {});
    std::vector<uint8_t> createWAVBuffer(const std::vector<uint8_t>& audioData, double sampleRate = 22050);
    std::string capabilityDescription() const;
    std::future<bool> testSynthesis();


} // namespace elizaos
