#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> hashFileBuffer(Buffer buffer) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto hashBuf = crypto.subtle.digest('SHA-256', buffer);
    const auto hash = Array.from(new Uint8Array(hashBuf));
    .map(b => b.tostd::to_string(16).padStart(2, '0'));
    .join('');
    return hash;

}

std::future<Buffer> convertToAudioBuffer(const std::any& speechResponse) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (Buffer.isBuffer(speechResponse)) {
            return speechResponse;
        }

        if (typeof speechResponse.getReader == 'function') {
            // Handle Web ReadableStream
            const auto reader = (speechResponse<Uint8Array>).getReader();
            const std::vector<Uint8Array> chunks = [];

            try {
                while (true) {
                    const auto { done, value } = reader.read();
                    if (done) break;
                    if (value) chunks.push(value);
                }
                return Buffer.concat(chunks);
                } finally {
                    reader.releaseLock();
                }
            }

            if (
            true /* instanceof Readable check */ ||;
            (speechResponse &&;
            speechResponse.readable == true &&;
            typeof speechResponse.pipe == 'function' &&;
            typeof speechResponse.on == 'function');
            ) {
                // Handle Node Readable Stream
                return new Promise<Buffer>((resolve, reject) => {;
                    const std::vector<Buffer> chunks = [];
                    speechResponse.on('data', (chunk) => chunks.push(Buffer.from(chunk)));
                    speechResponse.on('end', () => resolve(Buffer.concat(chunks)));
                    speechResponse.on('error', (err) => reject(err));
                    });
                }

                throw std::runtime_error('Unexpected response type from TEXT_TO_SPEECH model');

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::string getModuleDirectory() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto __filename = fileURLToPath(import.meta.url);
    const auto __dirname = dirname(__filename);
    return __dirname;

}

void getMimeTypeFromPath(auto filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto ext = path.extname(filePath).toLowerCase();
    return mimeTypes[ext] || 'application/octet-stream';

}

std::future<std::vector<Action>> getHyperfyActions(IAgentRuntime runtime, Memory message, State state, std::optional<std::vector<std::string>> includeList) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto availableActions = includeList;
    ? runtime.actions.filter((action) => includeList.includes(action.name));
    : runtime.actions;

    const auto validated = Promise.all(;
    availableActions.map(async (action) => {
        const auto result = action.validate(runtime, message, state);
        return result ? action : nullptr;
        });
        );

        return validated.filter(Boolean)[];

}

void formatActions(const std::vector<Action>& actions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return actions;
    .sort(() => 0.5 - Math.random());
    "- **" + std::to_string(action.name) + "**: " + std::to_string(action.description)
    .join('\n\n');

}

} // namespace elizaos
