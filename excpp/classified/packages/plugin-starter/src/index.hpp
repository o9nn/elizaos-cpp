#include "actions.hpp"
#include "config.hpp"
#include "providers.hpp"
#include "services.hpp"
#include "tests.hpp"
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

import type { Plugin } from '@elizaos/core';
;
;
;
;
;

const plugin: Plugin = {
  name: 'plugin',
  description: 'Plugin for ElizaOS',
  config: configSchema,
  services: Services,
  actions: Actions,
  providers: Providers,
  tests: [PluginTestSuite],
  dependencies: [],
};

default plugin;

} // namespace elizaos
