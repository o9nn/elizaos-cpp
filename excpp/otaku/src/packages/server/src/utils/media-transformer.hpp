#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Transform local file paths to API URLs for web clients
 */

// Path configurations mapping
// Pattern matches any ID format (not just UUIDs) to support all valid IDs
// The pattern captures the ID (first path segment) and filename (second path segment)

// Check if path is an external URL (http, https, blob, data, file, ipfs, s3, gs, etc.)

/**
 * Transform a local file path to an API URL
 */
std::string transformPathToApiUrl(const std::string& filePath);

/**
 * Convert local file paths to API URLs for attachments
 */
AttachmentInput attachmentsToApiUrls(AttachmentInput attachments);

/**
 * Transform attachments in message content and metadata to API URLs
 */
MessageWithAttachments transformMessageAttachments(MessageWithAttachments message);

} // namespace elizaos
