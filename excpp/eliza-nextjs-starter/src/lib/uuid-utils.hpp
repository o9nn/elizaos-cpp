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

 // Need to add js-sha1 dependency

/**
 * Converts a Uint8Array to a hexadecimal string.
 */

/**
 * Generates a deterministic UUID (version 5 like, using SHA-1) from a string.
 * Based on the core package's stringToUuid but uses js-sha1 for browser compatibility.
 * @param {string} inputString - The string to hash.
 * @returns {string} The generated UUID string.
 */
std::string generateUUIDFromString(const std::string& inputString);

/**
 * Generates a deterministic Room ID for a specific query and user seed.
 * @param seed Unique user seed.
 * @param query The search query string.
 * @returns Deterministic UUID for the room.
 */
std::string generateQueryRoomId(const std::string& seed, const std::string& query);

} // namespace elizaos
