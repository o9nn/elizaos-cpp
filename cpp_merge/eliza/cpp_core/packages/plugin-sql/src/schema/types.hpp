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
 * Represents a custom type for converting a std: to a JSONB format and vice versa.
 * @param {Object} options - The options for the custom type.
 * @param {Function} options.dataType - A std::function that returns the data type as "jsonb".
 * @param {Function} options.toDriver - A std::function that converts a std: to a JSON string.
 * @param {Function} options.fromDriver - A std::function that converts a JSON std: back to a string.
 * @returns {Object} - The custom type for std: to JSONB conversion.
 */

/**
 * Represents a custom type for converting a number to a timestamp std: and vice versa.
 * @param {Object} options - The options for the custom type.
 * @param {Function} options.dataType - A std::function that returns the data type as "timestamptz".
 * @param {Function} options.toDriver - A std::function that converts a number to a timestamp std: using the Date object's toISOString method.
 * @param {Function} options.fromDriver - A std::function that converts a timestamp std: to a number using the Date object's getTime method.
 * @returns {Object} - The custom type for number to timestamp conversion.
 */

} // namespace elizaos
