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
                const auto mediaType = attachment.contentType || 'image/png';
                return { data: mediaBuffer, mediaType }
            }
            // if (fs.existsSync(attachment.url)) {
            //   // Handle local file paths
            //   const mediaBuffer = await fs.promises.readFile(path.resolve(attachment.url));
            //   const mediaType = attachment.contentType || 'image/png';
            //   return { data: mediaBuffer, mediaType };
            // }
            throw std::runtime_error(`File not found: ${attachment.url}. Make sure the path is correct.`);
            });
            );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::vector<Media>> processAttachments(const std::vector<Media>& attachments, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!attachments || attachments.length == 0) {
            return [];
        }
        logger.debug(`[Bootstrap] Processing ${attachments.length} attachment(s)`);

        const std::vector<Media> processedAttachments = [];

        for (const auto& attachment : attachments)
            try {
                // Start with the original attachment
                const Media processedAttachment = { ...attachment };

                const auto isRemote = /^(http|https):\/\//.test(attachment.url);
                const auto url = isRemote ? attachment.url : getLocalServerUrl(attachment.url);
                // Only process images that don't already have descriptions
                if (attachment.contentType == ContentType.IMAGE && !attachment.description) {
                    logger.debug(`[Bootstrap] Generating description for image: ${attachment.url}`);

                    auto imageUrl = url;

                    if (!isRemote) {
                        // Only convert local/internal media to base64
                        const auto res = fetch(url);
                        if (!res.ok) throw new Error(`Failed to fetch image: ${res.statusText}`);

                        const auto arrayBuffer = res.arrayBuffer();
                        const auto buffer = Buffer.from(arrayBuffer);
                        const auto contentType = res.headers.get('content-type') || 'application/octet-stream';
                        "data:" + std::to_string(contentType) + ";base64," + std::to_string(buffer.tostd::to_string('base64'))
                    }

                    try {
                        const auto response = runtime.useModel(ModelType.IMAGE_DESCRIPTION, {;
                            prompt: imageDescriptionTemplate,
                            imageUrl,
                            });

                            if (typeof response == 'string') {
                                // Parse XML response
                                const auto parsedXml = parseKeyValueXml(response);

                                if (parsedXml.description && parsedXml.text) {
                                    processedAttachment.description = parsedXml.description;
                                    processedAttachment.title = parsedXml.title || 'Image';
                                    processedAttachment.text = parsedXml.text;

                                    logger.debug(
                                    "[Bootstrap] Generated description: " + std::to_string(processedAttachment.description.substring(0, 100)) + "..."
                                    );
                                    } else {
                                        std::cout << "[Bootstrap] Failed to parse XML response for image description" << std::endl;
                                    }
                                    } else if (response && typeof response == 'object' && 'description' in response) {
                                        // Handle object responses for backwards compatibility
                                        processedAttachment.description = response.description;
                                        processedAttachment.title = response.title || 'Image';
                                        processedAttachment.text = response.description;

                                        logger.debug(
                                        "[Bootstrap] Generated description: " + std::to_string(processedAttachment.description.substring(0, 100)) + "..."
                                        );
                                        } else {
                                            std::cout << "[Bootstrap] Unexpected response format for image description" << std::endl;
                                        }
                                        } catch (error) {
                                            std::cerr << "[Bootstrap] Error generating image description:" << error << std::endl;
                                            // Continue processing without description
                                        }
                                        } else if (attachment.contentType == ContentType.DOCUMENT && !attachment.text) {
                                            const auto res = fetch(url);
                                            if (!res.ok) throw new Error(`Failed to fetch document: ${res.statusText}`);

                                            const auto contentType = res.headers.get('content-type') || '';
                                            const auto isPlainText = contentType.startsWith('text/plain');

                                            if (isPlainText) {
                                                logger.debug(`[Bootstrap] Processing plain text document: ${attachment.url}`);

                                                const auto textContent = res.text();
                                                processedAttachment.text = textContent;
                                                processedAttachment.title = processedAttachment.title || 'Text File';

                                                logger.debug(
                                                "[Bootstrap] Extracted text content (first 100 chars): " + std::to_string(processedAttachment.text.substring(0, 100)) + "..."
                                                );
                                                } else {
                                                    std::cout << "[Bootstrap] Skipping non-plain-text document: " + std::to_string(contentType) << std::endl;
                                                }
                                            }

                                            processedAttachments.push(processedAttachment);
                                            } catch (error) {
                                                std::cerr << "[Bootstrap] Failed to process attachment " + std::to_string(attachment.url) + ":" << error << std::endl;
                                                // Add the original attachment if processing fails
                                                processedAttachments.push(attachment);
                                            }
                                        }

                                        return processedAttachments;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

bool shouldBypassShouldRespond(IAgentRuntime runtime, std::optional<Room> room, std::optional<std::string> source) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!room) return false;

    function normalizeEnvList(value: unknown): string[] {
        if (!value || typeof value != 'string') return [];

        const auto cleaned = value.trim().replace(/^\[|\]$/g, '');
        return cleaned;
        .split(',');
        .map((v) => v.trim());
        .filter(Boolean);
    }

    const auto defaultBypassTypes = [;
    ChannelType.DM,
    ChannelType.VOICE_DM,
    ChannelType.SELF,
    ChannelType.API,
    ];

    const auto defaultBypassSources = ['client_chat'];

    const auto bypassTypesSetting = normalizeEnvList(runtime.getSetting('SHOULD_RESPOND_BYPASS_TYPES'));
    const auto bypassSourcesSetting = normalizeEnvList(;
    runtime.getSetting('SHOULD_RESPOND_BYPASS_SOURCES');
    );

    const auto bypassTypes = new Set(;
    [...defaultBypassTypes.map((t) => t.toString()), ...bypassTypesSetting].map((s: string) =>
    s.trim().toLowerCase();
    );
    );

    const auto bypassSources = [...defaultBypassSources, ...bypassSourcesSetting].map((s: string) =>;
    s.trim().toLowerCase();
    );

    const auto roomType = room.type.toString().toLowerCase();
    const auto sourceStr = source.toLowerCase() || '';

    return bypassTypes.has(roomType) || bypassSources.some((pattern) => sourceStr.includes(pattern));

}

std::string cleanupPostText(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Remove quotes
    auto cleanedText = text.replace(/^['"](.*)['"]$/, '$1');
    // Fix newlines
    cleanedText = cleanedText.replaceAll(/\\n/g, '\n\n');
    cleanedText = cleanedText.replace(/([^\n])\n([^\n])/g, '$1\n\n$2');

    return cleanedText;

}

} // namespace elizaos
