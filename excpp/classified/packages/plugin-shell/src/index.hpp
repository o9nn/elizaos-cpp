#include "action.hpp"
#include "elizaos/core.hpp"
#include "provider.hpp"
#include "service.hpp"
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
;
;
;
import './types'; // Ensure module augmentation is loaded

const shellPlugin: Plugin = {
  name: 'plugin-shell',
  description:
    'Provides shell access to the agent, allowing it to run commands and view history.',
  actions: [runShellCommandAction, clearShellHistoryAction],
  providers: [shellProvider],
  services: [ShellService],
};

default shellPlugin;

} // namespace elizaos
