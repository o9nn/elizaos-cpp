#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// I want to create an action that lets anyone create or update a component for an entity.
// Components represent different sources of data about an entity (telegram, twitter, etc)
// Sources can be registered by plugins or inferred from room context and available components
// The action should first check if the component exists for the entity, and if not, create it.
// We want to use an LLM (runtime.useModel) to generate the component data.
// We should include the prior component data if it exists, and have the LLM output an update to the component.
// sourceEntityId represents who is making the update, entityId is who they are talking about

/**
 * Component Template for Task: Extract Source and Update Component Data
 *
 * @type {string}
 */
/**
 * Component Template for extracting source and updating component data.
 *
 * @type {string}
 */

    // Component-specific fields
    // e.g. username, username, displayName, etc.

/**
 * Action for updating contact details for a user entity.
 *
 * @name UPDATE_ENTITY
 * @description Add or edit contact details for a user entity (like twitter, discord, email address, etc.)
 *
 * @param {IAgentRuntime} _runtime - The runtime environment.
 * @param {Memory} _message - The message data.
 * @param {State} _state - The current state.
 * @returns {Promise<boolean>} Returns a promise indicating if validation was successful.
 *
 * @param {IAgentRuntime} runtime - The runtime environment.
 * @param {Memory} message - The message data.
 * @param {State} state - The current state.
 * @param {any} _options - Additional options.
 * @param {HandlerCallback} callback - The callback function.
 * @param {Memory[]} responses - Array of responses.
 * @returns {Promise<void>} Promise that resolves after handling the update entity action.
 *
 * @example
 * [
 *    [
 *      {
 *        name: "{{name1}}",
 *        content: {
 *          text: "Please update my telegram username to @dev_guru",
 *        },
 *      },
 *      {
 *        name: "{{name2}}",
 *        content: {
 *          text: "I've updated your telegram information.",
 *          actions: ["UPDATE_ENTITY"],
 *        },
 *      },
 *    ],
 *    ...
 * ]
 */

    // Check if we have any registered sources or existing components that could be updated
    // const worldId = message.roomId;
    // const agentId = runtime.agentId;

    // // Get all components for the current room to understand available sources

    // // Get source types from room components
    // const availableSources = new Set(roomComponents.map(c => c.type));

      // Handle initial responses

      // First, find the entity being referenced

      // Get existing component if it exists - we'll get this after the LLM identifies the source

      // Generate component data using the combined template

      // Parse the generated data

      // Now that we know the component type, get the existing component if it exists

      // Create or update the component


} // namespace elizaos
