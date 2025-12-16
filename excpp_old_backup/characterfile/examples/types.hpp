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
    string; // The main text content of the message. text;
    std::optional<string; // An optional action associated with the message, indicating a specific behavior or response required.> action;
    std::optional<string; // The source of the content, if applicable, such as a reference or origin.> source;
    std::optional<string; // The actual URL of the message or post, i.e. tweet URL or message link in discord> url;
    std::optional<UUID; // If this is a message in a thread, or a reply, store this> inReplyTo;
    std::optional<std::vector<Media>> attachments;
};

/**
 * Represents an example of a message, typically used for demonstrating or testing purposes, including optional content and action.
 */
struct MessageExample {
    string; // The user associated with the message example. If {{user1}}, {{user2}}, etc. will be replaced with random names user;

/**
 * Represents a character, which can be used for an LLM agent.
 */
using Character = {
} // namespace elizaos
