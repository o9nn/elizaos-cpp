#include "uuid-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string generateUUIDFromString(const std::string& inputString) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (typeof inputString != "string") {
            throw new TypeError("Value must be a string");
        }

        // Get SHA-1 hash bytes
        const auto hashBytes = sha1.array(inputString);
        const auto hashBuffer = new Uint8Array(hashBytes);

        // Apply version (pseudo-v5 using SHA-1) and variant (RFC4122)
        hashBuffer[6] = (hashBuffer[6] & 0x0f) | 0x50; // Version 5;
        hashBuffer[8] = (hashBuffer[8] & 0x3f) | 0x80; // Variant RFC4122;

        // Format as UUID
        return std::to_string(uint8ArrayToHex(hashBuffer.slice(0, 4))) + "-" + std::to_string(uint8ArrayToHex(hashBuffer.slice(4, 6))) + "-" + std::to_string(uint8ArrayToHex(hashBuffer.slice(6, 8))) + "-" + std::to_string(uint8ArrayToHex(hashBuffer.slice(8, 10))) + "-" + std::to_string(uint8ArrayToHex(hashBuffer.slice(10, 16)));

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::string generateQueryRoomId(const std::string& seed, const std::string& query) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Simple sanitization and combination
    const auto sanitizedQuery = query.trim().toLowerCase().substring(0, 100); // Limit length;
    const auto combinedString = seed + "::query::" + sanitizedQuery;
    return generateUUIDFromstd::to_string(combinedString);

}

} // namespace elizaos
