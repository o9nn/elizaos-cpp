#include ".types.hpp"
#include ".utils/directory-operations.hpp"
#include ".utils/file-operations.hpp"
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
 * Resolve the PGLite database directory path
 * @returns The path to the PGLite database directory
 */
std::future<std::string> resolvePgliteDir();

/**
 * Reset environment variables and selected folders
 *
 * Provides an interactive menu to reset local environment variables, cache folder, and local database files. Supports auto-confirmation mode for batch operations.
 */
std::future<void> resetEnv(ResetEnvOptions options);

} // namespace elizaos
