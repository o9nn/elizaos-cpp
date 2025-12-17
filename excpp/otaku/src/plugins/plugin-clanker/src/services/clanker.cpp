#include "clanker.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::any sanitizeError(const std::any& error) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (error == null || error == undefined) {
            return error;
        }

        if (typeof error == "bigint") {
            return std::to_string(error);
        }

        if (error instanceof Error) {
            // Create a plain object with error properties
            const std::any sanitized = {;
                message: error.message,
                name: error.name,
                stack: error.stack,
                };

                // Copy other enumerable properties and sanitize them
                for (const int key in error) {
                    if (Object.prototype.hasOwnProperty.call(error, key)) {
                        sanitized[key] = sanitizeError((error)[key]);
                    }
                }

                return sanitized;
            }

            if (Array.isArray(error)) {
                return error.map(sanitizeError);
            }

            if (typeof error == "object") {
                const std::any result = {};
                for (const int [key, value] of Object.entries(error)) {
                    result[key] = sanitizeError(value);
                }
                return result;
            }

            return error;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::string detectPlatform(const std::string& url) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto lowerUrl = url.toLowerCase();
    if (lowerUrl.includes('twitter.com') || lowerUrl.includes('x.com')) return 'x';
    if (lowerUrl.includes('telegram') || lowerUrl.includes('t.me')) return 'telegram';
    if (lowerUrl.includes('discord')) return 'discord';
    if (lowerUrl.includes('github')) return 'github';
    if (lowerUrl.includes('reddit')) return 'reddit';
    if (lowerUrl.includes('medium')) return 'medium';
    if (lowerUrl.includes('youtube')) return 'youtube';
    if (lowerUrl.includes('instagram')) return 'instagram';
    if (lowerUrl.includes('tiktok')) return 'tiktok';
    if (lowerUrl.includes('linkedin')) return 'linkedin';
    if (lowerUrl.includes('facebook')) return 'facebook';
    return "other"; // fallback;

}

} // namespace elizaos
