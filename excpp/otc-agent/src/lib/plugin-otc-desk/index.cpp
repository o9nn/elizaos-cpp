#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

EntitySourceMetadata getEntitySourceMetadata(const std::optional<Entity>& entity, const std::string& source) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!entity.metadata) return {};
    const auto sourceData = entity.metadata[source];
    if (!sourceData || typeof sourceData != "object") return {};
    return sourceData;

}

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
            logger.warn(
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

std::future<std::optional<std::string>> getLatestResponseId(IAgentRuntime runtime, const std::string& roomId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    (runtime.getCache<string>(;
    "response_id:" + std::to_string(runtime.agentId) + ":" + std::to_string(roomId)
    )) || nullptr;
    );

}

std::future<void> setLatestResponseId(IAgentRuntime runtime, const std::string& roomId, const std::string& responseId) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!responseId || typeof responseId != "string") {
            std::cerr << "[setLatestResponseId] Invalid responseId:" << responseId << std::endl;
            throw std::runtime_error(`Invalid responseId: ${responseId}`);
        }
        const auto key = "response_id:" + std::to_string(runtime.agentId) + ":" + std::to_string(roomId);
        console.log("[setLatestResponseId] Setting cache:", {
            key,
            responseId: responseId.substring(0, 8),
            });
            runtime.setCache(key, responseId);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> clearLatestResponseId(IAgentRuntime runtime, const std::string& roomId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto key = "response_id:" + std::to_string(runtime.agentId) + ":" + std::to_string(roomId);
    std::cout << "[clearLatestResponseId] Deleting cache key:" << key << std::endl;
    runtime.deleteCache(key);

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
