#include "bun-exec.hpp"
#include "directory-detection.hpp"
#include "elizaos/core.hpp"
#include "spinner-utils.hpp"
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
 * Builds a project or plugin in the specified directory using the most appropriate available build method.
 *
 * Attempts to run the build script from `package.json` using `bun` or `npm`, or falls back to building
 * with the TypeScript compiler if a `tsconfig.json` is present. Throws an error if no suitable build
 * method is found or if all build attempts fail.
 *
 * note: cleanup on ctrl-c is handled by the calling function (creators.ts) not here
 *
 * @param cwd - The directory containing the project or plugin to build.
 * @param isPlugin - Set to `true` if building a plugin; otherwise, builds a project.
 *
 * @throws {Error} If no build method can be determined or if all build attempts fail.
 */

  // Validate that the project directory exists and use centralized detection

  // Clean dist directory if it exists

    // Read package.json (we already validated it exists)
      // Package has a build script, use bun to run it

    // If we get here, no build script was found
    // For TypeScript projects, try tsc with bunx

    // If all else fails, throw an error

} // namespace elizaos
