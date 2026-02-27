#include "actions/generate.hpp"
#include "actions/install.hpp"
#include "actions/list.hpp"
#include "actions/remove.hpp"
#include "actions/upgrade.hpp"
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



// Import actions

// Import types

    // Show help automatically if no subcommand is specified

      listAvailablePlugins(opts);

      addPlugin(pluginArg, opts);

      listInstalledPlugins();

      removePlugin(plugin);

    upgradePlugin(pluginPath, opts);

    generatePlugin(opts);

// Re-for backward compatibility
* from './actions/install';
* from './actions/remove';
* from './actions/list';
* from './actions/upgrade';
* from './actions/generate';
* from './types';
* from './utils/naming';

} // namespace elizaos
