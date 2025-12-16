#include "elizaos/core.hpp"
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
 * Transform local file paths to API URLs for web clients
 */

;
;
import type {
  AttachmentInput,
  MessageWithAttachments,
  MessageContentWithAttachments,
} from '../types';

// Path configurations mapping
// Pattern matches any ID format (not just UUIDs) to support all valid IDs
// The pattern captures the ID (first path segment) and filename (second path segment)
const ID_PATTERN = /^([^/\\]+)[/\\]([^/\\]+)$/;

const PATH_CONFIGS = [
  {
    getPath: getGeneratedDir,
    apiPrefix: '/media/generated/',
    pattern: ID_PATTERN,
  },
  {
    getPath: getUploadsAgentsDir,
    apiPrefix: '/media/uploads/agents/',
    pattern: ID_PATTERN,
  },
  {
    getPath: getUploadsChannelsDir,
    apiPrefix: '/media/uploads/channels/',
    pattern: ID_PATTERN,
  },
];

// Check if path is an external URL (http, https, blob, data, file, ipfs, s3, gs, etc.)
const isExternalUrl = (p: string) => /^(?:https?:|blob:|data:|file:|ipfs:|s3:|gs:)/i.test(p);

/**
 * Transform a local file path to an API URL
 */


  // Normalize path for comparison
  const normalizedPath = filePath.replace(/\\/g, '/');

  // Check each path configuration
  for (const config of PATH_CONFIGS) {
    const configPathRaw = config.getPath().replace(/\\/g, '/');
    const configPath = configPathRaw.endsWith('/') ? configPathRaw : `${configPathRaw}/`;

    // Use strict boundary-aware startsWith check to prevent path traversal
    if (normalizedPath === configPathRaw || normalizedPath.startsWith(configPath)) {
      const relative =
        normalizedPath === configPathRaw ? '' : normalizedPath.slice(configPath.length);

      // Only process if we have a valid relative path
      if (relative) {
        const match = relative.match(config.pattern);
        if (match) {
          const [, id, filename] = match;
          return `${config.apiPrefix}${encodeURIComponent(id)}${'/'}${encodeURIComponent(filename)}`;
        }
      }
    }
  }

  return filePath;
}

/**
 * Convert local file paths to API URLs for attachments
 */

      if (attachment?.url) {
        return { ...attachment, url: transformPathToApiUrl(attachment.url) };
      }
      return attachment;
    });
  }

  // Single attachment
  if (typeof attachments === 'string') {
    return transformPathToApiUrl(attachments);
  }
  if (attachments?.url) {
    return { ...attachments, url: transformPathToApiUrl(attachments.url) };
  }
  return attachments;
}

/**
 * Transform attachments in message content and metadata to API URLs
 */


  // Transform attachments in content
  if (message.content && typeof message.content === 'object' && 'attachments' in message.content) {
    const content = message.content as MessageContentWithAttachments;
    if (content.attachments) {
      content.attachments = attachmentsToApiUrls(content.attachments);
    }
  }

  // Transform attachments in metadata
  if (message.metadata?.attachments) {
    message.metadata.attachments = attachmentsToApiUrls(message.metadata.attachments);
  }

  return message;
}

} // namespace elizaos
