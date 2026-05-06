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

/**
 * Payload for entity-related events
 */

/**
 * Payload for reaction-related events
 */

/**
 * Payload for channel cleared events
 */

/**
 * Payload for events that are invoked without a message
 */

/**
 * Run event payload type
 */

/**
 * Action event payload type
 */

/**
 * Evaluator event payload type
 */

/**
 * Model event payload type
 */

using MessageReceivedHandlerParams = {

/**
 * Maps event types to their corresponding payload types
 */
struct EventPayloadMap {
};

/**
 * Event handler function type
 */

/**
 * Defines a more specific type for event handlers, expecting an `Metadata`.
 * This aims to improve upon generic 'any' type handlers, providing a clearer contract
 * for functions that respond to events emitted within the agent runtime (see `emitEvent` in `AgentRuntime`).
 * Handlers can be synchronous or asynchronous.
 */
using TypedEventHandler = std::variant<(data: Metadata) => Promise<void>, void>;

} // namespace elizaos
