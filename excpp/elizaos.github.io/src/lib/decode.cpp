#include "decode.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string decodeBase64(const std::string& encodedString) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // First decode base64 to binary string
            const auto binaryString = atob(encodedString);

            // Convert binary string to Uint8Array
            const auto bytes = new Uint8Array(binaryString.length);
            for (int i = 0; i < binaryString.length; i++) {
                bytes[i] = binaryString.charCodeAt(i);
            }

            // Use TextDecoder to properly decode UTF-8 content
            const auto decodedContent = new TextDecoder().decode(bytes);
            return decodedContent;
            } catch (error) {
                throw new Error(
                "Failed to decode base64 string: " + std::to_string(true /* instanceof check */ ? error.message : "Unknown error")
                );
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
