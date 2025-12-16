#include "elizaos/core.hpp"
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

import type { ElizaOS, UUID } from '@elizaos/core';
;

/**
 * Validates and retrieves an agent runtime from the agents map
 */
const getRuntime = (elizaOS: ElizaOS, agentId: UUID) => {
  const runtime = elizaOS.getAgent(agentId);
  if (!runtime) {
    throw new Error(`Agent not found: ${agentId}`);
  }
  return runtime;
};

/**
 * Validates a UUID parameter and returns it as UUID type or null if invalid
 */
const validateAgentId = (agentId: string): UUID | null => {
  return validateUuid(agentId);
};

/**
 * Validates a room ID parameter
 */
const validateRoomId = (roomId: string): UUID | null => {
  return validateUuid(roomId);
};

/**
 * Enhanced channel ID validation with security logging
 * Validates a channel ID parameter with additional security checks
 */
const validateChannelId = (channelId: string, clientIp?: string): UUID | null => {
  // Basic UUID validation
  const validatedUuid = validateUuid(channelId);

  if (!validatedUuid) {
    // Log invalid channel ID attempts for security monitoring
    if (clientIp) {
      logger.warn(`[SECURITY] Invalid channel ID attempted from ${clientIp}: ${channelId}`);
    }
    return null;
  }

  // Additional security check: ensure channel ID doesn't contain suspicious patterns
  const suspiciousPatterns = ['..', '<', '>', '"', "'", '\\', '/'];
  const hasSuspiciousPattern = suspiciousPatterns.some((pattern) => channelId.includes(pattern));

  if (hasSuspiciousPattern) {
    if (clientIp) {
      logger.warn(`[SECURITY] Suspicious channel ID pattern from ${clientIp}: ${channelId}`);
    }
    return null;
  }

  return validatedUuid;
};

/**
 * Validates a memory ID parameter
 */
const validateMemoryId = (memoryId: string): UUID | null => {
  return validateUuid(memoryId);
};

/**
 * Validates a world ID parameter
 */
const validateWorldId = (worldId: string): UUID | null => {
  return validateUuid(worldId);
};

} // namespace elizaos
