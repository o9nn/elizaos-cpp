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
 * A custom hook for handling partial updates of objects with nested JSONb fields.
 * This hook ensures that updates to nested objects and arrays are properly
 * managed when sending updates to the server.
 *
 * @param initialValue The initial state object
 * @returns A tuple containing:
 *   - The current state object
 *   - A function to update a specific field (handles nested paths)
 *   - A function to add an item to an array field
 *   - A function to remove an item from an array field
 *   - A function to set the entire object
 *   - A function to reset to initial state
 */

  /**
   * Updates a specific field in the object, handling nested paths
   *
   * @param path The path to the field to update (e.g., 'settings.voice.model')
   * @param newValue The new value for the field
   */
      // Handle simple (non-nested) case

      // Handle nested paths

      // Handle arrays in path (e.g., 'style.all.0')

        // Ensure we're working with an array and handle it safely

          // Direct array item update
          // Updating a property of an object in an array

      // Special case for settings.secrets path

      // Handle regular nested objects

  /**
   * Helper function to update a nested object
   */

  /**
   * Adds an item to an array field
   *
   * @param path Path to the array field
   * @param item Item to add
   */

      // Handle simple array field

      // Handle nested array field

      return setNestedValue(prevValue, updatePath, [...currentArray, item]);

  /**
   * Removes an item from an array field
   *
   * @param path Path to the array field
   * @param index Index of the item to remove
   */

      // Handle simple array field

      // Handle nested array field

      return setNestedValue(prevValue, updatePath, newArray);

  /**
   * Helper function to get a nested value from an object
   */

  /**
   * Helper function to set a nested value in an object
   */

  /**
   * Resets to the initial state
   */

  // Special handling for updating the entire settings object
        // Extract settings but remove 'secrets' key to avoid duplication

        // Only include avatar if it's a valid string; otherwise, omit it from the update

        // Create the updated settings object

        // Only add secrets if it was included in the update
          // Create a new secrets object that only contains non-null values

            // If value is null, don't include it (this is how we delete)


} // namespace elizaos
