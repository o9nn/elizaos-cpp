#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Standard event types across all platforms
 */
enum EventType {
  // World events

  // Entity events

  // Room events

  // Message events

  // Channel events

  // Voice events

  // Interaction events

  // Run events

  // Action events

  // Evaluator events

  // Model events

  // Progression events
}

/**
 * Platform-specific event type prefix
 */
enum PlatformPrefix {
}

/**
 * Base payload interface for all events
 */
struct EventPayload {
    IAgentRuntime runtime;
    std::string source;
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
using TypedEventHandler = std::variant<std::function<std::future<void>(Metadata)>, void>;

} // namespace elizaos
