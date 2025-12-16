#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Utility functions for parsing and handling media URLs in chat messages
 */

struct MediaInfo {
    std::string url;
    'image' | 'video' | 'unknown' type;
    bool isEmbed;
};


// Common image extensions
const IMAGE_EXTENSIONS = new Set([
  'jpg',
  'jpeg',
  'png',
  'gif',
  'webp',
  'svg',
  'bmp',
  'ico',
  'tiff',
  'avif',
]);

// Common video extensions
const VIDEO_EXTENSIONS = new Set(['mp4', 'webm', 'mov', 'avi', 'mkv', 'flv', 'wmv', 'm4v', 'ogv']);

// Video platforms that support embedding
const VIDEO_PLATFORMS = {
  youtube: {
    patterns: [
      /(?:https?:\/\/)?(?:www\.)?(?:youtube\.com\/watch\?v=|youtu\.be\/)([a-zA-Z0-9_-]{11})/,
      /(?:https?:\/\/)?(?:www\.)?youtube\.com\/embed\/([a-zA-Z0-9_-]{11})/,
    ],
    getEmbedUrl: (id: string) => `https://www.youtube.com/embed/${id}`,
  },
  vimeo: {
    patterns: [/(?:https?:\/\/)?(?:www\.)?vimeo\.com\/(\d+)/],
    getEmbedUrl: (id: string) => `https://player.vimeo.com/video/${id}`,
  },
};

/**
 * Extracts the file extension from a URL
 */
 catch {
    return '';
  }
}

/**
 * Checks if a URL points to an image
 */


/**
 * Checks if a URL points to a video file
 */


/**
 * Checks if a URL is from a supported video platform
 */
 | null {
  for (const [platform, config] of Object.entries(VIDEO_PLATFORMS)) {
    for (const pattern of config.patterns) {
      const match = url.match(pattern);
      if (match && match[1]) {
        return {
          platform,
          id: match[1],
          embedUrl: config.getEmbedUrl(match[1]),
        };
      }
    }
  }
  return null;
}

/**
 * Parses URLs from text and identifies media types
 */
;
      }

      if (isVideoFileUrl(cleanUrl)) {
        return {
          url: cleanUrl,
          type: 'video' as const,
          isEmbed: false,
        };
      }

      const platformInfo = getVideoPlatformInfo(cleanUrl);
      if (platformInfo) {
        return {
          url: platformInfo.embedUrl,
          type: 'video' as const,
          isEmbed: true,
        };
      }

      return {
        url: cleanUrl,
        type: 'unknown' as const,
        isEmbed: false,
      };
    })
    .filter((media) => media.type !== 'unknown');
}

/**
 * Removes media URLs from text to avoid duplication in display
 */

      }
    } else {
      cleanText = cleanText.replace(media.url, '').trim();
    }
  });

  return cleanText;
}

} // namespace elizaos
