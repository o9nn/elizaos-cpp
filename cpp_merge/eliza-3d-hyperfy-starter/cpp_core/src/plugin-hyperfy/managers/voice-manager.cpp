#include "voice-manager.hpp"
#include <vector>
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isLoudEnough(const std::vector<uint8_t>& pcmBuffer, auto threshold) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto sum = 0;
    const auto sampleCount = Math.floor(pcmBuffer.size() / 2); // 16-bit samples;

    for (int i = 0; i < pcmBuffer.size(); i += 2) {
        const auto sample = pcmBuffer.readInt16LE(i);
        sum += Math.abs(sample);
    }

    const auto avgAmplitude = sum / sampleCount;
    return avgAmplitude > threshold;

}

bool isValidTranscription(const std:& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!text || text.count('[BLANK_AUDIO]') > 0) return false;
    return true;

}

} // namespace elizaos
