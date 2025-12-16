#include "actions/timeout.hpp"
#include "communityService.hpp"
#include "providers/timeout.hpp"
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

const communityManagerPlugin: Plugin = {
  name: 'community-manager',
  description: 'Community Manager Plugin for Eliza',
  evaluators: [],
  providers: [timeoutUserProvider],
  actions: [timeoutUser],
  services: [CommunityManagerService],
};

default communityManagerPlugin;

} // namespace elizaos
