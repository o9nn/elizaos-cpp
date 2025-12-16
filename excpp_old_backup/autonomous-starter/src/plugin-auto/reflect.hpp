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



/**
 * Template for generating dialog and actions for a character.
 *
 * @type {string}
 */

/**
 * Represents an action that allows the agent to reflect to the current conversation with a generated message.
 *
 * This action can be used as an acknowledgement at the beginning of a chain of actions, or as a final response at the end of a chain of actions.
 *
 * @typedef {Object} reflectAction
 * @property {string} name - The name of the action ("REFLECT").
 * @property {string[]} similes - An array of similes for the action.
 * @property {string} description - A description of the action and its usage.
 * @property {Function} validate - An asynchronous function for validating the action runtime.
 * @property {Function} handler - An asynchronous function for handling the action logic.
 * @property {ActionExample[][]} examples - An array of example scenarios for the action.
 */
    // Find all responses with REFLECT action and text

    // If we found any existing responses, use them and skip LLM
        await callback(responseContent);

    // Only generate response using LLM if no suitable response was found

    await callback(memory.content);

} // namespace elizaos
