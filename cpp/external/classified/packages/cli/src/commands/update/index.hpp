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
#include "actions/cli-update.hpp"
#include "actions/dependency-update.hpp"
#include "elizaos/core.hpp"
#include "types.hpp"
#include "utils/directory-utils.hpp"
#include "utils/version-utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Main update command
      // Early directory detection for better flow control

      // Determine what to update based on flags and context

      // Handle CLI update

      // Handle package updates
        // If explicitly requested to update packages but not in a valid directory

// Re-for backward compatibility
* from './actions/cli-update';
* from './actions/dependency-update';
* from './types';
* from './utils/version-utils';
* from './utils/package-utils';
* from './utils/environment-utils';
* from './utils/directory-utils';

} // namespace elizaos
