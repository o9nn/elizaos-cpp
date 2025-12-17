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
 * Decodes a base64 encoded string with proper UTF-8 handling
 * @param encodedString - The base64 encoded string to decode
 * @returns The decoded string with proper UTF-8 character support
 */
std::string decodeBase64(const std::string& encodedString);

} // namespace elizaos
