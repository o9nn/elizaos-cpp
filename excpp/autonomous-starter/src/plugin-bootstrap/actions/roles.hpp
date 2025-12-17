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



/**
 * Determines if a user with a given current role can modify the role of another user to a new role.
 * @param {Role} currentRole - The current role of the user attempting to modify the other user's role.
 * @param {UUID} requesterId - The ID of the entity attempting the change.
 * @param {Role | null} targetRole - The target user's current role. Can be null if the user does not exist.
 * @param {UUID} targetId - The ID of the entity whose role is being changed.
 * @param {Role} newRole - The new role that the current user is attempting to set for the target user.
 * @returns {boolean} Returns true if the user can modify the role, false otherwise.
 */
  // User's can't change their own role
    // Allow owner to change their own role if needed (e.g. demotion, though UI should prevent illogical changes)
    // Or disallow entirely: return false;
    // For now, let's assume owners can, others cannot change their own.

    // Owners can do everything
    // Admins can only create/modify users up to their level
    // Normal users can't modify roles

/**
 * Template for extracting role assignments from a conversation.
 *
 * @type {string} extractionTemplate - The template string containing information about the task, server members, available roles, recent messages, current speaker role, and extraction instructions.
 * @returns {string} JSON format of role assignments if valid role assignments are found, otherwise an empty array.
 */

/**
 * Interface representing a role assignment to a user.
 */
struct RoleAssignment {
    std::string entityId;
    Role newRole;
};

/**
 * Represents an action to update the role of a user within a server.
 * @typedef {Object} Action
 * @property {string} name - The name of the action.
 * @property {string[]} similes - The similar actions that can be performed.
 * @property {string} description - A description of the action and its purpose.
 * @property {Function} validate - A function to validate the action before execution.
 * @property {Function} handler - A function to handle the execution of the action.
 * @property {ActionExample[][]} examples - Examples demonstrating how the action can be used.
 */

    // Only activate in group chats where the feature is enabled

      // First, check if this is a supported channel type
      // Then, check if we have a server ID

    // Extract needed values from message and state

    // First, get the world for this server

    // Get the entities for this room

    // Get the role of the requester

    // Construct extraction prompt

    // Extract role assignments using type-safe model call

    // Process each role assignment

      // Validate role modification permissions

      // Update role in world metadata

    // Save updated world metadata if any changes were made


} // namespace elizaos
