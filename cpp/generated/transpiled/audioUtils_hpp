#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V2_SRC_AUDIOUTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V2_SRC_AUDIOUTILS_H
#include "core.h"
#include "@elizaos/core.h"
using coreGetWavHeader = getWavHeader;
using corePrependWavHeader = prependWavHeader;
#include "node:stream.h"

std::shared_ptr<Buffer> getWavHeader(double audioLength, double sampleRate, double channelCount = 1, double bitsPerSample = 16);

Readable prependWavHeader(Readable readable, double audioLength, double sampleRate, double channelCount = 1, double bitsPerSample = 16);

#endif
