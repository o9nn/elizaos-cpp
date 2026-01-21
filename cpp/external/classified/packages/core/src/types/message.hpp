#include "service.hpp"
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



struct MessageParticipant {
    UUID id;
    std::string name;
    std::optional<std::string> username;
    std::optional<std::string> avatar;
    std::optional<'online' | 'offline' | 'away' | 'busy'> status;
};

struct MessageAttachment {
    UUID id;
    std::string filename;
    std::string url;
    std::string mimeType;
    double size;
    std::optional<double> width;
    std::optional<double> height;
    std::optional<double> duration;
    std::optional<std::string> thumbnail;
};

struct MessageReaction {
    std::string emoji;
    double count;
    std::vector<UUID> users;
    bool hasReacted;
};

struct MessageReference {
    UUID messageId;
    UUID channelId;
    'reply' | 'forward' | 'quote' type;
};

struct MessageContent {
    std::optional<std::string> text;
    std::optional<std::string> html;
    std::optional<std::string> markdown;
    std::optional<std::vector<MessageAttachment>> attachments;
    std::optional<std::vector<MessageReaction>> reactions;
    std::optional<MessageReference> reference;
    std::optional<std::vector<UUID>> mentions;
    std::optional<Array<{> embeds;
    std::optional<std::string> title;
    std::optional<std::string> description;
    std::optional<std::string> url;
    std::optional<std::string> image;
    std::optional<Array<{> fields;
    std::string name;
    std::string value;
    std::optional<bool> inline;
};

struct MessageInfo {
    UUID id;
    UUID channelId;
    UUID senderId;
    MessageContent content;
    Date timestamp;
    std::optional<Date> edited;
    std::optional<Date> deleted;
    std::optional<bool> pinned;
    std::optional<{> thread;
    UUID id;
    double messageCount;
    std::vector<UUID> participants;
    Date lastMessageAt;
};

struct MessageSendOptions {
    std::optional<UUID> replyTo;
    std::optional<bool> ephemeral;
    std::optional<bool> silent;
    std::optional<Date> scheduled;
    std::optional<UUID> thread;
    std::optional<std::string> nonce;
};

struct MessageSearchOptions {
    std::optional<std::string> query;
    std::optional<UUID> channelId;
    std::optional<UUID> senderId;
    std::optional<Date> before;
    std::optional<Date> after;
    std::optional<double> limit;
    std::optional<double> offset;
    std::optional<bool> hasAttachments;
    std::optional<bool> pinned;
    std::optional<UUID> mentions;
};

struct MessageChannel {
    UUID id;
    std::string name;
    'text' | 'voice' | 'dm' | 'group' | 'announcement' | 'thread' type;
    std::optional<std::string> description;
    std::optional<std::vector<MessageParticipant>> participants;
    std::optional<{> permissions;
    bool canSend;
    bool canRead;
    bool canDelete;
    bool canPin;
    bool canManage;
    std::optional<Date> lastMessageAt;
    std::optional<double> messageCount;
    std::optional<double> unreadCount;
};

/**
 * Interface for messaging services
 */

  /**
   * Send a message to a channel
   * @param channelId - Channel ID
   * @param content - Message content
   * @param options - Send options
   * @returns Promise resolving to message ID
   */

  /**
   * Get messages from a channel
   * @param channelId - Channel ID
   * @param options - Search options
   * @returns Promise resolving to array of messages
   */

  /**
   * Get a specific message by ID
   * @param messageId - Message ID
   * @returns Promise resolving to message
   */

  /**
   * Edit a message
   * @param messageId - Message ID
   * @param content - New message content
   * @returns Promise resolving when edit completes
   */

  /**
   * Delete a message
   * @param messageId - Message ID
   * @returns Promise resolving when deletion completes
   */

  /**
   * Add a reaction to a message
   * @param messageId - Message ID
   * @param emoji - Reaction emoji
   * @returns Promise resolving when reaction is added
   */

  /**
   * Remove a reaction from a message
   * @param messageId - Message ID
   * @param emoji - Reaction emoji
   * @returns Promise resolving when reaction is removed
   */

  /**
   * Pin a message
   * @param messageId - Message ID
   * @returns Promise resolving when message is pinned
   */

  /**
   * Unpin a message
   * @param messageId - Message ID
   * @returns Promise resolving when message is unpinned
   */

  /**
   * Get available channels
   * @returns Promise resolving to array of channels
   */

  /**
   * Get channel information
   * @param channelId - Channel ID
   * @returns Promise resolving to channel info
   */

  /**
   * Create a new channel
   * @param name - Channel name
   * @param type - Channel type
   * @param options - Channel options
   * @returns Promise resolving to new channel ID
   */

  /**
   * Search messages across channels
   * @param query - Search query
   * @param options - Search options
   * @returns Promise resolving to search results
   */

} // namespace elizaos
