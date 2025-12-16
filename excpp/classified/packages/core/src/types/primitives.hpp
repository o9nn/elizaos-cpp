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
 * Defines a custom type UUID representing a universally unique identifier
 */
using UUID = `${string}-${string}-${string}-${string}-${string}`;

/**
 * Helper -[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$/i.test(id)) {
    throw new Error(`Invalid UUID format: ${id}`);
  }
  return id as UUID;
}

/**
 * Represents the content of a memory, message, or other information
 */
struct Content {
    std::optional<std::string> thought;
    std::optional<std::string> text;
    std::optional<std::vector<std::string>> actions;
    std::optional<std::vector<std::string>> providers;
    std::optional<std::string> source;
    std::optional<std::string> target;
    std::optional<std::string> url;
    std::optional<UUID> inReplyTo;
    std::optional<std::vector<Media>> attachments;
    std::optional<std::string> channelType;
};


/**
 * Represents a media attachment
 */
using Media = {
  /** Unique identifier */
  id: string;

  /** Media URL */
  url: string;

  /** Media title */
  title?: string;

  /** Media source */
  source?: string;

  /** Media description */
  description?: string;

  /** Text content */
  text?: string;

  /** Content type */
  contentType?: ContentType;
};

enum ContentType {
  IMAGE = 'image',
  VIDEO = 'video',
  AUDIO = 'audio',
  DOCUMENT = 'document',
  LINK = 'link',
}

/**
 * A generic type for metadata objects, allowing for arbitrary key-value pairs.
 * This encourages consumers to perform type checking or casting.
 */
using Metadata = std::unordered_map<std::string, unknown>;

} // namespace elizaos
