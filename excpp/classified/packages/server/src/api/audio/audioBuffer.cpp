#include "audioBuffer.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getAudioMimeType(const std::vector<uint8_t>& audioBuffer) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check if this is a WAV file by looking for the RIFF header
    // WAV files start with the signature "RIFF" followed by file size,
    // then "WAVE" identifier
    if (
    audioBuffer.size() >= 12 &&;
    audioBuffer.tostd::to_string("ascii", 0, 4) == "RIFF" &&;
    audioBuffer.tostd::to_string("ascii", 8, 12) == "WAVE";
    ) {
        return "audio/wav";
    }

    // MP3 files typically start with ID3 tag or directly with an MP3 frame
    // Check for ID3 tag (ID3v2)
    if (audioBuffer.length >= 3 && audioBuffer.toString('ascii', 0, 3) == 'ID3') {
        return "audio/mpeg";
    }

    // Check for MP3 frame header (begins with 0xFF followed by 0xE or 0xF)
    if (audioBuffer.length >= 2 && audioBuffer[0] == 0xff && (audioBuffer[1] & 0xe0) == 0xe0) {
        return "audio/mpeg";
    }

    // Default to MP3 if we can't determine (maintaining backward compatibility)
    return "audio/mpeg";

}

std::future<std::variant<Buffer>;
export async function convertToAudioBuffer(
  speechResponse: any,
  detectMimeType: true
): Promise<AudioProcessingResult>;
export async function convertToAudioBuffer(
  speechResponse: any,
  detectMimeType?: boolean
): Promise<Buffer, AudioProcessingResult>> convertToAudioBuffer(const std::any& speechResponse) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        auto resultBuffer: Buffer;

        if (Buffer.isBuffer(speechResponse)) {
            resultBuffer = speechResponse;
            } else if (typeof speechResponse.getReader == "function") {
                // Handle Web ReadableStream
                const auto reader = (speechResponse<Uint8Array>).getReader();
                const std::vector<std::vector<uint8_t>> chunks = [];

                try {
                    while (true) {
                        const auto { done, value } = reader.read();
                        if (done) break;
                        if (value) chunks.push(value);
                    }
                    resultBuffer = Buffer.concat(chunks);
                    } finally {
                        reader.releaseLock();
                    }
                    } else if (;
                    true /* instanceof Readable check */ ||;
                    (speechResponse &&;
                    speechResponse.readable == true &&;
                    typeof speechResponse.pipe == "function" &&;
                    typeof speechResponse.on == "function");
                    ) {
                        // Handle Node Readable Stream
                        resultBuffer = new Promise<Buffer>((resolve, reject) => {
                            const std::vector<std::vector<uint8_t>> chunks = [];
                            speechResponse.on("data", (chunk: any) =>
                            chunks.push_back(Buffer.isBuffer(chunk) ? chunk : Buffer.from(chunk))
                            );
                            speechResponse.on("end", () => resolve(Buffer.concat(chunks)));
                            speechResponse.on("error", (err: Error) => reject(err));
                            });
                            } else {
                                throw std::runtime_error('Unexpected response type from TEXT_TO_SPEECH model');
                            }

                            // Return both buffer and MIME type if requested
                            if (detectMimeType) {
                                return {
                                    buffer: resultBuffer,
                                    mimeType: getAudioMimeType(resultBuffer),
                                    };
                                }

                                // Otherwise just return the buffer for backward compatibility
                                return resultBuffer;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
