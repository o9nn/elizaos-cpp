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
#include "actions/edit.hpp"
#include "actions/interactive.hpp"
#include "actions/list.hpp"
#include "actions/reset.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Create command for managing environment variables

// List subcommand

// Edit local subcommand

// Reset subcommand

// Interactive mode

// Default command (show help if no subcommand provided)
  // Show available subcommands

// Re-for backward compatibility
* from './actions/edit';
* from './actions/interactive';
* from './actions/list';
* from './actions/reset';
* from './types';
* from './utils/directory-operations';
* from './utils/file-operations';
* from './utils/validation';

} // namespace elizaos
