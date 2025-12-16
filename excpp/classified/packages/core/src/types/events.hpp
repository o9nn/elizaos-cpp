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

import type { HandlerCallback } from './components';
import type { Entity, Room, World } from './environment';
import type { Memory } from './memory';
import type { ModelTypeName } from './model';
import type { Metadata, UUID } from './primitives';
import type { IAgentRuntime } from './runtime';

/**
 * Standard event types across all platforms
 */
enum EventType {
  // World events
  WORLD_JOINED = 'WORLD_JOINED',
  WORLD_CONNECTED = 'WORLD_CONNECTED',
  WORLD_LEFT = 'WORLD_LEFT',

  // Entity events
  ENTITY_JOINED = 'ENTITY_JOINED',
  ENTITY_LEFT = 'ENTITY_LEFT',
  ENTITY_UPDATED = 'ENTITY_UPDATED',

  // Room events
  ROOM_JOINED = 'ROOM_JOINED',
  ROOM_LEFT = 'ROOM_LEFT',

  // Message events
  MESSAGE_RECEIVED = 'MESSAGE_RECEIVED',
  MESSAGE_SENT = 'MESSAGE_SENT',
  MESSAGE_DELETED = 'MESSAGE_DELETED',

  // Channel events
  CHANNEL_CLEARED = 'CHANNEL_CLEARED',

  // Voice events
  VOICE_MESSAGE_RECEIVED = 'VOICE_MESSAGE_RECEIVED',
  VOICE_MESSAGE_SENT = 'VOICE_MESSAGE_SENT',

  // Interaction events
  REACTION_RECEIVED = 'REACTION_RECEIVED',
  POST_GENERATED = 'POST_GENERATED',
  INTERACTION_RECEIVED = 'INTERACTION_RECEIVED',

  // Run events
  RUN_STARTED = 'RUN_STARTED',
  RUN_ENDED = 'RUN_ENDED',
  RUN_TIMEOUT = 'RUN_TIMEOUT',

  // Action events
  ACTION_STARTED = 'ACTION_STARTED',
  ACTION_COMPLETED = 'ACTION_COMPLETED',

  // Evaluator events
  EVALUATOR_STARTED = 'EVALUATOR_STARTED',
  EVALUATOR_COMPLETED = 'EVALUATOR_COMPLETED',

  // Model events
  MODEL_USED = 'MODEL_USED',

  // Progression events
  CAPABILITY_USED = 'CAPABILITY_USED',
  AGENT_NAMED = 'AGENT_NAMED',
  SHELL_COMMAND_EXECUTED = 'SHELL_COMMAND_EXECUTED',
  GOAL_CREATED = 'GOAL_CREATED',
  TODO_CREATED = 'TODO_CREATED',
  FORM_SUBMITTED = 'FORM_SUBMITTED',
  BROWSER_ACTION_PERFORMED = 'BROWSER_ACTION_PERFORMED',
  VISION_ACTION_PERFORMED = 'VISION_ACTION_PERFORMED',
  MICROPHONE_USED = 'MICROPHONE_USED',
}

/**
 * Platform-specific event type prefix
 */
enum PlatformPrefix {
  DISCORD = 'DISCORD',
  TELEGRAM = 'TELEGRAM',
  TWITTER = 'TWITTER',
}

/**
 * Base payload interface for all events
 */
struct EventPayload {
    IAgentRuntime runtime;
    std::string source;
    std::optional<() => void> onComplete;
};


/**
 * Payload for world-related events
 */
interface WorldPayload extends EventPayload {
  world: World;
  rooms: Room[];
  entities: Entity[];
}

/**
 * Payload for entity-related events
 */
interface EntityPayload extends EventPayload {
  entityId: UUID;
  worldId?: UUID;
  roomId?: UUID;
  metadata?: {
    orginalId: string;
    username: string;
    displayName?: string;
    [key: string]: any;
  };
}

/**
 * Payload for reaction-related events
 */
interface MessagePayload extends EventPayload {
  message: Memory;
  callback?: HandlerCallback;
  onComplete?: () => void;
}

/**
 * Payload for channel cleared events
 */
interface ChannelClearedPayload extends EventPayload {
  roomId: UUID;
  channelId: string;
  memoryCount: number;
}

/**
 * Payload for events that are invoked without a message
 */
interface InvokePayload extends EventPayload {
  worldId: UUID;
  userId: string;
  roomId: UUID;
  callback?: HandlerCallback;
  source: string;
}

/**
 * Run event payload type
 */
interface RunEventPayload extends EventPayload {
  runId: UUID;
  messageId: UUID;
  roomId: UUID;
  entityId: UUID;
  startTime: number;
  status: 'started' | 'completed' | 'timeout';
  endTime?: number;
  duration?: number;
  error?: string;
}

/**
 * Action event payload type
 */
interface ActionEventPayload extends EventPayload {
  actionId: UUID;
  actionName: string;
  startTime?: number;
  completed?: boolean;
  error?: Error;
}

/**
 * Evaluator event payload type
 */
interface EvaluatorEventPayload extends EventPayload {
  evaluatorId: UUID;
  evaluatorName: string;
  startTime?: number;
  completed?: boolean;
  error?: Error;
}

/**
 * Model event payload type
 */
interface ModelEventPayload extends EventPayload {
  provider: string;
  type: ModelTypeName;
  prompt: string;
  tokens?: {
    prompt: number;
    completion: number;
    total: number;
  };
}

using MessageReceivedHandlerParams = {
  runtime: IAgentRuntime;
  message: Memory;
  callback: HandlerCallback;
  onComplete?: () => void;
};

/**
 * Maps event types to their corresponding payload types
 */
struct EventPayloadMap {
};


/**
 * Event handler function type
 */
type EventHandler<T extends keyof EventPayloadMap> = (
  payload: EventPayloadMap[T]
) => Promise<void>;

/**
 * Defines a more specific type for event handlers, expecting an `Metadata`.
 * This aims to improve upon generic 'any' type handlers, providing a clearer contract
 * for functions that respond to events emitted within the agent runtime (see `emitEvent` in `AgentRuntime`).
 * Handlers can be synchronous or asynchronous.
 */
using TypedEventHandler = std::variant<(data: Metadata) => Promise<void>, void>;

} // namespace elizaos
