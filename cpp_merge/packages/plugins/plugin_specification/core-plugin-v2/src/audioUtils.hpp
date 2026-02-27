#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V2_SRC_AUDIOUTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V2_SRC_AUDIOUTILS_H
#include "core.hpp"
// External dependency removed
using coreGetWavHeader = getWavHeader;
using corePrependWavHeader = prependWavHeader;
#include "node:stream.h"

std::shared_ptr<Buffer> getWavHeader(double audioLength, double sampleRate, double channelCount = 1, double bitsPerSample = 16);

Readable prependWavHeader(Readable readable, double audioLength, double sampleRate, double channelCount = 1, double bitsPerSample = 16);

#endif
