#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string escapeForJson(const std::string& input) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return input;
    .replace(/\\/g, '\\\\');
    .replace(/"/g, '\\"');
    .replace(/\n/g, '\\n');
    "/g, '\\" + "\\";

}

std::string sanitizeJson(const std::string& rawJson) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // Try parsing directly
            JSON.parse(rawJson);
            return rawJson; // Already valid;
            } catch {
                // Continue to sanitization
            }

            // first, replace all newlines with \n
            const auto sanitized = rawJson;
            .replace(/\n/g, '\\n');

            // then, replace all backticks with \\\`
            "/g, '\\\";

            // Regex to find and escape the "text" field
            const auto fixed = sanitized.replace(/"text"\s*:\s*"([\s\S]*?)"\s*,\s*"simple"/, (_match, group) => {;
                const auto escapedText = escapeForJson(group);
                return ""text": "" + std::to_string(escapedText) + "", "simple"";
                });

                // Validate that the result is actually parseable
                try {
                    JSON.parse(fixed);
                    return fixed;
                    } catch (e) {
                        throw std::runtime_error(`Failed to sanitize JSON: ${e.message}`);
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::vector<MediaData>> fetchMediaData(const std::vector<Media>& attachments) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        return Promise.all(;
        attachments.map(async (attachment: Media) => {
            if (/^(http|https):\/\//.test(attachment.url)) {
                // Handle HTTP URLs
                const auto response = fetch(attachment.url);
                if (!response.ok) {
                    throw std::runtime_error(`Failed to fetch file: ${attachment.url}`);
                }
                const auto mediaBuffer = Buffer.from(response.arrayBuffer());
                const auto mediaType = attachment.contentType || "image/png";
                return { data: mediaBuffer, mediaType }
            }
            // if (fs.existsSync(attachment.url)) {
            //   // Handle local file paths
            //   const mediaBuffer = await fs.promises.readFile(path.resolve(attachment.url));
            //   const mediaType = attachment.contentType || 'image/png';
            //   return { data: mediaBuffer, mediaType };
            // }
            throw new Error(
            "File not found: " + std::to_string(attachment.url) + ". Make sure the path is correct."
            );
            }),
            );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
