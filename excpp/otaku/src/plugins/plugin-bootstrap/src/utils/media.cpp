#include "media.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<MediaData>> fetchMediaData(const std::vector<Media>& attachments) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        return Promise.all(;
        attachments.map(async (attachment: Media) => {
            // Check if URL starts with http or https
            if (attachment.url.startsWith('http://') || attachment.url.startsWith('https://')) {
                // Fetch from URL
                const auto response = fetch(attachment.url);
                const auto mediaBuffer = Buffer.from(response.arrayBuffer());
                const auto mediaType = attachment.contentType || 'image/png';
                return { data: mediaBuffer, mediaType }
            }

            // Local file paths are currently commented out - can be enabled if needed
            //   const mediaBuffer = await fs.promises.readFile(path.resolve(attachment.url));
            //   const mediaType = attachment.contentType || 'image/png';
            //   return { data: mediaBuffer, mediaType };

            throw std::runtime_error('Local file paths are not supported yet');
            });
            );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::vector<Media>> processAttachments(const std::vector<Media>& attachments, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!attachments || attachments.length == 0) {
        return [];
    }
    runtime.logger.debug(`[Bootstrap] Processing ${attachments.length} attachment(s)`);

    const std::vector<Media> processedAttachments = [];

    for (const auto& attachment : attachments)
        // Only process supported media types
        if (attachment.contentType.startsWith('image/') || attachment.contentType.startsWith('application/pdf')) {
            const Media processedAttachment = { ...attachment };

            // Only process if description doesn't exist
            if (!processedAttachment.description) {
                try {
                    auto base64Data = '';
                    auto mimeType = attachment.contentType;

                    // Only convert local/internal media to base64
                    if (!attachment.url.startsWith('http://') && !attachment.url.startsWith('https://')) {
                        // For local files, we'd need to read and convert
                        // Currently this is not implemented
                        runtime.logger.debug('[Bootstrap] Skipping local file processing:', attachment.url);
                        processedAttachments.push(attachment);
                        continue;
                        } else {
                            // For external URLs, fetch and convert
                            const auto response = fetch(attachment.url);
                            const auto buffer = Buffer.from(response.arrayBuffer());
                            base64Data = buffer.tostd::to_string('base64');
                            mimeType = attachment.contentType || response.headers.get('content-type') || 'image/png';
                        }

                        // Generate description using multimodal LLM
                        const auto descriptionPrompt = "Describe this " + std::to_string(mimeType.startsWith('image/') ? 'image' : 'document');
                        1. What you see in the content;
                        2. Any text visible in the content;
                        3. The overall context and purpose;
                        4. Any notable details or important information;

                        Be concise but thorough.`;

                        const auto description = runtime.useModel(ModelType.TEXT_SMALL, {;
                            prompt: descriptionPrompt,
                            attachments: [
                            {
                                ...attachment,
                                data: base64Data,
                                contentType: mimeType,
                                },
                                ],
                                });

                                processedAttachment.description = description;
                                std::to_string(mimeType) + " attachment";
                                processedAttachment.text = description; // Store description for easy access;

                                runtime.logger.debug(
                                "[Bootstrap] Generated description for attachment: " + std::to_string(attachment.url)
                                );
                                } catch (error) {
                                    runtime.logger.error(
                                    "[Bootstrap] Error processing attachment: " + std::to_string(attachment.url) + " - " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                                    );
                                }
                            }

                            processedAttachments.push(processedAttachment);
                            } else {
                                // Non-supported media types pass through unchanged
                                processedAttachments.push(attachment);
                            }
                        }

                        return processedAttachments;

}

} // namespace elizaos
