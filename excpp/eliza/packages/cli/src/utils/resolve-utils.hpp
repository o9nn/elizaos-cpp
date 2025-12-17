#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "user-environment.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



//  // Replaced by UserEnvironment

/**
 * Expands a file path starting with `~` to the project directory.
 *
 * @param filepath - The path to expand.
 * @returns The expanded path.
 */
    // If ~ means project root, use projectRootForTilde. If it means OS home, os.homedir() would be used.
    // Assuming ~ means project root in this context based on previous behavior with cwd.

/**
 * Resolves the path to the `.env` file, searching only within the start directory or
 * optionally up to a boundary directory (e.g., a monorepo root).
 *
 * @param startDir - Directory to begin the lookup (default: current working directory).
 * @param boundaryDir - Optional directory at which to stop searching upward.
 * @returns The path to the found `.env` file, or a path to `.env` in startDir if none found.
 */
  // If no boundary provided, only consider .env in the start directory

/**
 * Resolves the directory used for PGlite database storage.
 *
 * Resolution order:
 * 1. The `dir` argument if provided.
 * 2. The `PGLITE_DATA_DIR` environment variable.
 * 3. The `fallbackDir` argument if provided.
 * 4. `./.eliza/.elizadb` relative to the current working directory.
 *
 * @param dir - Optional directory preference.
 * @param fallbackDir - Optional fallback directory when env var is not set.
 * @returns The resolved data directory with any tilde expanded.
 */
std::future<std::string> resolvePgliteDir(std::optional<std::string> dir, std::optional<std::string> fallbackDir);

} // namespace elizaos
