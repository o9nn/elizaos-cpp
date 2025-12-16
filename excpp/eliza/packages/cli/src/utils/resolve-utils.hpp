#include "user-environment.hpp"
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
;
// ; // Replaced by UserEnvironment
;

/**
 * Expands a file path starting with `~` to the project directory.
 *
 * @param filepath - The path to expand.
 * @returns The expanded path.
 */

  return filepath;
}

/**
 * Resolves the path to the `.env` file, searching only within the start directory or
 * optionally up to a boundary directory (e.g., a monorepo root).
 *
 * @param startDir - Directory to begin the lookup (default: current working directory).
 * @param boundaryDir - Optional directory at which to stop searching upward.
 * @returns The path to the found `.env` file, or a path to `.env` in startDir if none found.
 */

  let current = root;
  while (true) {
    const candidate = path.join(current, '.env');
    if (existsSync(candidate)) {
      return candidate;
    }
    if (stopAt && current === stopAt) {
      break;
    }
    const parent = path.dirname(current);
    if (parent === current) {
      break;
    }
    current = parent;
  }
  return path.join(root, '.env');
}

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
std::future<std::string> resolvePgliteDir(std::optional<std::string> dir, std::optional<std::string> fallbackDir););
  }

  // The fallbackDir passed from getElizaDirectories will be monorepoRoot + '.elizadb' or similar.
  // If fallbackDir is not provided (e.g. direct call to resolvePgliteDir),
  // then we construct the default path using projectRoot.
  const defaultBaseDir = path.join(projectRoot, '.eliza', '.elizadb');

  const base = dir ?? process.env.PGLITE_DATA_DIR ?? fallbackDir ?? defaultBaseDir;

  // Resolve and migrate legacy default (<projectRoot>/.elizadb) if detected
  const resolved = expandTildePath(base, projectRoot);
  const legacyPath = path.join(projectRoot, '.elizadb');
  if (resolved === legacyPath) {
    const newPath = path.join(projectRoot, '.eliza', '.elizadb');
    process.env.PGLITE_DATA_DIR = newPath;
    return newPath;
  }

  return resolved;
}

} // namespace elizaos
