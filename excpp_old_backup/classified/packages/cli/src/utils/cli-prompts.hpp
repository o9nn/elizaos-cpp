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
 * Prompts the user with a text input and optional navigation options.
 * @param {string} label - The label to display to the user.
 * @param {string} initial - The initial value for the input (default is an empty string).
 * @param {(val: string) => true | string} validate - Optional validation function for the input.
 * @returns {Promise<string>} The user's input after processing any navigation commands.
 */

/**
 * Prompts the user to enter multiple items for a specified field name.
 *
 * @param {string} fieldName - The name of the field being prompted for.
 * @param {string[]} initial - The initial values to display and allow the user to modify.
 * @returns {Promise<string[]>} The array of strings containing the user-entered values.
 */

/**
 * Asynchronous function that prompts the user with a confirmation message and returns a boolean value based on their response.
 * @param {string} message - The message to display for the confirmation prompt.
 * @returns {Promise<boolean>} A Promise that resolves to a boolean value representing the user's confirmation choice.
 */

std::future<bool> confirmAction(const std::string& message);

} // namespace elizaos
