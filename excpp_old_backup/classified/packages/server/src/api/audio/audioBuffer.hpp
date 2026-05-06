#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

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
std::string getAudioMimeType(Buffer audioBuffer);

/**
 * Result of audio processing containing the buffer and MIME type
 */
struct AudioProcessingResult {
    Buffer buffer;
    std::string mimeType;
};


} // namespace elizaos
