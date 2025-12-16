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
 * Expands tilde path to current working directory
 */


  if (filepath.startsWith('~')) {
    return path.join(process.cwd(), filepath.slice(1));
  }

  return filepath;
}

/**
 * Resolves the PGlite directory with proper fallbacks
 */


  // Try environment variable
  const envFile = '.env';
  if (fs.existsSync(envFile)) {
    dotenv.config({ path: envFile });
  }

  const envDir = process.env.PGLITE_DIR;
  if (envDir) {
    return expandTildePath(envDir);
  }

  // Use fallback directory if provided
  if (fallbackDir) {
    return expandTildePath(fallbackDir);
  }

  // Default fallback
  return expandTildePath('~/eliza/data');
}

} // namespace elizaos
