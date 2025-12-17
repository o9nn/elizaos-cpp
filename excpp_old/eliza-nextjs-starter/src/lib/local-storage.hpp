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
 * Retrieves the persistent random seed from Local Storage.
 * If no seed exists, generates a new one using crypto.randomUUID() and saves it.
 * Handles potential exceptions during Local Storage access (e.g., in private browsing).
 * @returns {string | null} The seed string or null if Local Storage is unavailable.
 */
std::optional<std::string> getOrGenerateSeed();

} // namespace elizaos
