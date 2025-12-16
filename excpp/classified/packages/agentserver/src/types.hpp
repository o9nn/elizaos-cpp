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

import type { UUID, ChannelType } from '@elizaos/core';

struct MessageServer {
    UUID; // global serverId id;
    std::string name;
    string; // e.g., 'eliza_native', 'discord_guild' sourceType;
    std::optional<string; // original platform ID if applicable> sourceId;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
    Date createdAt;
    Date updatedAt;
};


struct MessageChannel {
    UUID; // global channelId id;
    UUID; // FK to MessageServer.id serverId;
    std::string name;
    ChannelType; // Use the enum from @elizaos/core type;
    std::optional<std::string> sourceType;
    std::optional<std::string> sourceId;
    std::optional<std::string> topic;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
    Date createdAt;
    Date updatedAt;
};


struct CentralRootMessage {
    UUID id;
    UUID; // FK to MessageChannel.id channelId;
    UUID; // Identifier for the author (could be an agent's runtime.agentId or a dedicated central user ID) authorId;
    std::string content;
    std::optional<std::any> rawMessage;
    std::optional<UUID; // FK to CentralRootMessage.id (self-reference)> inReplyToRootMessageId;
    std::optional<std::string> sourceType;
    std::optional<string; // Original message ID from the source platform> sourceId;
    Date createdAt;
    Date updatedAt;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
};


// This is what goes on the internal bus and often what APIs might return for a "full" message
struct MessageServiceStructure {
    UUID; // CentralRootMessage.id id;
    UUID; // MessageChannel.id channel_id;
    UUID; // MessageServer.id server_id;
    UUID author_id;
    std::optional<std::string> author_display_name;
    std::string content;
    std::optional<std::any> raw_message;
    std::optional<std::string> source_id;
    std::optional<std::string> source_type;
    std::optional<UUID> in_reply_to_message_id;
    number; // timestamp ms created_at;
    std::optional<std::any> metadata;
};


// Extend the core service types with message bus service
declare module '@elizaos/core' {
  struct ServiceTypeRegistry {
    'MESSAGE_BUS_SERVICE' MESSAGE_BUS_SERVICE;
};

}

// Export service type constant
const AgentServerServiceType = {
  MESSAGE_BUS_SERVICE: 'MESSAGE_BUS_SERVICE' as const,
} satisfies Partial<import('@elizaos/core').ServiceTypeRegistry>;

} // namespace elizaos
