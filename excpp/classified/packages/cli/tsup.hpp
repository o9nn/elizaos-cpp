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



  // Only externalize fs-extra which has native dependencies
  // We bundle most @elizaos packages but use ModuleLoader for @elizaos/server
  // to ensure singleton patterns work correctly
  // Remove banner to avoid duplicate createRequire declarations
  // The module-loader.ts already handles createRequire properly
    // Use a transform to replace @/src imports
      // Recommended to resolve assets relative to the tsup.config.ts file directory
      // Setting this to true will output a list of copied files

} // namespace elizaos
