#include "actions/edit.hpp"
#include "actions/interactive.hpp"
#include "actions/list.hpp"
#include "actions/reset.hpp"
#include "types.hpp"
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



// Create command for managing environment variables

// List subcommand
      await handleListCommand(options);

// Edit local subcommand
      await editEnvVars(options);

// Reset subcommand
      await resetEnv(options);

// Interactive mode
      await showMainMenu(options);

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
