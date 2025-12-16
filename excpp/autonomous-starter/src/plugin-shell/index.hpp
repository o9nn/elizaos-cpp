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

// Shell plugin -- give an agent shell access
;
;
;
;
import "./types"; // Ensure module augmentation is loaded

const shellPlugin: Plugin = {
  name: "plugin-shell",
  description:
    "Provides shell access to the agent, allowing it to run commands and view history.",
  actions: [
    runShellCommandAction,
    clearShellHistoryAction,
    killAutonomousAction,
  ],
  providers: [shellProvider],
  services: [ShellService],
  init: async (config, runtime) => {
    // You could add specific initialization logic here if needed
    // For example, checking for required system dependencies for the shell
    // or setting up initial CWD based on config.
    // Ensure the ShellService is registered if not done automatically by core.
    // However, with `services: [ShellService]`, the runtime should handle registration.
  },
};

default shellPlugin;

} // namespace elizaos
