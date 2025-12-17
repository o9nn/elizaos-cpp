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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Represents a UUID, which is a universally unique identifier conforming to the UUID standard.
 */
using UUID = `${string}-${string}-${string}-${string}-${string}`;

/**
 * Represents a media object, such as an image, video, or other file, with various properties.
 */
using Media = {

/**
 * Represents the content of a message, including its main text (`content`), any associated action (`action`), and the source of the content (`source`), if applicable.
 */
struct Content {
    std::string text;
    std::optional<std::string> action;
    std::optional<std::string> source;
    std::optional<std::string> url;
    std::optional<UUID> inReplyTo;
    std::optional<std::vector<Media>> attachments;
};

/**
 * Represents an example of a message, typically used for demonstrating or testing purposes, including optional content and action.
 */
struct MessageExample {
    std::string user;
    Content content;
};

/**
 * Represents a character, which can be used for an LLM agent.
 */
using Character = {
} // namespace elizaos
