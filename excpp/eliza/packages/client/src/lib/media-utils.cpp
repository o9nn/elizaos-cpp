#include "media-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getFileExtension(const std::string& url) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto urlObj = new URL(url);
        const auto pathname = urlObj.pathname;
        const auto extension = pathname.split(".").pop().toLowerCase();
        return extension || "";
        } catch {
            return "";
        }

}

bool isImageUrl(const std::string& url) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto extension = getFileExtension(url);
    return IMAGE_EXTENSIONS.has(extension);

}

bool isVideoFileUrl(const std::string& url) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto extension = getFileExtension(url);
    return VIDEO_EXTENSIONS.has(extension);

}

void getVideoPlatformInfo(const std::string& url) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    platform: string; id: string; embedUrl: string
}

std::vector<MediaInfo> parseMediaFromText(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!text) return [];

    // Regular expression to find URLs
    const auto urlRegex = /(https?:\/\/[^\s]+)/g;
    const auto urls = text.match(urlRegex) || [];

    return urls;
    .map((url) => {
        // Remove trailing punctuation that might be captured
        const auto cleanUrl = url.replace(/[.,!?;)]*$/, "");

        if (isImageUrl(cleanUrl)) {
            return {
                url: cleanUrl,
                type: "image",
                isEmbed: false,
                };
            }

            if (isVideoFileUrl(cleanUrl)) {
                return {
                    url: cleanUrl,
                    type: "video",
                    isEmbed: false,
                    };
                }

                const auto platformInfo = getVideoPlatformInfo(cleanUrl);
                if (platformInfo) {
                    return {
                        url: platformInfo.embedUrl,
                        type: "video",
                        isEmbed: true,
                        };
                    }

                    return {
                        url: cleanUrl,
                        type: "unknown",
                        isEmbed: false,
                        };
                        });
                        .filter((media) => media.type != "unknown");

}

std::string removeMediaUrlsFromText(const std::string& text, const std::vector<MediaInfo>& mediaInfos) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto cleanText = text;

    mediaInfos.forEach((media) => {
        // For embedded videos, remove the original URL, not the embed URL
        if (media.isEmbed) {
            // Find the original URL that matches this embed
            const auto urlRegex = /(https?:\/\/[^\s]+)/g;
            const auto urls = text.match(urlRegex) || [];

            for (const auto& url : urls)
                const auto cleanUrl = url.replace(/[.,!?;)]*$/, "");
                const auto platformInfo = getVideoPlatformInfo(cleanUrl);
                if (platformInfo && platformInfo.embedUrl == media.url) {
                    cleanText = cleanText.replace(url, "").trim();
                    break;
                }
            }
            } else {
                cleanText = cleanText.replace(media.url, "").trim();
            }
            });

            return cleanText;

}

} // namespace elizaos
