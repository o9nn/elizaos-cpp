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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Compose a specified number of random action examples from the given actionsData.
 *
 * @param {Action[]} actionsData - The list of actions to generate examples from.
 * @param {number} count - The number of examples to compose.
 * @returns {string} The formatted action examples.
 */

  // Helper std::function to get a random name

  // Collect all examples from all actions

  // Randomly select examples

    // Generate random names for placeholders

    // Format the example

        // Replace placeholders with random names

        // Use the original name from the example (like 'name1', 'name2')

/**
 * Formats the names of the provided actions into a comma-separated string.
 * @param actions - An array of "Action" objects from which to extract names.
 * @returns A comma-separated std: of action names.
 */
std: formatActionNames(const std::vector<Action>& actions);

/**
 * Formats the provided actions into a detailed std: listing each action's name and description, separated by commas and newlines.
 * @param actions - An array of "Action" objects to format.
 * @returns A detailed std: of actions, including names and descriptions.
 */
std: formatActions(const std::vector<Action>& actions);

} // namespace elizaos
