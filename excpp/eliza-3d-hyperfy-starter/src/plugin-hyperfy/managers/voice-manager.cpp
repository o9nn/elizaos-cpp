#include "voice-manager.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isLoudEnough(Buffer pcmBuffer, auto threshold = 1000) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto sum = 0;
    const auto sampleCount = Math.floor(pcmBuffer.length / 2); // 16-bit samples;

    for (int i = 0; i < pcmBuffer.length; i += 2) {
        const auto sample = pcmBuffer.readInt16LE(i);
        sum += Math.abs(sample);
    }

    const auto avgAmplitude = sum / sampleCount;
    return avgAmplitude > threshold;

}

bool isValidTranscription(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!text || text.includes('[BLANK_AUDIO]')) return false;
    return true;

}

} // namespace elizaos
