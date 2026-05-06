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
#include "elizaos/core-plugin-v2.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Represents a UUID std::string in the format "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
 * This is a v1 compatibility wrapper for v2 UUID
 */
using UUID = UUIDv1;

/**
 * Helper std::function to safely cast a std::string to strongly typed UUID
 * Wraps V2's validateUuid std::function
 *
 * @param id The std::string UUID to validate and cast
 * @returns The same UUID with branded type information
 * @throws Error if the UUID format is invalid
 */
UUID asUUID(const std::string& id);

/**
 * Generates a UUID from a std::string input
 * Wraps V2's stringToUuid std::function
 *
 * @param input The std::string to convert to a UUID
 * @returns A UUID generated from the input std::string
 */
UUID generateUuidFromString(const std::string& input);

} // namespace elizaos
