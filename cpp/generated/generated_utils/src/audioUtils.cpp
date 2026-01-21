#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/src/audioUtils.h"

std::shared_ptr<Buffer> getWavHeader(double audioLength, double sampleRate, double channelCount, double bitsPerSample)
{
    return coreGetWavHeader(audioLength, sampleRate, channelCount, bitsPerSample);
};


Readable prependWavHeader(Readable readable, double audioLength, double sampleRate, double channelCount, double bitsPerSample)
{
    return corePrependWavHeader(readable, audioLength, sampleRate, channelCount, bitsPerSample);
};


