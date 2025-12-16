#include "..utils.hpp"
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

;
;
import type { UUID } from './types';

const uuidSchema = z.string().uuid() as z.ZodType<UUID>;

/**
 * Validate if the given value is a valid UUID.
 *
 * @param {unknown} value - The value to be validated.
 * @returns {UUID | null} The validated UUID value or null if validation fails.
 */


/**
 * Converts a string or number to a UUID.
 *
 * @param {string | number} target - The string or number to convert to a UUID.
 * @returns {UUID} The UUID generated from the input target.
 * @throws {TypeError} Throws an error if the input target is not a string.
 */


} // namespace elizaos
