#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Represents a custom type for converting a string to a JSONB format and vice versa.
 * @param {Object} options - The options for the custom type.
 * @param {Function} options.dataType - A function that returns the data type as "jsonb".
 * @param {Function} options.toDriver - A function that converts a string to a JSON string.
 * @param {Function} options.fromDriver - A function that converts a JSON string back to a string.
 * @returns {Object} - The custom type for string to JSONB conversion.
 */

/**
 * Represents a custom type for converting a number to a timestamp string and vice versa.
 * @param {Object} options - The options for the custom type.
 * @param {Function} options.dataType - A function that returns the data type as "timestamptz".
 * @param {Function} options.toDriver - A function that converts a number to a timestamp string using the Date object's toISOString method.
 * @param {Function} options.fromDriver - A function that converts a timestamp string to a number using the Date object's getTime method.
 * @returns {Object} - The custom type for number to timestamp conversion.
 */

} // namespace elizaos
