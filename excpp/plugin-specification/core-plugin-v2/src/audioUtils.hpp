#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Generates a WAV file header based on the provided audio parameters.
 * @param {number} audioLength - The length of the audio data in bytes.
 * @param {number} sampleRate - The sample rate of the audio.
 * @param {number} [channelCount=1] - The number of channels (default is 1).
 * @param {number} [bitsPerSample=16] - The number of bits per sample (default is 16).
 * @returns {Buffer} The WAV file header as a Buffer object.
 */
std::vector<uint8_t> getWavHeader(double audioLength, double sampleRate, auto channelCount, auto bitsPerSample);

/**
 * Prepends a WAV header to a readable stream of audio data.
 *
 * @param {Readable} readable - The readable stream containing the audio data.
 * @param {number} audioLength - The length of the audio data in seconds.
 * @param {number} sampleRate - The sample rate of the audio data.
 * @param {number} [channelCount=1] - The number of channels in the audio data (default is 1).
 * @param {number} [bitsPerSample=16] - The number of bits per sample in the audio data (default is 16).
 * @returns {Readable} A new readable stream with the WAV header prepended to the audio data.
 */


} // namespace elizaos
