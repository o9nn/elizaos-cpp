#include ".v2.hpp"
#include "messages.hpp"
#include "types.hpp"
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



class AgentRuntime implements IAgentRuntime {
  private _runtime;

  get agentId(): UUID {
    return this._runtime.agentId;
  }

    // WRITE ME
    // WRITE ME

  /**
   * Creates an instance of AgentRuntime.
   * @param opts - The options for configuring the AgentRuntime.
   * @param opts.conversationLength - The number of messages to hold in the recent message cache.
   * @param opts.token - The JWT token, can be a JWT token if outside worker, or an OpenAI token if inside worker.
   * @param opts.serverUrl - The URL of the worker.
   * @param opts.actions - Optional custom actions.
   * @param opts.evaluators - Optional custom evaluators.
   * @param opts.services - Optional custom services.
   * @param opts.memoryManagers - Optional custom memory managers.
   * @param opts.providers - Optional context providers.
   * @param opts.model - The model to use for generateText.
   * @param opts.embeddingModel - The model to use for embedding.
   * @param opts.agentId - Optional ID of the agent.
   * @param opts.databaseAdapter - The database adapter used for interacting with the database.
   * @param opts.fetch - Custom fetch function to use for making requests.
   */

    // verifiableInferenceAdapter?: IVerifiableInferenceAdapter;

  //private async initializeDatabase() {}

  /**
   * Get the number of messages that are kept in the conversation buffer.
   * @returns The number of recent messages to be kept in memory.
   */

  /**
   * Register an action for the agent to perform.
   * @param action The action to register.
   */

  /**
   * Register an evaluator to assess and guide the agent's responses.
   * @param evaluator The evaluator to register.
   */

  /**
   * Register a context provider to provide context for message generation.
   * @param provider The context provider to register.
   */

  /**
   * Register an adapter for the agent to use.
   * @param adapter The adapter to register.
   */
    // WRITE ME, maybe...

  /**
   * Process the actions of a message.
   * @param message The message to process.
   * @param content The content of the message to process actions from.
   */

  /**
   * Evaluate the message and state using the registered evaluators.
   * @param message The message to evaluate.
   * @param state The state of the agent.
   * @param didRespond Whether the agent responded to the message.~
   * @param callback The handler callback
   * @returns The results of the evaluation.
   */
    // v2 now takes responses: Memory[]

  /**
   * Ensure the existence of a participant in the room. If the participant does not exist, they are added to the room.
   * @param userId - The user ID to ensure the existence of.
   * @throws An error if the participant cannot be added.
   */
    // WRITE ME

  /**
   * Ensure the existence of a user in the database. If the user does not exist, they are added to the database.
   * @param userId - The user ID to ensure the existence of.
   * @param userName - The user name to ensure the existence of.
   * @returns
   */

  /**
   * Ensure the existence of a room between the agent and a user. If no room exists, a new room is created and the user
   * and agent are added as participants. The room ID is returned.
   * @param roomId - The room ID to create a room with.
   * @returns The room ID of the room between the agent and the user.
   * @throws An error if the room cannot be created.
   */

  /**
   * Compose the state of the agent into an object that can be passed or used for response generation.
   * @param message The message to compose the state from.
   * @returns The state of the agent.
   */

    // get memories

    // use v1 formatMessage


} // namespace elizaos
