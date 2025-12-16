#include "action.ts.hpp"
#include "elizaos/core.hpp"
#include "provider.ts.hpp"
#include "service.ts.hpp"
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
import "./types"; // Ensure module augmentation is loaded

const robotPlugin: Plugin = {
  name: "plugin-robot",
  description: "Control screen using robotjs and provide screen context",
  actions: [performScreenAction],
  providers: [screenProvider],
  services: [RobotService],
};

default robotPlugin;

} // namespace elizaos
