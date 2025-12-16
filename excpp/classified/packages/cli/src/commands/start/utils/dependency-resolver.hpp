#include "elizaos/core.hpp"
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

/**
 * Resolve plugin dependencies with circular dependency detection
 *
 * Performs topological sorting of plugins to ensure dependencies are loaded in the correct order, with support for test dependencies.
 */
" not found and will be skipped.`);
      return;
    }
    if (visited.has(pluginName)) return;
    if (visiting.has(pluginName)) {
      logger.error(`Circular dependency detected involving plugin: ${pluginName}`);
      return;
    }

    visiting.add(pluginName);
    const plugin = availablePlugins.get(pluginName);
    if (plugin) {
      const deps = [...(plugin.dependencies || [])];
      if (isTestMode) {
        deps.push(...(plugin.testDependencies || []));
      }
      for (const dep of deps) {
        visit(dep);
      }
    }
    visiting.delete(pluginName);
    visited.add(pluginName);
    resolutionOrder.push(pluginName);
  }

  for (const name of availablePlugins.keys()) {
    if (!visited.has(name)) {
      visit(name);
    }
  }

  const finalPlugins = resolutionOrder
    .map((name) => availablePlugins.get(name))
    .filter((p) => p) as Plugin[];

  logger.info(`Final plugins being loaded: ${finalPlugins.map((p) => p.name).join(', ')}`);

  return finalPlugins;
}

} // namespace elizaos
