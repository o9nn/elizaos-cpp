#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<std::string> extractResponseText(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!text) return null;

    // Regex to find the content within <response>...</response>
    const auto responseMatch = text.match(/<response>([\s\S]*?)<\/response>/);

    if (!responseMatch || responseMatch[1] == undefined) {
        std::cout << "Could not find <response> tag or its content in text" << std::endl;
        // Attempt to find *any* XML block as a fallback, but log that it wasn't the expected <response>
        const auto fallbackMatch = text.match(/<(\w+)>([\s\S]*?)<\/\1>/);
        if (fallbackMatch && fallbackMatch[2] != undefined) {
            console.warn(
            "Found <" + std::to_string(fallbackMatch[1]) + "> tag instead of <response>. Using its content."
            );
            const auto fallbackContent = fallbackMatch[2].trim();
            return fallbackContent || nullptr; // Return nullptr if content is empty after trimming;
        }
        return nullptr;
    }

    const auto responseContent = responseMatch[1].trim();

    // Return null if the content is empty after trimming
    if (!responseContent) {
        std::cout << "Found <response> tag << but its content is empty" << std::endl;
        return nullptr;
    }

    // Basic unescaping for common XML entities (can be expanded if needed)
    const auto unescapedContent = responseContent;
    .replace(/&lt;/g, "<");
    .replace(/&gt;/g, ">");
    .replace(/&amp;/g, "&");
    .replace(/&quot;/g, '"');
    .replace(/&apos;/g, "'");

    return unescapedContent;

}

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

std::vector<std::string> getFilesRecursively(const std::string& dir, const std::vector<std::string>& extensions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto dirents = fs.readdirSync(dir, { withFileTypes: true });

        const auto files = dirents;
        .filter((dirent) => !dirent.isDirectory());
        .filter((dirent) => extensions.some((ext) => dirent.name.endsWith(ext)));
        .map((dirent) => path.join(dir, dirent.name));

        const auto folders = dirents;
        .filter((dirent) => dirent.isDirectory());
        .map((dirent) => path.join(dir, dirent.name));

        const auto subFiles = folders.flatMap((folder) => {;
            try {
                return getFilesRecursively(folder, extensions);
                } catch (error) {
                    std::cout << "Error accessing folder " + std::to_string(folder) + ":" << error << std::endl;
                    return [];
                }
                });

                return [...files, ...subFiles];
                } catch (error) {
                    std::cout << "Error reading directory " + std::to_string(dir) + ":" << error << std::endl;
                    return [];
                }

}

std::vector<std::string> loadDocumentation(const std::string& directoryPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto basePath = path.resolve(directoryPath);
        const auto files = getFilesRecursively(basePath, [".md", ".mdx"]);

        return files;
        .map((filePath) => {
            try {
                const auto relativePath = path.relative(basePath, filePath);
                const auto content = fs.readFileSync(filePath, "utf-8");
                return content;
                } catch (error) {
                    std::cout << "Error reading file " + std::to_string(filePath) + ":" << error << std::endl;
                    return "";
                }
                });
                .filter((content) => content.length > 0);
                } catch (error) {
                    std::cerr << "Error loading documentation:" << error << std::endl;
                    return [];
                }

}

} // namespace elizaos
