#include "media-transformer.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string transformPathToApiUrl(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Skip if already transformed or not a local absolute path
    if (
    !filePath ||;
    isExternalUrl(filePath) ||;
    filePath.startsWith('/media/') ||;
    !path.isAbsolute(filePath);
    ) {
        return filePath;
    }

    // Normalize path for comparison
    const auto normalizedPath = filePath.replace(/\\/g, '/');

    // Check each path configuration
    for (const auto& config : PATH_CONFIGS)
        const auto configPathRaw = config.getPath().replace(/\\/g, '/');
        const auto configPath = std::to_string(configPathRaw) + "/";

        // Use strict boundary-aware startsWith check to prevent path traversal
        if (normalizedPath == configPathRaw || normalizedPath.startsWith(configPath)) {
            const auto relative =;
            normalizedPath == configPathRaw ? '' : normalizedPath.slice(configPath.length);

            // Only process if we have a valid relative path
            if (relative) {
                const auto match = relative.match(config.pattern);
                if (match) {
                    const auto [, id, filename] = match;
                    return std::to_string(config.apiPrefix) + std::to_string(encodeURIComponent(id)) + std::to_string('/') + std::to_string(encodeURIComponent(filename));
                }
            }
        }
    }

    return filePath;

}

AttachmentInput attachmentsToApiUrls(AttachmentInput attachments) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!attachments) return attachments;

    if (Array.isArray(attachments)) {
        return attachments.map((attachment) => {;
            if (typeof attachment == 'string') {
                return transformPathToApiUrl(attachment);
            }
            if (attachment.url) {
                return { ...attachment, url: transformPathToApiUrl(attachment.url) }
            }
            return attachment;
            });
        }

        // Single attachment
        if (typeof attachments == 'string') {
            return transformPathToApiUrl(attachments);
        }
        if (attachments.url) {
            return { ...attachments, url: transformPathToApiUrl(attachments.url) }
        }
        return attachments;

}

MessageWithAttachments transformMessageAttachments(MessageWithAttachments message) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!message || typeof message != 'object') {
        return message;
    }

    // Transform attachments in content
    if (message.content && typeof message.content == 'object' && 'attachments' in message.content) {
        const auto content = message.content;
        if (content.attachments) {
            content.attachments = attachmentsToApiUrls(content.attachments);
        }
    }

    // Transform attachments in metadata
    if (message.metadata.attachments) {
        message.metadata.attachments = attachmentsToApiUrls(message.metadata.attachments);
    }

    return message;

}

} // namespace elizaos
