#include "providers/helius.hpp"
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
const heliusPlugin: Plugin = {
  name: 'helius',
  description: 'Helius data plugin',
  actions: [],
  evaluators: [],
  providers: [heliusProvider],
};

default heliusPlugin;

} // namespace elizaos
