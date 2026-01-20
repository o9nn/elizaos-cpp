#include "..runtime.hpp"
#include "..types.hpp"
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


// Import types with the 'type' keyword

class Semaphore {
  private _semphonre;
  constructor(count: number) {
    this._semphonre = new coreSemaphore(count);
  }

/**
 * Represents the runtime environment for an agent.
 * @class
 * @implements { IAgentRuntime }
 * @property { number } #conversationLength - The maximum length of a conversation.
 * @property { UUID } agentId - The unique identifier for the agent.
 * @property { Character } character - The character associated with the agent.
 * @property { IDatabaseAdapter } adapter - The adapter for interacting with the database.
 * @property {Action[]} actions - The list of actions available to the agent.
 * @property {Evaluator[]} evaluators - The list of evaluators for decision making.
 * @property {Provider[]} providers - The list of providers for external services.
 * @property {Plugin[]} plugins - The list of plugins to extend functionality.
 */
class AgentRuntime implements IAgentRuntime {
  private _runtime;

  get services(): Map<ServiceTypeName, Service> {
    return this._runtime.services as any;
  }

    // If _runtime.events is already a Map, just cast it

    // If it's an object that needs to be converted to a Map

    // Convert object to Map if needed

  /**
   * Registers a plugin with the runtime and initializes its components
   * @param plugin The plugin to register
   */
    // Wrap the plugin to ensure it receives the v2 runtime instance
            // Pass the v2 runtime instance (this) instead of the core runtime

  /**
   * Get the number of messages that are kept in the conversation buffer.
   * @returns The number of recent messages to be kept in memory.
   */

  /**
   * Register a provider for the agent to use.
   * @param provider The provider to register.
   */
    // Wrap the provider to ensure it receives the v2 runtime instance
        // Pass the v2 runtime instance (this) instead of the core runtime

  /**
   * Register an action for the agent to perform.
   * @param action The action to register.
   */
    // Wrap the action to ensure its handler receives the v2 runtime instance
        // Pass the v2 runtime instance (this) instead of the core runtime

  /**
   * Register an evaluator to assess and guide the agent's responses.
   * @param evaluator The evaluator to register.
   */

  /**
   * Process the actions of a message.
   * @param message The message to process.
   * @param responses The array of response memories to process actions from.
   * @param state Optional state object for the action processing.
   * @param callback Optional callback handler for action results.
   */

  /**
   * Evaluate the message and state using the registered evaluators.
   * @param message The message to evaluate.
   * @param state The state of the agent.
   * @param didRespond Whether the agent responded to the message.~
   * @param callback The handler callback
   * @returns The results of the evaluation.
   */

  /**
   * Ensures a participant is added to a room, checking that the entity exists first
   */

  /**
   * Ensure the existence of a world.
   */

  /**
   * Ensure the existence of a room between the agent and a user. If no room exists, a new room is created and the user
   * and agent are added as participants. The room ID is returned.
   * @param entityId - The user ID to create a room with.
   * @returns The room ID of the room between the agent and the user.
   * @throws An error if the room cannot be created.
   */

  /**
   * Composes the agent's state by gathering data from enabled providers.
   * @param message - The message to use as context for state composition
   * @param includeList - Optional list of provider names to include, filtering out all others
   * @param onlyInclude - Whether to only include the specified providers
   * @param skipCache - Whether to skip the cache
   * @returns A State object containing provider data, values, and text
   */

    // Wrap the handler to ensure it receives the v2 runtime instance
      // Pass the v2 runtime instance (this) instead of the core runtime
      return handler(this, params);

  /**
   * Use a model with strongly typed parameters and return values based on model type
   * @template T - The model type to use
   * @template R - The expected return type, defaults to the type defined in ModelResultMap[T]
   * @param {T} modelType - The type of model to use
   * @param {ModelParamsMap[T] | any} params - The parameters for the model, typed based on model type
   * @returns {Promise<R>} - The model result, typed based on the provided generic type parameter
   */

  /**
   * Get a task worker by name
   */

  // Implement database adapter methods

  // Event emitter methods

  /**
   * Register a message send handler for a specific source
   * @param source - The source identifier (e.g., 'discord', 'telegram')
   * @param handler - The handler function to send messages
   */

  /**
   * Send a message to a specific target
   * @param target - The target information including source and channel/user ID
   * @param content - The message content to send
   */

  // Run tracking methods


} // namespace elizaos
