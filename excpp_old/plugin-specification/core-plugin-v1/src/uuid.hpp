#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core-plugin-v2.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Represents a UUID string in the format "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
 * This is a v1 compatibility wrapper for v2 UUID
 */
using UUID = UUIDv1;

/**
 * Helper function to safely cast a string to strongly typed UUID
 * Wraps V2's validateUuid function
 *
 * @param id The string UUID to validate and cast
 * @returns The same UUID with branded type information
 * @throws Error if the UUID format is invalid
 */
UUID asUUID(const std::string& id);

/**
 * Generates a UUID from a string input
 * Wraps V2's stringToUuid function
 *
 * @param input The string to convert to a UUID
 * @returns A UUID generated from the input string
 */
UUID generateUuidFromString(const std::string& input);

} // namespace elizaos
