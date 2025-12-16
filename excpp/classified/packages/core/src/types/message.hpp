#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "service.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct MessageParticipant {
    UUID id;
    std::string name;
    std::optional<std::string> username;
    std::optional<std::string> avatar;
    std::optional<std::variant<'online', 'offline', 'away', 'busy'>> status;
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
    std::variant<'reply', 'forward', 'quote'> type;
};

struct MessageContent {
    std::optional<std::string> text;
    std::optional<std::string> html;
    std::optional<std::string> markdown;
    std::optional<std::vector<MessageAttachment>> attachments;
    std::optional<std::vector<MessageReaction>> reactions;
    std::optional<MessageReference> reference;
    std::optional<std::vector<UUID>> mentions;
    std::optional<std::string> title;
    std::optional<std::string> description;
    std::optional<std::string> url;
    std::optional<std::string> image;
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
    std::variant<'text', 'voice', 'dm', 'group', 'announcement', 'thread'> type;
    std::optional<std::string> description;
    std::optional<std::vector<MessageParticipant>> participants;
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
    std::future<UUID> sendMessage(UUID channelId, MessageContent content, std::optional<MessageSendOptions> options);


} // namespace elizaos
