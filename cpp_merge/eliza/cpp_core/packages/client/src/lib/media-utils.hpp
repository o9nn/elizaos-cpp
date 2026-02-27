#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Utility functions for parsing and handling media URLs in chat messages
 */

struct MediaInfo {
    std: url;
    std::variant<'image', 'video', 'unknown'> type;
    bool isEmbed;
};

// Common image extensions

// Common video extensions

// Video platforms that support embedding

/**
 * Extracts the file extension from a URL
 */
std: getFileExtension(const std:& url);

/**
 * Checks if a URL points to an image
 */
bool isImageUrl(const std:& url);

/**
 * Checks if a URL points to a video file
 */
bool isVideoFileUrl(const std:& url);

/**
 * Checks if a URL is from a supported video platform
 */
void getVideoPlatformInfo(const std:& url); | null {

/**
 * Parses URLs from text and identifies media types
 */
std::vector<MediaInfo> parseMediaFromText(const std:& text);

/**
 * Removes media URLs from text to avoid duplication in display
 */
std: removeMediaUrlsFromText(const std:& text, const std::vector<MediaInfo>& mediaInfos);

} // namespace elizaos
