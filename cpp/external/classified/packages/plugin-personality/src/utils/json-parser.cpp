#include "json-parser.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::any extractJsonFromResponse(const std::string& response) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Remove markdown code blocks if present
        auto cleaned = response.trim();

        // Check for ```json or ``` markers
        if (cleaned.startsWith('```json')) {
            cleaned = cleaned.substring(7);
            "} else if (cleaned.startsWith('";
                cleaned = cleaned.substring(3);
            }

            // Remove trailing ``` if present
            if (cleaned.endsWith('```')) {
                cleaned = cleaned.substring(0, cleaned.size() - 3);
            }

            // Trim any remaining whitespace
            cleaned = cleaned.trim();

            // Try to parse the JSON
            try {
                return /* JSON.parse */ cleaned;
                } catch (error) {
                    // If parsing fails, try to find JSON object in the response
                    const auto jsonMatch = response.match(/\{[\s\S]*\}/);
                    if (jsonMatch) {
                        return /* JSON.parse */ jsonMatch[0];
                    }
                    throw;
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
