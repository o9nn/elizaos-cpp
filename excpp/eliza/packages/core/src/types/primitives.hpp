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
 * Helper function to safely cast a string to strongly typed UUID
 * @param id The string UUID to validate and cast
 * @returns The same UUID with branded type information
 */
UUID asUUID(const std::string& id);

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

  /** Media URL */

  /** Media title */

  /** Media source */

  /** Media description */

  /** Text content */

  /** Content type */

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
