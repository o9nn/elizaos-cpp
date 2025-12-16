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

/**
 * Expands a file path starting with `~` to the project directory.
 *
 * @param filepath - The path to expand.
 * @returns The expanded path.
 */

  return filepath;
}

/**
 * Resolves the path to the nearest `.env` file.
 *
 * If no `.env` file is found when traversing up from the starting directory,
 * a path to `.env` in the starting directory is returned.
 *
 * @param startDir - The directory to start searching from. Defaults to the
 *   current working directory.
 * @returns The resolved path to the `.env` file.
 */


    const parentDir = path.dirname(currentDir);
    if (parentDir === currentDir) {
      break;
    }
    currentDir = parentDir;
  }

  return path.join(startDir, '.env');
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
);
  }

  const base =
    dir ??
    process.env.PGLITE_DATA_DIR ??
    fallbackDir ??
    path.join(process.cwd(), '.eliza', '.elizadb');

  // Automatically migrate legacy path (<cwd>/.elizadb) to new location (<cwd>/.eliza/.elizadb)
  const resolved = expandTildePath(base);
  const legacyPath = path.join(process.cwd(), '.elizadb');
  if (resolved === legacyPath) {
    const newPath = path.join(process.cwd(), '.eliza', '.elizadb');
    process.env.PGLITE_DATA_DIR = newPath;
    return newPath;
  }

  return resolved;
}

} // namespace elizaos
