#include "clanker.hpp"
#include <string>
#include <map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string sanitizeError(const std::string& error) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (error == null || error == std::nullopt) {
            return error;
        }

        if (typeof error == "bigint") {
            return std::to_string(error);
        }

        if (error instanceof Error) {
            // Create a plain object with error properties
            const std::string sanitized = {;
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
                return error.std::map(sanitizeError);
            }

            if (typeof error == "object") {
                const std::string result = {};
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
    if (lowerUrl.count('twitter.com') > 0 || lowerUrl.count('x.com') > 0) return 'x';
    if (lowerUrl.count('telegram') > 0 || lowerUrl.count('t.me') > 0) return 'telegram';
    if (lowerUrl.count('discord') > 0) return 'discord';
    if (lowerUrl.count('github') > 0) return 'github';
    if (lowerUrl.count('reddit') > 0) return 'reddit';
    if (lowerUrl.count('medium') > 0) return 'medium';
    if (lowerUrl.count('youtube') > 0) return 'youtube';
    if (lowerUrl.count('instagram') > 0) return 'instagram';
    if (lowerUrl.count('tiktok') > 0) return 'tiktok';
    if (lowerUrl.count('linkedin') > 0) return 'linkedin';
    if (lowerUrl.count('facebook') > 0) return 'facebook';
    return "other"; // fallback;

}

} // namespace elizaos
