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

;
import type { UUID } from './primitives';

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
abstract class IMessageService extends Service {
  static override readonly serviceType = ServiceType.MESSAGE;

  public readonly capabilityDescription = 'Message sending, receiving, and management capabilities';

  /**
   * Send a message to a channel
   * @param channelId - Channel ID
   * @param content - Message content
   * @param options - Send options
   * @returns Promise resolving to message ID
   */
  abstract sendMessage(
    channelId: UUID,
    content: MessageContent,
    options?: MessageSendOptions
  ): Promise<UUID>;

  /**
   * Get messages from a channel
   * @param channelId - Channel ID
   * @param options - Search options
   * @returns Promise resolving to array of messages
   */
  abstract getMessages(channelId: UUID, options?: MessageSearchOptions): Promise<MessageInfo[]>;

  /**
   * Get a specific message by ID
   * @param messageId - Message ID
   * @returns Promise resolving to message
   */
  abstract getMessage(messageId: UUID): Promise<MessageInfo>;

  /**
   * Edit a message
   * @param messageId - Message ID
   * @param content - New message content
   * @returns Promise resolving when edit completes
   */
  abstract editMessage(messageId: UUID, content: MessageContent): Promise<void>;

  /**
   * Delete a message
   * @param messageId - Message ID
   * @returns Promise resolving when deletion completes
   */
  abstract deleteMessage(messageId: UUID): Promise<void>;

  /**
   * Add a reaction to a message
   * @param messageId - Message ID
   * @param emoji - Reaction emoji
   * @returns Promise resolving when reaction is added
   */
  abstract addReaction(messageId: UUID, emoji: string): Promise<void>;

  /**
   * Remove a reaction from a message
   * @param messageId - Message ID
   * @param emoji - Reaction emoji
   * @returns Promise resolving when reaction is removed
   */
  abstract removeReaction(messageId: UUID, emoji: string): Promise<void>;

  /**
   * Pin a message
   * @param messageId - Message ID
   * @returns Promise resolving when message is pinned
   */
  abstract pinMessage(messageId: UUID): Promise<void>;

  /**
   * Unpin a message
   * @param messageId - Message ID
   * @returns Promise resolving when message is unpinned
   */
  abstract unpinMessage(messageId: UUID): Promise<void>;

  /**
   * Get available channels
   * @returns Promise resolving to array of channels
   */
  abstract getChannels(): Promise<MessageChannel[]>;

  /**
   * Get channel information
   * @param channelId - Channel ID
   * @returns Promise resolving to channel info
   */
  abstract getChannel(channelId: UUID): Promise<MessageChannel>;

  /**
   * Create a new channel
   * @param name - Channel name
   * @param type - Channel type
   * @param options - Channel options
   * @returns Promise resolving to new channel ID
   */
  abstract createChannel(
    name: string,
    type: MessageChannel['type'],
    options?: {
      description?: string;
      participants?: UUID[];
      private?: boolean;
    }
  ): Promise<UUID>;

  /**
   * Search messages across channels
   * @param query - Search query
   * @param options - Search options
   * @returns Promise resolving to search results
   */
  abstract searchMessages(query: string, options?: MessageSearchOptions): Promise<MessageInfo[]>;
}

} // namespace elizaos
