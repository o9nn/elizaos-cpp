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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Determines the appropriate MIME type for audio data based on its format.
 * Detects WAV files by checking for the RIFF header signature.
 *
 * @param audioBuffer - The audio data buffer to check
 * @returns The appropriate MIME type string
 */
std::string getAudioMimeType(const std::vector<uint8_t>& audioBuffer);

/**
 * Result of audio processing containing the buffer and MIME type
 */
struct AudioProcessingResult {
    std::vector<uint8_t> buffer;
    std::string mimeType;
};


} // namespace elizaos
