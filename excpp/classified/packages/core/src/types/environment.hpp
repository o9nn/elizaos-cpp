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

import type { Metadata, UUID } from './primitives';

struct Component {
    UUID id;
    UUID entityId;
    UUID agentId;
    UUID roomId;
    UUID worldId;
    UUID sourceEntityId;
    std::string type;
    double createdAt;
    Metadata data;
};


/**
 * Represents a user account
 */
struct Entity {
    std::optional<UUID> id;
    std::vector<std::string> names;
    std::optional<Metadata> metadata;
    UUID agentId;
    std::optional<std::vector<Component>> components;
};


/**
 * Defines roles within a system, typically for access control or permissions, often within a `World`.
 * - `OWNER`: Represents the highest level of control, typically the creator or primary administrator.
 * - `ADMIN`: Represents administrative privileges, usually a subset of owner capabilities.
 * - `NONE`: Indicates no specific role or default, minimal permissions.
 * These roles are often used in `World.metadata.roles` to assign roles to entities.
 */
enum Role {
  OWNER = 'OWNER',
  ADMIN = 'ADMIN',
  NONE = 'NONE',
}

using World = {
  id: UUID;
  name?: string;
  agentId: UUID;
  serverId: string;
  metadata?: {
    ownership?: {
      ownerId: string;
    };
    roles?: {
      [entityId: UUID]: Role;
    };
    [key: string]: unknown;
  };
};

enum ChannelType {
  SELF = 'SELF', // Messages to self
  DM = 'DM', // Direct messages between two participants
  GROUP = 'GROUP', // Group messages with multiple participants
  VOICE_DM = 'VOICE_DM', // Voice direct messages
  VOICE_GROUP = 'VOICE_GROUP', // Voice channels with multiple participants
  FEED = 'FEED', // Social media feed
  THREAD = 'THREAD', // Threaded conversation
  WORLD = 'WORLD', // World channel
  FORUM = 'FORUM', // Forum discussion
  // Legacy types - kept for backward compatibility but should be replaced
  API = 'API', // @deprecated - Use DM or GROUP instead
}

using Room = {
  id: UUID;
  name?: string;
  agentId?: UUID;
  source: string;
  type: ChannelType;
  channelId?: string;
  serverId?: string;
  worldId?: UUID;
  metadata?: Metadata;
};

using RoomMetadata = {
  [key: string]: unknown;
};

/**
 * Room participant with account details
 */
struct Participant {
    UUID id;
    Entity entity;
};


/**
 * Represents a relationship between users
 */
struct Relationship {
    UUID id;
    UUID sourceEntityId;
    UUID targetEntityId;
    UUID agentId;
    std::vector<std::string> tags;
    Metadata metadata;
    std::optional<std::string> createdAt;
};


} // namespace elizaos
