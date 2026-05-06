#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// action: SEND_MESSAGE
// send message to a user or room (other than this room we are in)

/**
 * Task: Extract Target and Source Information
 *
 * Recent Messages:
 * {{recentMessages}}
 *
 * Instructions:
 * Analyze the conversation to identify:
 * 1. The target type (user or room)
 * 2. The target platform/source (e.g. telegram, discord, etc)
 * 3. Any identifying information about the target
 *
 * Return a JSON object with:
 * {
 *   "targetType": "user|room",
 *   "source": "platform-name",
 *   "identifiers": {
 *     // Relevant identifiers for that target
 *     // e.g. username, roomName, etc.
 *   }
 * }
 *
 * Example outputs:
 * For "send a message to @dev_guru on telegram":
 * {
 *   "targetType": "user",
 *   "source": "telegram",
 *   "identifiers": {
 *     "username": "dev_guru"
 *   }
 * }
 *
 * For "post this in #announcements":
 * {
 *   "targetType": "room",
 *   "source": "discord",
 *   "identifiers": {
 *     "roomName": "announcements"
 *   }
 * }
 *
 * Make sure to include the ```json``` tags around the JSON object.
 */
/**
 * Task: Extract Target and Source Information
 *
 * Recent Messages:
 * {{recentMessages}}
 *
 * Instructions:
 * Analyze the conversation to identify:
 * 1. The target type (user or room)
 * 2. The target platform/source (e.g. telegram, discord, etc)
 * 3. Any identifying information about the target
 *
 * Return a JSON object with:
 * {
 *    "targetType": "user|room",
 *    "source": "platform-name",
 *    "identifiers": {
 *      // Relevant identifiers for that target
 *      // e.g. username, roomName, etc.
 *    }
 * }
 *
 * Example outputs:
 * 1. For "send a message to @dev_guru on telegram":
 * {
 *    "targetType": "user",
 *    "source": "telegram",
 *    "identifiers": {
 *      "username": "dev_guru"
 *    }
 * }
 *
 * 2. For "post this in #announcements":
 * {
 *    "targetType": "room",
 *    "source": "discord",
 *    "identifiers": {
 *      "roomName": "announcements"
 *    }
 * }
 *
 * Make sure to include the `json` tags around the JSON object.
 */

    // Relevant identifiers for that target
    // e.g. username, roomName, etc.

/**
 * Represents an action to send a message to a user or room.
 *
 * @typedef {Action} sendMessageAction
 * @property {string} name - The name of the action.
 * @property {string[]} similes - Additional names for the action.
 * @property {string} description - Description of the action.
 * @property {function} validate - Asynchronous function to validate if the action can be executed.
 * @property {function} handler - Asynchronous function to handle the action execution.
 * @property {ActionExample[][]} examples - Examples demonstrating the usage of the action.
 */

    // Check if we have permission to send messages

    // Get all components for the current room to understand available sources

    // Get source types from room components

    // TODO: Add ability for plugins to register their sources
    // const registeredSources = runtime.getRegisteredSources?.() || [];
    // availableSources.add(...registeredSources);

      // Handle initial responses

      // Extract target and source information

        // Try to find the target user entity

        // Get the component for the specified source

        // Send the message using the appropriate client

        // Try to find the target room
          // Match room name from identifiers

        // Send the message to the room


} // namespace elizaos
