#include "audioUtils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

Buffer getWavHeader(double audioLength, double sampleRate, auto channelCount, auto bitsPerSample) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreGetWavHeader(audioLength, sampleRate, channelCount, bitsPerSample);

}

typeof Readable prependWavHeader(typeof Readable readable, double audioLength, double sampleRate, auto channelCount, auto bitsPerSample) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return corePrependWavHeader(;
    readable,
    audioLength,
    sampleRate,
    channelCount,
    bitsPerSample;
    );

}

} // namespace elizaos
